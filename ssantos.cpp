// Original author: Simon Santos
// Date: March 08, 2025
#include <cstdio>
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

/*
class entityHealth {
    public:
        int userHealth = 100;
        int oppHealth = 100;
} e;
*/

void simonText(Rect *rSimon)
{
    ggprint8b(rSimon, 16, 0xffffff, "Simon");
}

int logicSimon(int choice, int enChoice, int &pHealth, int &eHealth) 
{
    if (choice == ROCK) {
        if (enChoice == 1) {
            printf("Player Wins! Enemy loses 20 HP!\n");
            eHealth -= 20;
            printf("%i\n", eHealth);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        }
        if (enChoice == 2) {
            printf("Player Loses! Player loses 20 HP!\n");
            pHealth -= 20;
            printf("%i\n", pHealth);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (choice == PAPER) {
        if (enChoice == 0) {
            printf("Player Wins! Enemy loses 20 HP!\n");
            fflush(stdout);
        }
        if (enChoice == 2) {
            printf("Player Loses! Player loses 20 HP!\n");
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (choice == SCISSORS) {
        if (enChoice == 1) {
            printf("Player Wins! Enemy loses 20 HP!\n");
            fflush(stdout);
        }
        if (enChoice == 0) {
            printf("Player Loses! Player loses 20 HP!\n");
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    printf("\n");
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
