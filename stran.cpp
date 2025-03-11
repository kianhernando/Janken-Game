#include <iostream>
#include <chrono>
#include <thread>
#include "stran.h"
using namespace std;

// Function to start intro screen delay
void showIntroScreen() 
{
    if (true) {
        cout << "===================================" << endl;
        cout << "          Welcome to My Game      " << endl;
        cout << "===================================" << endl;
        cout << "Loading, please wait..." << endl;
    }
    // Simulate a loading delay
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\nStarting the game..." << endl;
}

void startGame() 
{
    // Display the intro screen
    showIntroScreen();
    // Main program or game logic here
    cout << "\nGame has started!" << endl;
}

void stevenText(Rect *rSteven)
{
    ggprint8b(rSteven, 16, 0xffffff, "Steven Tran");
}
