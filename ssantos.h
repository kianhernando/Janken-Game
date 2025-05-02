#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Global;
extern Global g;

class Item {
    public:
        Item(const std::string& itemName = "");
        std::string getName() const { return name; }
        
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

    private:
        std::string name;
};

enum optionSelect{
    ROCK,
    PAPER,
    SCISSORS
};

extern const char* battleChoice[];
extern const char* simplifyControls[];
extern const char* loseDialogue[];
extern const char* tieDialogue[];

void simonText(Rect *rSimon);
void genNewEnemy(int eDeath);
int grabEnemyHealth(int &eHealth);
int grabPlayerHealth(int &pHealth);
int battleChoiceFunc(int &pHealth, int &eHmakealth);
int compareHealth(int &pHealth, int &eHealth);
bool blockDamage(bool block);
int logicSimon(int choice, int enChoice, int &pHealth, bool &blocked);
int scoreCalculator(int &pLoseCondition, int &eLoseCondition);
int randGen();
int randItemGen();
int randCommonItem();
int randUncommonItem();
