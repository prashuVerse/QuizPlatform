#pragma once
#include "../persistence/DatabaseStorage.h"
#include "QuizEngine.h"

class StudentService {
private:
    QuizEngine& engine;
    DatabaseStorage& storage;

public:
    StudentService(QuizEngine& engine, DatabaseStorage& storage);
    void run(int student_id);
};