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
#define CENTER_Y (WINDOW_HEIGHT / 2)

/* defines the threshold for when 
one credit pushes the next one */
#define HIT_THRESHOLD 20

/* width/height of the punching fist */
#define ROCK_W 64
#define ROCK_H 64

/* ====================================================================== */
/* ======================== GETTING THINGS READY ======================== */

/* initializing the credits */
const char *credits[NUM_CREDITS] = {
    "Kian", "Simon", "Jaden", "Steven", 
    "Garrett", "The Janken Game."
};

float yPositions[NUM_CREDITS];
float velocities[NUM_CREDITS] = {0, 0, 0, 0, 0, 0};
bool landed[NUM_CREDITS] = {false};
bool done[NUM_CREDITS]   = {false};    /* mark punched-off credits */

/* used as a switch to reset the screen 
back to its defaults if the user wants to 
run the credit screen multiple times */
bool firstRun = true;

/* delay for each name before falling */
int delayFrames[NUM_CREDITS] = {132, 264, 396, 528, 660, 792};

/* tracks when the name should stay in place */
int nameStayTime[NUM_CREDITS] = {0};

/* global timer for controlling when
each credit should fall */
int dropTimer = 0;

/* fist & punch variables */
GLuint rockTexture = 0;
float rockX = -ROCK_W;
float rockY = CENTER_Y - (ROCK_H / 2);
bool punching    = false;
bool punchingOff = false;
float xOffsets[NUM_CREDITS] = {0};
int punchTarget = -1;
float rockSpeed = 15.0f;

/* ---------------------------------------------------------------------- 
   loadTexture: uses Image & buildAlphaData from image.h                  
   to load a PNG file into an OpenGL RGBA texture,                        
   then keys out pure-magenta (255,0,255) for transparency.             */
GLuint loadTexture(const char *filename) {
    Image *img = new Image(filename);
    int w = img->width, h = img->height;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    /* load fist texture once */
    if (firstRun) {
        rockTexture = loadTexture("assets/player/rock_x.png");
        firstRun = false;
    }

    /* initialize yPositions */
    for (int i = 0; i < NUM_CREDITS; i++) {
        if (yPositions[i] == 0) {
            /* start ABOVE the screen (off-screen) */
            yPositions[i] = WINDOW_HEIGHT + (i * 40);
        }
    }

    /* drop & land names, skipping any already done */
    for (int i = 0; i < NUM_CREDITS; ++i) {
        if (done[i] || dropTimer < delayFrames[i]) 
            continue;

        if (!landed[i]) {
            velocities[i] -= 0.05f;
            yPositions[i] += velocities[i];

            if (yPositions[i] <= CENTER_Y) {
                yPositions[i] = CENTER_Y;
                velocities[i] = 0;
                landed[i] = true;
                nameStayTime[i] = dropTimer;
            }
        }
    }

    /* determine when to start the punch (never punch the last credit) */
    if (!punching && !punchingOff) {
        for (int i = 0; i < NUM_CREDITS - 1; ++i) {
            if (landed[i] && !done[i] &&
                dropTimer - nameStayTime[i] > 30) {
                punchTarget = i;
                punching = true;
                rockX = -ROCK_W;
                break;
            }
        }
    }

    /* animate fist flying in */
    if (punching) {
        rockX += rockSpeed;
        int tw = strlen(credits[punchTarget]) * 8;
        int nameX = (WINDOW_WIDTH - tw) / 2;
        if (rockX + ROCK_W >= nameX) {
            /* punch! start moving name off */
            punching = false;
            punchingOff = true;
            xOffsets[punchTarget] = 0;
        }
    }

    /* draw all names (skip done, apply horizontal offsets) */
    for (int i = 0; i < NUM_CREDITS; ++i) {
        if (done[i]) continue;
        int tw     = strlen(credits[i]) * 8;
        int baseX  = (WINDOW_WIDTH - tw) / 2;
        int drawX  = baseX;
        if (punchingOff && i == punchTarget) {
            xOffsets[i] += rockSpeed;
            drawX += (int)xOffsets[i];
        }
        if (drawX < WINDOW_WIDTH) {
            renderText(drawX, (int)yPositions[i], credits[i]);
        }
    }

    /* draw the fist while it's flying, restoring GL state afterward */
    if (punching && rockTexture) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex2f(rockX, rockY);
            glTexCoord2f(1,0); glVertex2f(rockX + ROCK_W, rockY);
            glTexCoord2f(1,1); glVertex2f(rockX + ROCK_W, rockY + ROCK_H);
            glTexCoord2f(0,1); glVertex2f(rockX, rockY + ROCK_H);
        glEnd();
        glPopAttrib();
    }

    /* once name is fully off-screen, mark it done */
    if (punchingOff && xOffsets[punchTarget] > WINDOW_WIDTH) {
        done[punchTarget] = true;
        punchingOff = false;
    }

    dropTimer++;
}

/* ensures that the credit screen gets reset
before usage if a user wants to replay the
credits screen multiple times */
void creditsScreenReset() {
    dropTimer = 0;
    firstRun = true;
    rockX = -ROCK_W;
    punching = false;
    punchingOff = false;
    punchTarget = -1;
    for (int i = 0; i < NUM_CREDITS; i++) {
        yPositions[i] = WINDOW_HEIGHT + (i * 40);
        velocities[i] = 0;
        landed[i] = false;
        done[i] = false;
        nameStayTime[i] = 0;
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