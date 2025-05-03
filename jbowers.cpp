/* ====================================================================== */
/* ================================= INFO =============================== */
/*        author:   jaden bowers 
    start   date:   2.14.2025
    last updated:   4.27.2025  

    purpose: credits screen functionality for the Janken Game.            */
/* ====================================================================== */
/* =============================== INCLUDES ============================= */
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "jbowers.h"
#include "image.h"

/* ====================================================================== */
/* =========================== GLOBAL DEFINES =========================== */
#define NUM_CREDITS 6
#define WINDOW_WIDTH 576
#define WINDOW_HEIGHT 324
#define CENTER_Y ((WINDOW_HEIGHT / 2) -20)  // Moved down by 80 pixels instead of 40

/* defines the threshold for when 
one credit pushes the next one */
#define HIT_THRESHOLD 20

/* width/height of the punching fist */
#define ROCK_W 26  // Original width
#define ROCK_H 21  // Original height

/* Animation timing controls */
#define NAME_FALL_DELAY 160    // Was 120, increased by 4/3
#define PUNCH_WAIT_TIME 120    // Was 90, increased by 4/3
#define CLEAR_WAIT_TIME 80     // Was 60, increased by 4/3

/* ====================================================================== */
/* ======================== GETTING THINGS READY ======================== */

/* initializing the credits */
const char *credits[NUM_CREDITS] = {
    "Steven", "Jaden", "Garrett", "Simon", 
    "Kian", "Janken"
};

float yPositions[NUM_CREDITS];
float velocities[NUM_CREDITS] = {0, 0, 0, 0, 0, 0};
bool landed[NUM_CREDITS] = {false};
bool done[NUM_CREDITS]   = {false};    /* mark punched-off credits */

/* used as a switch to reset the screen 
back to its defaults if the user wants to 
run the credit screen multiple times */
bool firstRun = true;

/* fist & punch variables */
GLuint rockTexture = 0;
float rockX = -ROCK_W;
float rockY = CENTER_Y - (ROCK_H / 2);
bool punching    = false;
bool punchingOff = false;
float xOffsets[NUM_CREDITS] = {0};
int punchTarget = -1;
float rockSpeed = 1.125f;    // Was 1.5f, reduced by 3/4

/* Add timing controls for animations */
int waitBetweenNames = 160;  // Was 120, increased by 4/3
int punchDelay = 80;         // Was 60, increased by 4/3

/* Add a new texture for the title image */
GLuint titleTexture = 0;

/* Animation state tracking */
int currentNameIndex = 0;
int stateTimer = 0;
enum AnimState {
    WAITING_TO_DROP,
    NAME_FALLING,
    NAME_WAITING,
    PUNCHING,
    NAME_CLEARING,
    TRANSITION_WAIT
} currentState = WAITING_TO_DROP;

/* ---------------------------------------------------------------------- 
   loadTexture: uses Image & buildAlphaData from image.h                  
   to load a PNG file into an OpenGL texture,                        
   then keys out pure-magenta (255,0,255) for transparency.             */
GLuint loadTexture(const char *filename) {
    Image *img = new Image(filename);
    int w = img->width, h = img->height;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(img));
    delete img;
    return tex;
}
/* ---------------------------------------------------------------------- */

void renderText(int x, int y, const char *text) {
    Rect r;
    r.left = x;
    r.bot  = y;
    ggprint16(&r, 32, 0x00ffffff, text);
}

/* ====================================================================== */
/* ========================== CREDIT FUNCTIONS ========================== */

