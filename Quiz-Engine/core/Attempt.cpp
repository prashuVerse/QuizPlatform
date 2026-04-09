#include "Attempt.h"

Attempt::Attempt(User user, Quiz quiz) : user(user), quiz(quiz) {
	// Initialize the answers vector with -1 to indicate unanswered questions
	//It looks at the quiz, counts how many questions there are, and immediately makes the answers vector that exact size. It fills every single slot with a -1
	answers.resize(quiz.getQuestion().size(), -1); 
}

void Attempt::submitAnswer(int questionIndex, int answer) { //funciton save user answer
	// Optional but recommended: Check if the index is valid before assigning
	//its a check if user do not mark answer for negative question and user not mark answer for more than toatal question
	if (questionIndex >= 0 && questionIndex < answers.size()) {
		answers[questionIndex] = answer; //if the undex valid, then overwrite the -1 with the actual answer he mark
	}
}

const vector<int>& Attempt::getAllAnswers() const { // return the list of answer user mark for each question
	return answers;
}


//since user and quiz are private members of the Attempt class, we provide these getter functions to allow other parts of the program to access their details without allowing modification. This encapsulation ensures that the integrity of the Attempt object is maintained while still providing necessary information to other components of the system.
const User& Attempt::getUser() const { // hand back the user object associated with this attempt, allowing other parts of the program to access user details without modifying them.
	return user;
}

const Quiz& Attempt::getQuiz() const { // hand back the quiz object associated with this attempt, allowing other parts of the program to access quiz details without modifying them.
	return quiz;
}
