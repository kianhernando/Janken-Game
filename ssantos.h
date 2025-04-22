#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Global;
extern Global g;

class Item {
    public:
        Item();
        void swordItem();
        void bowItem();
        void spearItem();
        void axeItem();
        void hammerItem();
        void shieldItem();
        void helmItem();
        void armorItem();
        void serratedSword();
        void thornedArmor();

        int boostedDamage;
        int protectedDamage;
        int crit;
        int thorns;
};

enum optionSelect{
    ROCK,
    PAPER,
    SCISSORS
};

void simonText(Rect *rSimon);
int logicSimon(int choice, int enChoice, int &pHealth, int &eHealth);
int scoreCalculator(int &pLoseCondition, int &eLoseCondition);
int randGen();
int randItemGen();
int randCommonItem();
int randUncommonItem();