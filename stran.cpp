#include <iostream>
#include <chrono>
#include <thread>
#include "stran.h"
using namespace std;

/*
//The Title Screen Function
void renderStart()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Render background (same as main screen)
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
    glEnd();

    // Render new text or UI elements
    Rect rText;
    rText.left = g.xres / 2;
    rText.bot = g.yres / 2;
    rText.center = 1;
    ggprint8b(&rText, 16, 0xffffff, "Alternate Screen Active");

    x11.swapBuffers();
}
*/


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
    ggprint8b(rSteven, 16, 0xffffff, "Steven");
}
