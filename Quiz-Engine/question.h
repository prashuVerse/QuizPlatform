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
    vector<int> optionIds; // Stores the actual opt_id from DB
    int correctOptId;
    int correctIndex;

public:
    Question(string id,string subject,int difficulty,string text, vector<string> options, vector<int> optionIds, int correctOptId);

    bool isCorrect(int correctIndex) const;
	int getdifficulty() const;
    vector<string> getOption() const;
    string getText() const;
    int getOptionId(int index) const;
    string getId() const;
};

