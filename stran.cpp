#include <iostream>
#include <chrono>
#include <thread>
#include "stran.h"
#include <cmath>
using namespace std;

// Forward declare the full class definitions from background.cpp
class Global {
    public:
        int xres, yres;
        Texture tex;
        bool isBackgroundMoving;
        bool encounterEnemy;
        bool showMembers;
        bool showCreditsScreen;
        bool renderStartScreen;
        int playerHealth;
        int enemyHealth;
};

class X11_wrapper {
    public:
        void swapBuffers();
};

// Now extern the actual instances
extern Global g;
extern X11_wrapper x11;


// The Title Screen Function with Animated Falling Text and Additional Instructions
// Instead of calling from here ; calling from main
// Changed from one function with static variables to enum states
/*
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

// Render background (same as main screen) <--- Now edited out
// Plan: Put custom background separate from the main game


glColor3f(1.0, 1.0, 1.0);
glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
glBegin(GL_QUADS);
glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);
glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);
glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
glEnd();


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
ggprint16(&rText, 32, 0xffffff, "Janken-Game!");

Once text has finished falling, render additional instructions
if (yOffset1 <= 0 && yOffset2 <= 50 && yOffset3 <= 100) {
// Render the additional text in the center
rText.bot = (g.yres / 2) - 100;  // Position for "Press Tab..." text
ggprint16(&rText, 32, 0xffffff, "Press Tab to Begin Playing!");

rText.bot = (g.yres / 2) - 150;  // Position for "Press Backspace..." text
ggprint16(&rText, 32, 0xffffff, "Press Backspace to see our Credits!");
}

x11.swapBuffers();
}
*/

// Function to start intro screen delay
/*void showIntroScreen() 
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
*/

/*void startGame() 
  {
// Display the intro screen
showIntroScreen();
// Main program or game logic here
cout << "\nGame has started!" << endl;
}
*/

void startControlsScreen()
{
    //    Beep(1000, 200);


    static auto startTime = std::chrono::steady_clock::now();
    // Draw a transparent black overlay to dim the game background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f); // 30% transparent black
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(g.xres, 0);
    glVertex2i(g.xres, g.yres);
    glVertex2i(0, g.yres);
    glEnd();
    glDisable(GL_BLEND);

    Rect r;
    r.left = g.xres / 2;
    r.center = 1;
    r.bot = g.yres - 100;

    // Draw header
    ggprint16(&r, 32, 0xffffff, "Controls");

    // Box coordinates
    int box_x = g.xres / 2 - 150;
    int box_y = g.yres - 160;
    int box_width = 300;
    int box_height = 220;

    // Draw semi-transparent background box
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.7f); // dark translucent
    glBegin(GL_QUADS);
    glVertex2i(box_x, box_y);
    glVertex2i(box_x + box_width, box_y);
    glVertex2i(box_x + box_width, box_y - box_height);
    glVertex2i(box_x, box_y - box_height);
    glEnd();
    glDisable(GL_BLEND);

    // Drop shadow for box border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(box_x + 2, box_y - 2);
    glVertex2i(box_x + box_width + 2, box_y - 2);
    glVertex2i(box_x + box_width + 2, box_y - box_height - 2);
    glVertex2i(box_x + 2, box_y - box_height - 2);
    glEnd();

    // Draw white border box
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(box_x, box_y);
    glVertex2i(box_x + box_width, box_y);
    glVertex2i(box_x + box_width, box_y - box_height);
    glVertex2i(box_x, box_y - box_height);
    glEnd();

    // Draw controls text inside box
    r.bot = g.yres - 180;
    ggprint16(&r, 16, 0xffcc00, "M - Show Contributors");
    r.bot -= 30;
    ggprint16(&r, 16, 0x00ffcc, "Space - Start Gameplay");
    r.bot -= 30;
    ggprint16(&r, 16, 0xff6666, "Shift - Continue");
    r.bot -= 30;
    ggprint16(&r, 16, 0x66ccff, "< ( Arrow Key ) - Left");
    r.bot -= 30;
    ggprint16(&r, 16, 0xcc66ff, "> ( Arrow Key ) - Right");
    r.bot -= 30;
    ggprint16(&r, 16, 0xcccccc, "Enter - Continue");

    // Draw pulsing triangle icon for "Play"
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(now - startTime).count();
    float t = sin(elapsed * 0.002f) * 5.0f;
    glColor3f(0.3f, 1.0f, 0.3f);
    glBegin(GL_TRIANGLES);
    glVertex2i(100, 100 + t);
    glVertex2i(100, 140 + t);
    glVertex2i(140, 120 + t);
    glEnd();

    // Return to menu instruction with faded color
    r.bot -= 60;
    ggprint16(&r, 16, 0xaaaaaa, "Press ENTER to return to main menu");
}

void stevenText(Rect *rSteven)
{
    ggprint8b(rSteven, 16, 0xffffff, "Steven");
}
