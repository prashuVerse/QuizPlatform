
#pragma once
#include <string>
#include <vector>
#include "question.h"
using namespace std;

class Quiz {
private:
    vector<Question> questions;
    int timeLimit;
public:
    Quiz(vector<Question> questions, int timeLimit);
    

    //gets timelimit from Quiz class , cannot access directly from other classes , therefore creating a seperate class
    int getTimeLimit() const;
    vector <Question> getQuestion() const;
};
