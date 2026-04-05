#pragma once

#include <vector>
#include "Quiz.h"
#include "Attempt.h"
#include "DatabaseStorage.h"

class QuizEngine {
private:
    DatabaseStorage storage;

public:
    void runQuiz(int student_id, std::string student_name);
    Quiz createQuiz(std::string& subject);
};