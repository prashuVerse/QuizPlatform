//interface for question
#pragma once
#include <string>
#include <vector>
using namespace std;

class Question {
private:
    string id;
    string subject;
    int difficulty;
    string text;
    vector<string> options;
    int correctIndex;

public:
    Question(string id,string subject,int difficulty,string text, vector<string> options, int correctIndex);

    bool isCorrect(int correctIndex) const;
	int getdifficulty() const;
    vector<string> getOption() const;
    string getText() const;
};

