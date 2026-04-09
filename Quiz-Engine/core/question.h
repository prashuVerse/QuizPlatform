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
    vector<int> optionIds;  // NEW: Added this
    int correctOptId;       // NEW: Added this

public:
    // NEW: Updated constructor to match .cpp
    Question(string id, string subject, int difficulty, string text, vector<string> options, vector<int> optionIds, int correctOptId);

    bool isCorrect(int selectedOptId) const;
    int getdifficulty() const;
    vector<string> getOption() const;
    string getText() const;
    string getId() const;             // NEW: Added this
    int getOptionId(int index) const; // NEW: Added this
};