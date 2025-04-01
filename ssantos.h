#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Global;
extern Global g;

enum optionSelect{
    ROCK,
    PAPER,
    SCISSORS
};

void simonText(Rect *rSimon);
int logicSimon(int choice, int enChoice, int &pHealth, int &eHealth);
int scoreCalculator(int &pLoseCondition, int &eLoseCondition);
int randGen();