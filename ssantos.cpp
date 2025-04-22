// Original author: Simon Santos
// Date: March 08, 2025
#include <cstdio>
#include <unistd.h>
#include <GL/gl.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include "ssantos.h"
#include "khernando.h"

extern Global g;
extern Player player;
extern Enemy enemy;
extern void init_hp(int health);
extern void changeHealthBar(int health);
std::random_device rd;
std::mt19937 gen(rd());

struct battleState{
    int playerLoseInteraction;
    int enemyLoseInteraction;
    int playerDeath;
    int enemyDeath;
} bState;
int initScore = 100;
int totalScore = 0;


 void Item::swordItem()
 {
     boostedDamage = 1;
 }
 
 void Item::bowItem()
 {
     boostedDamage = 1;
 }
 
 void Item::spearItem()
 {
     boostedDamage = 1;
 }
 
 void Item::axeItem()
 {
     boostedDamage = 1;
 }
 
 void Item::hammerItem()
 {
     boostedDamage = 1;
 }
 
 void Item::shieldItem()
 {
     protectedDamage = 1;
 }
 
 void Item::armorItem()
 {
     protectedDamage = 1;
 }
 
 void Item::helmItem()
 {
     protectedDamage = 1;
 }
 
 void Item::serratedSword()
 {
     crit = 2;
 }
 
 void Item::thornedArmor()
 {
     thorns = 2;
 }

void simonText(Rect *rSimon)
{
    ggprint8b(rSimon, 16, 0xffffff, "Simon");
}

int logicSimon(int choice, int enChoice, int &pHealth, int &eHealth) 
{
    if (choice == ROCK) {
        player.changeImage("assets/player/rock_x.png");
        if (enChoice == 2) {
            enemy.changeImage("assets/enemy/scissors.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            printf("amount of times enemy has lost: %i\n", 
                bState.enemyLoseInteraction);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        } else if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            printf("amount of times player has lost: %i\n", 
                bState.playerLoseInteraction);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/rock.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    else if (choice == PAPER) {
        player.changeImage("assets/player/paper_x.png");
        if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            printf("amount of times enemy has lost: %i\n", 
                bState.enemyLoseInteraction);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        } else if (enChoice == 2) {
            enemy.changeImage("assets/enemy/scissors.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            printf("amount of times player has lost: %i\n", 
                bState.playerLoseInteraction);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/paper.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    } else if (choice == SCISSORS) {
        player.changeImage("assets/player/scissors_x.png");
        if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            printf("amount of times enemy has lost: %i\n", 
                bState.enemyLoseInteraction);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        } else if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            printf("amount of times player has lost: %i\n", 
                bState.playerLoseInteraction);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/scissors.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (pHealth == 0) {
        printf("Player has died!\n");
        scoreCalculator(bState.playerLoseInteraction, 
            bState.enemyLoseInteraction);
        ++bState.playerDeath;
    } else if (eHealth == 0) {
        printf("Enemy has died!\n");
        scoreCalculator(bState.playerLoseInteraction, 
            bState.enemyLoseInteraction);
        ++bState.enemyDeath;
    }
    printf("\n");
    usleep(700000);
    return 0;
}

//Random Gen for enemy choice
int randGen()
{
    //======================================================================
    //Optimization Change
    //previous code:
    //  srand(time(0));
    //  int randomNum  = rand() % 3;
    //to code below
    //change was made to make randGen more random
    //======================================================================
    //set a range for the num to generate between 0 and 2
    std::uniform_int_distribution<> dis(0, 2);
    int randNum = dis(gen);

    return randNum;
}

int randItemGen()
{
    std::uniform_int_distribution<> dis(1, 10);
    int randItem = dis(gen);
    if (randItem <= 10 && randItem > 9) {
        printf("Ultra Rare Item: %i\n", randItem);
        printf("serrated sword acquired\n");
        fflush(stdout);
    } else if (randItem <= 9 && randItem > 8) {
        printf("Rare Item: %i\n", randItem);
        printf("thorned armor acquired\n");
        fflush(stdout);
    } else if (randItem <= 8 && randItem > 5) {
        printf("Uncommon Item: %i\n", randItem);
        randUncommonItem();
        fflush(stdout);
    } else if (randItem <= 5 && randItem > 0) {
        printf("Common Item: %i\n", randItem);
        randCommonItem();
        fflush(stdout);
    }
    printf("Item: %i\n", randItem);
    fflush(stdout);

    return randItem;
}
 
int randCommonItem()
{
    std::uniform_int_distribution<> dis(0, 4);
    int randCommon = dis(gen);
    if (randCommon == 0) {
        printf("sword acquired\n");
    } else if (randCommon == 1) {
        printf("bow acquired\n");
    } else if (randCommon == 2) {
        printf("spear acquired\n");
    } else if (randCommon == 3) {
        printf("axe acquired\n");
    } else if (randCommon == 4) {
        printf("hammer acquired\n");
    }

    return randCommon;
}
 
int randUncommonItem()
{
    std::uniform_int_distribution<> dis(0, 2);
    int randUncommon = dis(gen);
    if (randUncommon == 0) {
        printf("shield acquired\n");
    } else if (randUncommon == 1) {
        printf("helm acquired\n");
    } else if (randUncommon == 2) {
        printf("armor acquired\n");
    }
    
    return randUncommon;
}

int scoreCalculator(int &pLoseCondition, int &eLoseCondition)
{
    printf("initial score is: %i\n", initScore);
    int scoreMultCondition = eLoseCondition - pLoseCondition;
    if (scoreMultCondition <= 10 && scoreMultCondition > 8) {
        totalScore = initScore * 5;
    } else if (scoreMultCondition <= 8 && scoreMultCondition > 6) {
        totalScore = initScore * 4;
    } else if (scoreMultCondition <= 6 && scoreMultCondition > 4) {
        totalScore = initScore * 3;
    } else if (scoreMultCondition <= 4 && scoreMultCondition > 2) {
        totalScore = initScore * 2;
    } else if (scoreMultCondition <= 2 && scoreMultCondition > 0) {
        totalScore = initScore * 1;
    }
    printf("the total score is: %i\n", totalScore);
    return totalScore;
}