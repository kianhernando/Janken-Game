// Original author: Garrett Gregory
// CMPS 3350 - Janken Game Project
// Date: March 11, 2025

#include <cstdio>
#include "ggregory.h"
#include <fstream>
#include <string>
#include <GL/glx.h>
#include <X11/keysym.h>
#include "image.h"
#include "textures.h"

// Global class implementation
Global::Global() 
{
    xres = 576;
    yres = 324;
    isBackgroundMoving = true;
    encounterEnemy = false;
    showMembers = false;
    currentTextState = TEXT_INTRO;
    playerHealth = 100;
    enemyHealth = 100;
    settingsMenuSelection = 0;
    exitMenuSelection = 0;
    isGameOver = false;
    enemyDefeated = false;
    gameOverMenuSelection = 0;
    pauseMenuSubState = PAUSE_NONE;
}

Global g;  // Global definition

// Game Over Screen Images
Image rpsImages[3] = {
    {"./assets/player/rock_y.png"},
    {"./assets/player/paper_y.png"},
    {"./assets/player/scissors_y.png"}
};
GLuint rpsTextures[3];

// Menu Colors
const int COLOR_MENU_TITLE      = 0x00ffff; // Neon Cyan
const int COLOR_MENU_SELECTED   = 0xff00ff; // Magenta Highlight
const int COLOR_MENU_DEFAULT    = 0xffffff; // White
const int COLOR_SUBHEADER       = 0xffff00; // Yellow
const int COLOR_HINT_TEXT       = 0x666666; // Gray

void garrettText(Rect *rGarrett)
{
    ggprint8b(rGarrett, 16, 0xffffff, "Garrett");
}

void renderPauseMenu()
{
    // Draw base pause box
    float boxWidth = 280.0f;
    float boxHeight = 180.0f;
    float x = (g.xres - boxWidth) / 2;
    float y = (g.yres - boxHeight) / 2;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 1.0, 1.0, 0.15);
    for (int i = 0; i < 6; i++) {
        float offset = i * 2.0f;
        glBegin(GL_QUADS);
            glVertex2f(x - offset, y - offset);
            glVertex2f(x + boxWidth + offset, y - offset);
            glVertex2f(x + boxWidth + offset, y + boxHeight + offset);
            glVertex2f(x - offset, y + boxHeight + offset);
        glEnd();
    }

    glColor4f(0.05, 0.05, 0.1, 0.85);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + boxWidth, y);
        glVertex2f(x + boxWidth, y + boxHeight);
        glVertex2f(x, y + boxHeight);
    glEnd();
    glDisable(GL_BLEND);

    Rect r;
    r.center = 1;
    r.left = g.xres / 2;
    r.bot = y + boxHeight - 30;
    ggprint16(&r, 32, COLOR_MENU_TITLE, "-- PAUSED --");

    // Show main pause options only if no submenu is active
    if (g.pauseMenuSubState == PAUSE_NONE) {
        const char* options[] = {"Resume", "Settings", "Exit"};
        for (int i = 0; i < 3; ++i) {
            r.bot -= 1;
            int color = (i == g.pauseMenuSelection) ? COLOR_MENU_SELECTED : COLOR_MENU_DEFAULT;
            ggprint16(&r, 32, color, options[i]);
        }
    }

    // Submenu: Exit
    if (g.pauseMenuSubState == PAUSE_EXIT) {
        r.bot -= 1;
        ggprint16(&r, 32, COLOR_SUBHEADER, "Exit to:");
        const char* exitOpts[] = {"Main Menu", "Close Game"};
        for (int i = 0; i < 2; ++i) {
            r.bot -= 1;
            int color = (i == g.exitMenuSelection) ? COLOR_MENU_SELECTED : COLOR_MENU_DEFAULT;
            ggprint16(&r, 32, color, exitOpts[i]);
        }
        r.bot -= 1;
        ggprint16(&r, 32, COLOR_HINT_TEXT, "(Backspace = Back)");
    }

    // Submenu: Settings
    if (g.pauseMenuSubState == PAUSE_SETTINGS) {
        r.bot -= 1;
        ggprint16(&r, 32, COLOR_SUBHEADER, "Settings:");
        const char* setOpts[] = {"Toggle Music", "Toggle Sound"};
        for (int i = 0; i < 2; ++i) {
            r.bot -= 1;
            int color = (i == g.settingsMenuSelection) ? COLOR_MENU_SELECTED : COLOR_MENU_DEFAULT;
            ggprint16(&r, 32, color, setOpts[i]);
        }
        r.bot -= 1;
        ggprint16(&r, 32, COLOR_HINT_TEXT, "(Backspace = Back)");
    }
}

void renderGameOverScreen()
{
    static float timer = 0.0f;
    static int frame = 0;
    static float angle = 0.0f; // this is your rotating angle

    // Frame timer
    timer += 0.1f;
    if (timer > 1.0f) {
        frame = (frame + 1) % 3;
        timer = 0.0f;
    }

    angle += 1.5f; // rotate image a bit every frame
    if (angle >= 360.0f) angle -= 360.0f;

    // Dark fullscreen background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.85);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(g.xres, 0);
        glVertex2f(g.xres, g.yres);
        glVertex2f(0, g.yres);
    glEnd();
    glDisable(GL_BLEND);

    // Draw the rotating animated RPS image
    glBindTexture(GL_TEXTURE_2D, rpsTextures[frame]);
    float iw = 50.0f;
    float ih = 50.0f;
    float cx = g.xres / 2;
    float cy = g.yres / 2 + 70;

    glPushMatrix();
    glTranslatef(cx, cy, 0);         // Move pivot to center of image
    glRotatef(angle, 0, 0, 1);       // Rotate clockwise over time
    glTranslatef(-cx, -cy, 0);       // Move pivot back

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(cx - iw, cy - ih);
        glTexCoord2f(0.0, 1.0); glVertex2f(cx - iw, cy + ih);
        glTexCoord2f(1.0, 1.0); glVertex2f(cx + iw, cy + ih);
        glTexCoord2f(1.0, 0.0); glVertex2f(cx + iw, cy - ih);
    glEnd();

    glPopMatrix();

    // Game Over Title
    Rect r;
    r.center = 1;
    r.left = g.xres / 2;
    r.bot = g.yres / 2 - 20;
    ggprint16(&r, 32, COLOR_MENU_TITLE, "-- GAME OVER --");

    // Options
    const char* options[2] = {"Play Again", "Exit to Main Menu"};
    int numOptions = g.enemyDefeated ? 2 : 1;

    for (int i = 0; i < numOptions; ++i) {
        r.bot -= 15;
        int color = (i == g.gameOverMenuSelection) ? COLOR_MENU_SELECTED : COLOR_MENU_DEFAULT;
        ggprint16(&r, 28, color, options[i]);
    }

    r.bot -= 15;
    ggprint16(&r, 16, 0x666666, "(Use arrow keys + Enter)");
}

void checkDeathLogTrigger()
{
    std::ifstream log("game_log.txt");
    if (!log.is_open()) return;

    std::string line;
    std::string lastLine;

    // Grab last non-empty line
    while (std::getline(log, line)) {
        if (!line.empty())
            lastLine = line;
    }

    if (lastLine == "Player has died!") {
        g.isGameOver = true;
        g.enemyDefeated = false;
    } else if (lastLine == "Enemy has died!") {
        g.isGameOver = true;
        g.enemyDefeated = true;
    }

    if (g.isGameOver) {
        std::ofstream clear("game_log.txt", std::ofstream::out | std::ofstream::trunc);
        clear.close();
    }    
}