#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "Quiz.h"
#include "DatabaseStorage.h"
#include "Score.h"
#include "QuizEngine.h"
#include "question.h"
#include "User.h"       
#include "Attempt.h"    
#include <mysql/jdbc.h> // Mysql connector

// Keeps only the Quiz class method (Question methods are safely inside question.cpp now)
vector<Question> Quiz::getQuestion() const {
	return questions;
}

Quiz QuizEngine::createQuiz(string& subject) {
	int no_of_questions;

	cout << "Enter the Subject for the quiz: ";
	cin >> subject;

	cout << "How many questions do you want in the quiz? ";
	cin >> no_of_questions;

	vector<Question> quizQuestions = storage.loadQuestions(subject, no_of_questions);

	int timeLimit = 60;
	Quiz quiz(quizQuestions, timeLimit);
	return quiz;
}

void QuizEngine::runQuiz(int student_id, string student_name) {
	string subject;
	Quiz quiz = createQuiz(subject);

	// Use the real dynamically passed name!
	User user(student_name);
	Attempt attempt(user, quiz);

	int currentDifficulty;
	int e = 0, m = 0, h = 0; // Indexes for easy, medium, and hard question vectors

	vector<Question> easy, medium, hard;
	vector<Question> allQuestions = quiz.getQuestion();

	// Basic safety check so it doesn't crash if the subject doesn't exist
	if (allQuestions.empty()) {
		cout << "\nNo questions found. Exiting quiz.\n";
		return;
	}

	cout << "Enter the difficulty level you want to start with (1 for Easy, 2 for Medium, 3 for Hard): ";
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
		Question currentQuestion = allQuestions[0];

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

		cout << "\nQuestion (Difficulty: " << currentQuestion.getdifficulty() << "):" << endl;
		cout << currentQuestion.getText() << "\n";

		vector<string> options = currentQuestion.getOption();
		for (int j = 0; j < options.size(); j++) {
			cout << (j + 1) << ". " << options[j] << endl;
		}

		// Take user input 
		int userAnswer;
		cout << "Enter your answer: ";
		cin >> userAnswer;

		// --- DB INTEGRATION: MAP UI INDEX TO DB PRIMARY KEY ---
		int selectedOptId = currentQuestion.getOptionId(userAnswer - 1);
		attempt.submitAnswer(i, selectedOptId);

		// Adaptive Scaling Logic
		if (currentQuestion.isCorrect(selectedOptId)) {
			cout << "Correct!\n";
			if (currentDifficulty < 3) {
				currentDifficulty++;
			}
		}
		else {
			cout << "Incorrect.\n";
			if (currentDifficulty > 1) {
				currentDifficulty--;
			}
		}

		// Clamp difficulty boundaries
		if (currentDifficulty < 1) currentDifficulty = 1;
		if (currentDifficulty > 3) currentDifficulty = 3;
	}

	// Calculate Final Score
	int score = score::scoreCalculate(attempt);

	cout << "\n===== QUIZ RESULT =====\n";
	cout << "Score: " << score << " / " << totalQuestion << endl;

	// --- DB INTEGRATION: SAVE ATTEMPT ---
	// Hardcoded for testing. Matches student_id=1 and sub_id=1 ('DBMS') from your DB.
	int subId = storage.getSubjectId(subject);

	if (subId != -1) {
		// Make sure to pass subId here instead of sub_id
		storage.saveAttempt(attempt, student_id, subId, score);
	}
	else {
		cout << "\n[Error] Could not save results. Subject not found in database.\n";
	}
}