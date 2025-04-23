/* ====================================================================== */
/* ================================= INFO =============================== */
/*        author:   jaden bowers 
    start   date:   2.14.2025
    last updated:   3.29.2025  

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

/* ====================================================================== */
/* =========================== GLOBAL DEFINES =========================== */
#define NUM_CREDITS 6
#define WINDOW_WIDTH 576
#define WINDOW_HEIGHT 324
#define CENTER_Y (WINDOW_HEIGHT / 2)

/* defines the threshold for when 
one credit pushes the next one */
#define HIT_THRESHOLD 20

/* ====================================================================== */
/* ======================== GETTING THINGS READY ======================== */

/* initializing the credits */
const char *credits[NUM_CREDITS] = {
    "Kian", "Simon", "Jaden", "Steven", 
    "Garrett", "The Jake Game."
};

float yPositions[NUM_CREDITS];
float velocities[NUM_CREDITS] = {0, 0, 0, 0, 0, 0};

bool landed[NUM_CREDITS] = {false};

/* used as a switch to reset the screen 
back to it's defaults if the user wants to 
run the credit screen multiple times */
bool firstRun = true;

/* delay for each name before falling */
int delayFrames[NUM_CREDITS] = {132, 264, 396, 528, 660, 792};

/* tracks when the name should stay in place */
int nameStayTime[NUM_CREDITS] = {0};

/* global timer for controlling when
each credit should fall */
int dropTimer = 0;

void renderText(int x, int y, const char *text) {
    Rect r;
    r.left = x;
    r.bot = y;

    ggprint8b(&r, 16, 0x00ffffff, text);
}


/* ====================================================================== */
/* ========================== CREDIT FUNCTIONS ========================== */

/* basically the MAIN function. this is 
where  everything really happens */
void creditsScreen() {
    
    /* sets the screen to black and clears it */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* initialize the yPositions for each name */
    for (int i = 0; i < NUM_CREDITS; i++) {
        if (yPositions[i] == 0) {

            /* start ABOVE the screen (off-screen) */
            yPositions[i] = WINDOW_HEIGHT + (i * 40);
        }
    }

    /* iterates through the names array and drops
    the names at their specified intervals */
    for (int i = 0; i < NUM_CREDITS; ++i) {
        if (dropTimer < delayFrames[i])
            continue;

        /* if a name has NOT landed yet, the gravity 
        will slowly keep increasing, falling faster 
        and faster until it gets there */
        if (!landed[i]) {
            velocities[i] -= 0.05f;
            yPositions[i] += velocities[i];

            /* if it's less than the center of the
            screen, it will set it to the center of
            the screen. it will stop it's falling right
            at the center of the screen, and turn it's
            "landed" value to true instead of false */
            if (yPositions[i] <= CENTER_Y) {
                yPositions[i] = CENTER_Y;
                velocities[i] = 0;
                landed[i] = true;

                /* starts the timer once the name lands */
                nameStayTime[i] = dropTimer;
            }
        }

        /* calculate the width of the text */
        int textWidth = strlen(credits[i]) * 8;

        /* "where's the center of the screen?" */
        int xPosition = (WINDOW_WIDTH - textWidth) / 2;

        /* calling the function made earlier in 
        the program, to generate the names */
        renderText(xPosition, (int)yPositions[i], credits[i]);
    }

    /* pushing effect: */
    for (int i = 0; i < NUM_CREDITS; ++i) {
        if (landed[i] && dropTimer - nameStayTime[i] > 30) {

            /* if the currently falling name is close
            to landing, push the name CURRENTLY in the
            middle of the screen downwards */
            if (i + 1 < NUM_CREDITS && yPositions[i + 1]
                <= CENTER_Y + HIT_THRESHOLD) {
                
                /* push the current name downwards */
                velocities[i] = -10.0f;
            }
            yPositions[i] += velocities[i];
        }
    }

    dropTimer++;
}

/* ensures that the credit screen gets reset
before usage if a user wants to replay the
credits screen multiple times */
void creditsScreenReset() {
    dropTimer = 0;
    firstRun = true;

    for (int i = 0; i < NUM_CREDITS; i++) {
        yPositions[i] = WINDOW_HEIGHT + (i * 40);
        velocities[i] = 0;
        landed[i] = false;
        nameStayTime[i] = 0;
    }
}

void playerWins()
{
    // Draw base pause box
    int ares = 576;
    int bres = 324;
    float jBoxWidth = 281.0f;
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
void jadenBox(Rect *rJaden)
{
    ggprint8b(rJaden, 16, 0xffffff, "Jaden");
}