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

void testFunction()
{
    printf("Hello World!\n");
    fflush(stdout);
}

void simonText(Rect *rSimon)
{
    ggprint8b(rSimon, 16, 0xffffff, "Simon");
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
    
    int randomNum = dis(gen);
    
    printf("%i\n", randomNum);

    return 0;
}
