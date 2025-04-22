#pragma once
#include "fonts.h"
#include "textures.h"

// Menu state enum
enum PauseSubState {
    PAUSE_NONE,
    PAUSE_SETTINGS, 
    PAUSE_EXIT
};

// Text state enum - updated to match usage in background.cpp
enum TextState {
    TEXT_NONE,
    TEXT_INTRO,
    TEXT_BATTLE,
    INTRO,      // Add these to match existing usage
    CONTROLS,
    NONE
};

// Function declarations
void garrettText(Rect *rGarrett);
void renderPauseMenu();
void renderGameOverScreen(); 
void checkDeathLogTrigger();

// Menu color constants
extern const int COLOR_MENU_TITLE;
extern const int COLOR_MENU_SELECTED;
extern const int COLOR_MENU_DEFAULT;
extern const int COLOR_SUBHEADER;
extern const int COLOR_HINT_TEXT;

// Global class definition
class Global {
public:
    int xres, yres;
    Texture tex;
    bool isBackgroundMoving;
    bool encounterEnemy;
    bool showMembers;
    bool showCreditsScreen;
    TextState currentTextState;
    int playerHealth;
    int enemyHealth;
    bool isPaused;
    int pauseMenuSelection;
    bool showExitOptions;
    int exitMenuSelection;
    int settingsMenuSelection;
    bool isGameOver;
    bool enemyDefeated;
    int gameOverMenuSelection;
    PauseSubState pauseMenuSubState;

    Global();  // Declaration only, definition will be in cpp file
};

extern Global g;  // Global declaration