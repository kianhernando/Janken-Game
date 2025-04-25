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

extern const char* battleChoice[];
extern const char* simplifyControls[];


void simonText(Rect *rSimon);
void genNewEnemy(int eDeath);
int grabEnemyHealth(int &eHealth);
int grabPlayerHealth(int &pHealth);
int battleChoiceFunc(int &pHealth, int &eHmakealth);
int compareHealth(int &pHealth, int &eHealth);
int logicSimon(int choice, int enChoice, int &pHealth);
int scoreCalculator(int &pLoseCondition, int &eLoseCondition);
int randGen();
int randItemGen();
int randCommonItem();
int randUncommonItem();
