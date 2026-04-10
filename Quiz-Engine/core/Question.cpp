#include "question.h"  //access the bluprint of the question class

// Updated Constructor implementation

//This is the Constructor. Whenever  DatabaseStorage file pulls a row from MySQL, it calls this exact function to build a brand new Question object in your computer's memory.
Question::Question(string id, string subject, int difficulty, string text, vector<string> options, vector<int> optionIds, int correctOptId) {
    this->id = id;
    this->subject = subject;
    this->difficulty = difficulty;
    this->text = text;
    this->options = options;
    this->optionIds = optionIds;
    this->correctOptId = correctOptId;
}

//This function will only ever answer true or false.
//The database ID of the option the student clicked on.

bool Question::isCorrect(int selectedOptId) const {  //const mean only read the data member of the class and not modify it
    return selectedOptId == correctOptId; //It checks if the ID the student picked perfectly matches the correct answer ID we memorized in the constructor. If it matches, it returns true (Correct!).
}


//these are getter function because we make the data member private so we need to use getter function to access the data member of the class
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
    //the student sees options numbered 1, 2, 3, 4.  suppose he selcet 1 
    // In the code array, those are indexes 0, 1, 2, 3.  then when calling function ans is passed as ans -1 that is 1-1 -> 0
    // But in the actual MySQL database, those options might have IDs like 405, 406, 407, 408. return option[0]-> 405
    //When the student types "1" (index 0), you pass 0 into this function.
    if (index >= 0 && index < optionIds.size()) {
        return optionIds[index];
    }
    return -1; // Fallback if something goes wrong
}