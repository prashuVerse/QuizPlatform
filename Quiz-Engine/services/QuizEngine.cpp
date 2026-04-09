#include <iostream>
#include <vector>
#include <string>
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

	// Creates a dummy user object (just to satisfy the Attempt constructor)
	User user("Student");
	Attempt attempt(user, quiz);

	int currentDifficulty;
	int e = 0, m = 0, h = 0; // Indexes for easy, medium, and hard question vectors

	vector<Question> easy, medium, hard;
	vector<Question> allQuestions = quiz.getQuestion();

	// Basic safety check so it doesn't crash if the subject doesn't exist
	if (allQuestions.empty()) {
		cout << "\n[!] No questions found for this subject. Returning to Dashboard.\n";
		return;
	}

	cout << "\nEnter the difficulty level you want to start with (1 for Easy, 2 for Medium, 3 for Hard): ";
	cin >> currentDifficulty;

	// Filter questions based on difficulty level
	for (auto& q : allQuestions) {
		if (q.getdifficulty() == 1) {
			easy.push_back(q);
		}
		else if (q.getdifficulty() == 2) {
			medium.push_back(q);
		}
		else {
			hard.push_back(q);
		}
	}

	int totalQuestion = allQuestions.size();

	for (int i = 0; i < totalQuestion; i++) {
		Question currentQuestion = allQuestions[0]; // temporary placeholder

		// Pull the question based on the current adaptive difficulty
		if (currentDifficulty == 1 && e < easy.size()) {
			currentQuestion = easy[e++];
		}
		else if (currentDifficulty == 2 && m < medium.size()) {
			currentQuestion = medium[m++];
		}
		else if (currentDifficulty == 3 && h < hard.size()) {
			currentQuestion = hard[h++];
		}
		else {
			// Fallback! If we run out of questions in the current difficulty tier, 
			// give them whatever is left.
			if (m < medium.size()) {
				currentQuestion = medium[m++];
			}
			else if (h < hard.size()) {
				currentQuestion = hard[h++];
			}
			else if (e < easy.size()) {
				currentQuestion = easy[e++];
			}
			else {
				break; // No more questions available
			}
		}

		cout << "\n------------------------------------------------";
		cout << "\nQuestion " << (i + 1) << " of " << totalQuestion << " (Difficulty: " << currentQuestion.getdifficulty() << "):" << endl;
		cout << currentQuestion.getText() << "\n\n";

		vector<string> options = currentQuestion.getOption();
		for (int j = 0; j < options.size(); j++) {
			cout << "  " << (j + 1) << ". " << options[j] << endl;
		}

		// Take user input 
		int userAnswer;
		cout << "\nEnter your answer: ";
		cin >> userAnswer;

		// --- DB INTEGRATION: MAP UI INDEX TO DB PRIMARY KEY ---
		int selectedOptId = currentQuestion.getOptionId(userAnswer - 1);
		attempt.submitAnswer(i, selectedOptId);

		// Adaptive Scaling Logic
		if (currentQuestion.isCorrect(selectedOptId)) {
			cout << "✅ Correct!\n";
			if (currentDifficulty < 3) {
				currentDifficulty++;
			}
		}
		else {
			cout << "❌ Incorrect.\n";
			if (currentDifficulty > 1) {
				currentDifficulty--;
			}
		}

		// Clamp difficulty boundaries just to be safe
		if (currentDifficulty < 1) currentDifficulty = 1;
		if (currentDifficulty > 3) currentDifficulty = 3;
	}

	// Calculate Final Score
	int score = score::scoreCalculate(attempt);

	cout << "\n================================================";
	cout << "\n                  QUIZ RESULT                   ";
	cout << "\n================================================";
	cout << "\n Final Score: " << score << " / " << totalQuestion << "\n";

	// --- DB INTEGRATION: SAVE ATTEMPT ---
	// Fetch the subject ID based on the string the user typed in
	int sub_id = storage.getSubjectId(subject);

	if (sub_id != -1) {
		// Save to MySQL!
		storage.saveAttempt(attempt, student_id, sub_id, score);
	}
	else {
		cout << "\n[!] Error finding subject in database. Attempt not saved.\n";
	}
}