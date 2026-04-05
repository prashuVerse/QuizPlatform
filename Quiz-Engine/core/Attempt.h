#pragma once
#include <string>
#include <vector>
#include "Quiz.h"
//#include "question.h"
#include "User.h"

using namespace std;
// this header file is for user attempt , it has functions which getUsers getQuiz , questions etx. 
class Attempt {
private:
    User user;
    Quiz quiz;
    vector<int> answers;


public:
    Attempt(User user, Quiz quiz);
    void submitAnswer(int questionIndex, int chosenOption);
    const vector<int>& getAllAnswers() const;
    const User& getUser() const;
    const Quiz& getQuiz() const;
};