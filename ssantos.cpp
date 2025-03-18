// Original author: Simon Santos
// Date: March 08, 2025
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include "ssantos.h"

std::random_device rd;
std::mt19937 gen(rd());

void simonText(Rect *rSimon)
{
    ggprint8b(rSimon, 16, 0xffffff, "Simon");
}

int logicSimon(int choice, int enChoice) 
{
    if (choice == 0) {
        if (enChoice == 1) {
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == 2) {
            printf("Player Loses! Player loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Both lose 10 HP!\n");
            fflush(stdout);
        }
    }
    if (choice == 1) {
        if (enChoice == 0) {
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == 2) {
            printf("Player Loses! Player loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Both lose 10 HP!\n");
            fflush(stdout);
        }
    }
    if (choice == 2) {
        if (enChoice == 1) {
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == 0) {
            printf("Player Loses! Player loses 10 HP!\n");
            fflush(stdout);
        }
        if (enChoice == choice) {
            printf("Tie! Both lose 10 HP!\n");
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
