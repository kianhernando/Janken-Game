#pragma once
#include "fonts.h"
#include <GL/glx.h>
#include "textures.h"  // For Texture class

// Forward declarations
class X11_wrapper;
class Image;

// Color constants
const int COLOR_MENU_TITLE = 0xFFFFFF;
const int COLOR_MENU_SELECTED = 0x00FF00;
const int COLOR_MENU_DEFAULT = 0xAAAAAA;
const int COLOR_SUBHEADER = 0xCCCCCC;
const int COLOR_HINT_TEXT = 0x666666;

// Pause menu states
enum PauseSubState {
    PAUSE_NONE,
    PAUSE_SETTINGS,
    PAUSE_EXIT
};

// External declarations
extern class Global g;
extern X11_wrapper x11;
extern GLuint rpsTextures[3];
extern Image rpsImages[3];
extern int totalScore;

// Function declarations
void garrettText(Rect *rGarrett);
void renderPauseMenu(int xres, int yres, int pauseMenuSelection, 
                    int exitMenuSelection, int settingsMenuSelection,
                    PauseSubState pauseMenuSubState);
void renderGameOverScreen(int xres, int yres, bool enemyDefeated, 
                         int gameOverMenuSelection);
void checkDeathLogTrigger(bool &isGameOver, bool &enemyDefeated);

