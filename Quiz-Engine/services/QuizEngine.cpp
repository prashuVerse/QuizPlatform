#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
using namespace std;

// Adjust these include paths if your folders are structured differently
#include "../core/Quiz.h"
#include "../core/question.h"
#include "../core/User.h"       
#include "../core/Attempt.h"    
#include "../core/Score.h"
#include "../persistence/DatabaseStorage.h"
#include "QuizEngine.h"

// Note: If you have a Quiz.cpp file, this method belongs there. 
// I am keeping it here because it was in your original snippet!
vector<Question> Quiz::getQuestion() const {
	return questions;
}

Quiz::Quiz(vector<Question> questions, int timeLimit) {
	this->questions = questions;
	this->timeLimit = timeLimit;
}

User::User(string userName) {
	this->userName = userName;
}
// ------------------------------------------

Quiz QuizEngine::createQuiz(string& subject) {
	int no_of_questions;

    storage.showAvailableSubjects();
	// Ask for the subject and number of questions to create the quiz
	cout << "\nEnter the Subject for the quiz: ";
	cin >> subject;

	cout << "How many questions do you want in the quiz? ";
	cin >> no_of_questions;

	// Calls the database file, hands it the subject and the number, 
	// and the database hands back a filled vector of actual questions from MySQL!
	vector<Question> quizQuestions = storage.loadQuestions(subject, no_of_questions);

	int timeLimit = 60;
	Quiz quiz(quizQuestions, timeLimit);
	return quiz;
}

void QuizEngine::runQuiz(int student_id) {
    string subject;
    Quiz quiz = createQuiz(subject);
    vector<Question> all = quiz.getQuestion();

    if (all.empty()) {
        cout << "\n[!] No questions found for this subject. Returning to Dashboard.\n";
        return;
    }

    // Split into Easy / Medium / Hard buckets
    vector<Question> E, M, H;
    for (auto& q : all) {
        if (q.getdifficulty() == 1) E.push_back(q);
        else if (q.getdifficulty() == 2) M.push_back(q);
        else H.push_back(q);
    }

    mt19937 rng(time(nullptr));

    // Remove and return a random question from a pool
    auto pick = [&](vector<Question>& pool) {
        int i = rng() % pool.size();
        Question q = pool[i];
        pool.erase(pool.begin() + i);
        return q;
        };

    // Helper to safely remove a question from a bucket using its Database ID
    auto removeQ = [&](vector<Question>& pool, Question target) {
        auto it = find_if(pool.begin(), pool.end(), [&](Question& q) {
            return q.getId() == target.getId();
            });
        if (it != pool.end()) pool.erase(it);
        };

    User user("Student");
    Attempt attempt(user, quiz);
    int score = 0;
    bool fallback = false;

    // Lambda to ask a question, record the Database Option ID, and check if it's correct
    // (Removed the 'random' parameter entirely, UI is clean now)
    auto ask = [&](Question q, int idx) -> bool {
        const char* label[] = { "Easy", "Medium", "Hard" };

        cout << "\n------------------------------------------------";
        cout << "\nQuestion " << idx + 1 << " of " << all.size() << " [" << label[q.getdifficulty() - 1] << "]\n";
        cout << q.getText() << "\n\n";

        vector<string> opts = q.getOption();
        for (int j = 0; j < (int)opts.size(); j++) {
            cout << "  " << j + 1 << ". " << opts[j] << "\n";
        }

        int ans;
        cout << "\nEnter your answer: ";
        cin >> ans;

        // DB INTEGRATION: Map answer to real Option ID
        int selectedOptId = q.getOptionId(ans - 1);
        attempt.submitAnswer(idx, selectedOptId);

        bool correct = q.isCorrect(selectedOptId);

        // SILENT GRADING: No cout statements here!
        if (correct) {
            score++;
        }

        return correct;
        };

    // First question is completely random from all available questions
    vector<Question> allPool = all;
    Question current = pick(allPool);

    if (current.getdifficulty() == 1) removeQ(E, current);
    else if (current.getdifficulty() == 2) removeQ(M, current);
    else removeQ(H, current);

    bool correct = ask(current, 0);

    for (int i = 1; i < (int)all.size(); i++) {

        if (fallback) {
            vector<Question> left;
            left.insert(left.end(), E.begin(), E.end());
            left.insert(left.end(), M.begin(), M.end());
            left.insert(left.end(), H.begin(), H.end());

            if (left.empty()) break;

            Question q = pick(left);
            if (q.getdifficulty() == 1) removeQ(E, q);
            else if (q.getdifficulty() == 2) removeQ(M, q);
            else removeQ(H, q);

            correct = ask(q, i);
            continue;
        }

        int d = current.getdifficulty();
        vector<Question*> choices;

        // --- YOUR EXACT ADAPTIVE RULES ---
        if (correct) {
            if (d == 1) {
                // Easy & Correct -> Medium or Hard
                for (auto& q : M) choices.push_back(&q);
                for (auto& q : H) choices.push_back(&q);
            }
            else if (d == 2) {
                // Medium & Correct -> Hard
                for (auto& q : H) choices.push_back(&q);
            }
            else if (d == 3) {
                // Hard & Correct -> Hard (If empty, come down to Medium or Easy)
                for (auto& q : H) choices.push_back(&q);
                if (choices.empty()) {
                    for (auto& q : M) choices.push_back(&q);
                    for (auto& q : E) choices.push_back(&q);
                }
            }
        }
        else {
            if (d == 1) {
                // Easy & Wrong -> Easy
                for (auto& q : E) choices.push_back(&q);
            }
            else if (d == 2) {
                // Medium & Wrong -> Easy
                for (auto& q : E) choices.push_back(&q);
            }
            else if (d == 3) {
                // Hard & Wrong -> Medium or Easy
                for (auto& q : M) choices.push_back(&q);
                for (auto& q : E) choices.push_back(&q);
            }
        }

        // If we still run out of valid questions after all your rules, THEN trigger fallback
        if (choices.empty()) {
            fallback = true;
            i--;
            continue;
        }

        // Pick randomly from valid choices
        Question next = *choices[rng() % choices.size()];

        if (next.getdifficulty() == 1) removeQ(E, next);
        else if (next.getdifficulty() == 2) removeQ(M, next);
        else removeQ(H, next);

        current = next;
        correct = ask(current, i);
    }

    // Print Results
    cout << "\n================================================";
    cout << "\n                  QUIZ RESULT                   ";
    cout << "\n================================================";
    cout << "\n Final Score: " << score << " / " << (int)all.size() << "\n";

    // DB INTEGRATION: Dynamically Save Attempt
    int sub_id = storage.getSubjectId(subject);

    if (sub_id != -1) {
        storage.saveAttempt(attempt, student_id, sub_id, score);
    }
    else {
        cout << "\n[!] Error finding subject in database. Attempt not saved.\n";
    }
}