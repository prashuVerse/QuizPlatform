#pragma once

#include <vector>
#include "Quiz.h"
#include "Attempt.h"
#include "DatabaseStorage.h"

class QuizEngine {
private:
    DatabaseStorage storage;

public:
    void runQuiz();
    Quiz createQuiz(std::string& subject);
};