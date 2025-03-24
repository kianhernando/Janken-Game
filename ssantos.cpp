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
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        }
        if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        }
        if (enChoice == choice) {
            enemy.changeImage("assets/enemy/rock.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (choice == PAPER) {
        player.changeImage("assets/player/paper_x.png");
        if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        }
        if (enChoice == 2) {
            enemy.changeImage("assets/enemy/scissors.png");
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        }
        if (enChoice == choice) {
            enemy.changeImage("assets/enemy/paper.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (choice == SCISSORS) {
        player.changeImage("assets/player/scissors_x.png");
        if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            printf("Player Wins! Enemy loses 10 HP!\n");
            eHealth -= 10;
            printf("%i\n", eHealth);
            enemy.changeHealthBar(eHealth);
            fflush(stdout);
        }
        if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            printf("Player Loses! Player loses 10 HP!\n");
            pHealth -= 10;
            printf("%i\n", pHealth);
            player.changeHealthBar(pHealth);
            fflush(stdout);
        }
        if (enChoice == choice) {
            enemy.changeImage("assets/enemy/scissors.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
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