/* basically the MAIN function. this is 
where everything really happens */
void creditsScreen() {
    /* clear to black */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* load textures first time */
    if (firstRun) {
        rockTexture = loadTexture("assets/player/rock_x.png");
        titleTexture = loadTexture("assets/title.png");
        firstRun = false;
        
        /* initialize positions */
        for (int i = 0; i < NUM_CREDITS; i++) {
            yPositions[i] = WINDOW_HEIGHT + (i * 40);
        }
    }

    /* State machine for animations */
    switch (currentState) {
        case WAITING_TO_DROP:
            if (stateTimer >= NAME_FALL_DELAY) {
                currentState = NAME_FALLING;
                stateTimer = 0;
            }
            break;

        case NAME_FALLING:
            if (!landed[currentNameIndex]) {
                velocities[currentNameIndex] -= 0.00375f;  // Was 0.005f, reduced by 3/4
                yPositions[currentNameIndex] += velocities[currentNameIndex];

                if (yPositions[currentNameIndex] <= CENTER_Y) {
                    yPositions[currentNameIndex] = CENTER_Y;
                    velocities[currentNameIndex] = 0;
                    landed[currentNameIndex] = true;
                    currentState = NAME_WAITING;
                    stateTimer = 0;
                }
            }
            break;

        case NAME_WAITING:
            if (stateTimer >= PUNCH_WAIT_TIME && currentNameIndex < NUM_CREDITS - 1) {
                currentState = PUNCHING;
                punching = true;
                punchTarget = currentNameIndex;
                rockX = -ROCK_W;
                stateTimer = 0;
            }
            break;

        case PUNCHING:
            if (punching) {
                rockX += rockSpeed;
                int tw = strlen(credits[punchTarget]) * 8;
                int nameX = (WINDOW_WIDTH - tw) / 2;
                if (rockX + ROCK_W >= nameX) {
                    punching = false;
                    punchingOff = true;
                    xOffsets[punchTarget] = 0;
                    currentState = NAME_CLEARING;
                }
            }
            break;

        case NAME_CLEARING:
            if (punchingOff) {
                xOffsets[punchTarget] += rockSpeed;
                if (xOffsets[punchTarget] > WINDOW_WIDTH) {
                    done[punchTarget] = true;
                    punchingOff = false;
                    currentState = TRANSITION_WAIT;
                    stateTimer = 0;
                }
            }
            break;

        case TRANSITION_WAIT:
            if (stateTimer >= CLEAR_WAIT_TIME) {
                currentNameIndex++;
                currentState = WAITING_TO_DROP;
                stateTimer = 0;
            }
            break;
    }

    /* Draw all names */
    for (int i = 0; i < NUM_CREDITS; ++i) {
        if (done[i]) continue;
        int tw = strlen(credits[i]) * 8;
        int baseX = (WINDOW_WIDTH - tw) / 2;
        int drawX = baseX;
        
        if (punchingOff && i == punchTarget) {
            drawX += (int)xOffsets[i];
        }
        
        if (drawX < WINDOW_WIDTH) {
            if (i == NUM_CREDITS - 1) {
                // Render the title image
                if (titleTexture) {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glBindTexture(GL_TEXTURE_2D, titleTexture);
                    glColor4f(1.0, 1.0, 1.0, 1.0);

                    float scale = 4.0f;
                    float titleWidth = 48.0f * scale;
                    float titleHeight = 37.0f * scale;

                    float centeredX = round((WINDOW_WIDTH - titleWidth) / 2);
                    // Use yPositions[i] for the falling animation
                    float centeredY = round(yPositions[i] - titleHeight / 3);

                    glBegin(GL_QUADS);
                        glTexCoord2f(0, 1); glVertex2f(centeredX, centeredY);
                        glTexCoord2f(1, 1); glVertex2f(centeredX + titleWidth, centeredY);
                        glTexCoord2f(1, 0); glVertex2f(centeredX + titleWidth, centeredY + titleHeight);
                        glTexCoord2f(0, 0); glVertex2f(centeredX, centeredY + titleHeight);
                    glEnd();

                    glDisable(GL_BLEND);
                }
            } else {
                renderText(drawX, (int)yPositions[i], credits[i]);
            }
        }
    }

    /* Draw the punching fist */
    if (punching && rockTexture) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glColor4f(1.0, 1.0, 1.0, 1.0);

        // Use 3x scaling for the rock sprite
        float scale = 3.0f;
        float enlargedRockW = round(ROCK_W * scale);
        float enlargedRockH = round(ROCK_H * scale);

        // Round positions to nearest pixel
        float drawX = round(rockX);
        float drawY = round(rockY);

        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(drawX, drawY);
            glTexCoord2f(1, 1); glVertex2f(drawX + enlargedRockW, drawY);
            glTexCoord2f(1, 0); glVertex2f(drawX + enlargedRockW, drawY + enlargedRockH);
            glTexCoord2f(0, 0); glVertex2f(drawX, drawY + enlargedRockH);
        glEnd();

        glDisable(GL_BLEND);
        glPopAttrib();
    }

    stateTimer++;
}

/* ensures that the credit screen gets reset
before usage if a user wants to replay the
credits screen multiple times */
void creditsScreenReset() {
    firstRun = true;
    rockX = -ROCK_W;
    punching = false;
    punchingOff = false;
    punchTarget = -1;
    currentNameIndex = 0;
    stateTimer = 0;
    currentState = WAITING_TO_DROP;
    
    for (int i = 0; i < NUM_CREDITS; i++) {
        yPositions[i] = WINDOW_HEIGHT + (i * 40);
        velocities[i] = 0;
        landed[i] = false;
        done[i] = false;
        xOffsets[i] = 0;
    }
}

void playerWins() {
    // Draw base pause box
    int ares = 576;
    int bres = 324;
    float jBoxWidth  = 281.0f;
    float jBoxHeight = 181.0f;
    float a = (ares - jBoxWidth) / 2;
    float b = (bres - jBoxHeight) / 2;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 1.0, 1.0, 0.15);
    for (int i = 0; i < 6; i++) {
        float offset = i * 2.0f;
        glBegin(GL_QUADS);
            glVertex2f(a - offset, b - offset);
            glVertex2f(a + jBoxWidth + offset, b - offset);
            glVertex2f(a + jBoxWidth + offset, b + jBoxHeight + offset);
            glVertex2f(a - offset, b + jBoxHeight + offset);
        glEnd();
    }

    glColor4f(0.05, 0.05, 0.1, 0.85);
    glBegin(GL_QUADS);
        glVertex2f(a, b);
        glVertex2f(a + jBoxWidth, b);
        glVertex2f(a + jBoxWidth, b + jBoxHeight);
        glVertex2f(a, b + jBoxHeight);
    glEnd();
    glDisable(GL_BLEND);

    Rect r;
    r.center = 1;
    r.left = ares / 2;
    r.bot = b + jBoxHeight - 30;
    ggprint16(&r, 32, 0x000000, "YOU WIN!!!");
}

/* ====================================================================== */
/* ================================= MISC =============================== */
void jadenBox(Rect *rJaden) {
    ggprint8b(rJaden, 16, 0xffffff, "Jaden");
}