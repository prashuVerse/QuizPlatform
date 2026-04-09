#pragma once //this tell compiler to include this file only once when building your project
#include <string>
#include <vector>
#include "Quiz.h"
//#include "question.h"
#include "User.h"

using namespace std;
// this header file is for user attempt , it has functions which getUsers getQuiz , questions etx. 
class Attempt {
private:
    User user;  //Store specific user who is taking the quiz
    Quiz quiz;  //Store the specific quiz they are attempting
    vector<int> answers; //list of choosen option ids for each question


public:
	Attempt(User user, Quiz quiz); //constructor to initialize the attempt with user and quiz, also initializes answers vector
    void submitAnswer(int questionIndex, int chosenOption); //function to record the user's amswer
    const vector<int>& getAllAnswers() const; // "const" mean to only read the data no change allowed

    //Function to safely read the user and the quiz associated with this attempt.
    const User& getUser() const;
    const Quiz& getQuiz() const;
};