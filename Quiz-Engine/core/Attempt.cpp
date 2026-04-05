#include "Attempt.h"

Attempt::Attempt(User user, Quiz quiz) : user(user), quiz(quiz) {
	// Initialize the answers vector with -1 to indicate unanswered questions
	answers.resize(quiz.getQuestion().size(), -1);
}

void Attempt::submitAnswer(int questionIndex, int answer) {
	// Optional but recommended: Check if the index is valid before assigning
	if (questionIndex >= 0 && questionIndex < answers.size()) {
		answers[questionIndex] = answer;
	}
}

// Fixed missing <int> in the return type
const vector<int>& Attempt::getAllAnswers() const {
	return answers;
}

// Added the missing getUser implementation
const User& Attempt::getUser() const {
	return user;
}

// Completed the cut-off getQuiz implementation
const Quiz& Attempt::getQuiz() const {
	return quiz;
}
