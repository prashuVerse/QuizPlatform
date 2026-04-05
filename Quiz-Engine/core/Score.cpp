#include "Score.h"

// FIX: Capitalized class name
int Score::scoreCalculate(const Attempt& attempt) {
    int score = 0;

    const std::vector<int>& answers = attempt.getAllAnswers();
    const std::vector<Question>& questions = attempt.getQuiz().getQuestion();

    for (int i = 0; i < questions.size(); i++) {
        if (answers[i] != -1 && questions[i].isCorrect(answers[i])) {
            score++;
        }
    }

    return score;
}