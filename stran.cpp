#include <iostream>
#include <chrono>
#include <thread>
#include "stran.h"
#include "textures.h"
using namespace std;

// Only define classes that aren't already defined
class Global {
public:
    int xres, yres;
    Texture tex;
};

class X11_wrapper {
public:
    void swapBuffers();
};

// External declarations
extern Global g;
extern X11_wrapper x11;

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

// The Title Screen Function with Animated Falling Text and Additional Instructions
void renderStart()
{
    static float yOffset1 = 200; // Initial vertical offsets for each piece of text
    static float yOffset2 = 250;
    static float yOffset3 = 300;
    
    // Gradually bring text down into position
    if (yOffset1 > 0) yOffset1 -= 5;
    if (yOffset2 > 50) yOffset2 -= 5;
    if (yOffset3 > 100) yOffset3 -= 5;

    glClear(GL_COLOR_BUFFER_BIT);

    // Render background (same as main screen) edited out currently
    // Plan is to put a custom background separate from the main game
    /*
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
    glEnd();
*/
    // Render Falling Text
    Rect rText;
    rText.left = g.xres / 2;
    rText.center = 1;

    // "Janken - Game!" - Falls last
    rText.bot = (g.yres / 2) + yOffset3;
    ggprint16(&rText, 32, 0xffffff, "Welcome!");

    // "To!" - Falls next, below "Janken - Game!"
    rText.bot = (g.yres / 2) + yOffset2;
    ggprint16(&rText, 32, 0xffffff, "To!");

    // "Welcome!" - Falls first, above "To!"
    rText.bot = (g.yres / 2) + yOffset1;
    ggprint16(&rText, 32, 0xfffffff, "Janken - Game!");

    // Once text has finished falling, render additional instructions
    if (yOffset1 <= 0 && yOffset2 <= 50 && yOffset3 <= 100) {
        // Render the additional text in the center
        rText.bot = (g.yres / 2) - 100;  // Position for "Press Tab..." text
        ggprint16(&rText, 32, 0xffffff, "Press Tab to Begin Playing!");

        rText.bot = (g.yres / 2) - 150;  // Position for "Press Backspace..." text
        ggprint16(&rText, 32, 0xffffff, "Press Backspace to see our Credits!");
    }

    x11.swapBuffers();
}
