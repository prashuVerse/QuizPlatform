#pragma once //Protects the file from being included twice and causing errors.

#include <vector>
#include "../core/Quiz.h"
#include "../core/Attempt.h"
#include "../persistence/DatabaseStorage.h"

class QuizEngine {
private:
	DatabaseStorage storage; // Responsible for all database interactions, such as loading questions and saving attempts.
      
public:
    void runQuiz(int student_id); //main start button for the quiz
    Quiz createQuiz(std::string& subject); // A helper function that builds the quiz before it starts running.
};