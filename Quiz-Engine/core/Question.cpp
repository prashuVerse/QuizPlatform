#include "question.h"

// Updated Constructor implementation
Question::Question(string id, string subject, int difficulty, string text, vector<string> options, vector<int> optionIds, int correctOptId) {
    this->id = id;
    this->subject = subject;
    this->difficulty = difficulty;
    this->text = text;
    this->options = options;
    this->optionIds = optionIds;
    this->correctOptId = correctOptId;
}


bool Question::isCorrect(int selectedOptId) const {
    return selectedOptId == correctOptId;
}

int Question::getdifficulty() const {
    return difficulty;
}

vector<string> Question::getOption() const {
    return options;
}

string Question::getText() const {
    return text;
}

string Question::getId() const {
    return id;
}

int Question::getOptionId(int index) const {
    if (index >= 0 && index < optionIds.size()) {
        return optionIds[index];
    }
    return -1; // Fallback if something goes wrong
}