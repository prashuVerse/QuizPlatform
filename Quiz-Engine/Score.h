#pragma once
#include <string>
#include "Attempt.h"

class score {
private:
    //no private as of defining core 
public:
    static int scoreCalculate(const Attempt& attempt);
};