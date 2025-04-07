//
// modified by: Kian Hernando, Simon Santos, 
//              Steven Tran, Jaden Bowers,
//              and Garrett Gregory
// start date: February 2025
// purpose: Framework for building Video Game
//
// program: background.cpp
// original author:  Gordon Griesel
// date:    2017 - 2018
//
// The position of the background QUAD does not change.
// Just the texture coordinates change.
// In this example, only the x coordinates change.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "khernando.h"
#include "ssantos.h"
#include "stran.h"
#include "jbowers.h"
#include "ggregory.h"

// MOVED IMAGE CLASS TO IMAGE.H
// class Image {
// public:
//     int width, height;
//     unsigned char *data;
//     ~Image() { delete [] data; }
//     Image(const char *fname) {
//         if (fname[0] == '\0')
//             return;
//         char name[40];
//         strcpy(name, fname);
//         int slen = strlen(name);
//         name[slen-4] = '\0';
//         char ppmname[80];
//         sprintf(ppmname,"%s.ppm", name);
//         char ts[100];
//         sprintf(ts, "convert %s %s", fname, ppmname);
//         system(ts);
//         FILE *fpi = fopen(ppmname, "r");
//         if (fpi) {
//             char line[200];
//             fgets(line, 200, fpi);
//             fgets(line, 200, fpi);
//             //skip comments and blank lines
//             while (line[0] == '#' || strlen(line) < 2)
//                 fgets(line, 200, fpi);
//             sscanf(line, "%i %i", &width, &height);
//             fgets(line, 200, fpi);
//             //get pixel data
//             int n = width * height * 3;            
//             data = new unsigned char[n];            
//             for (int i=0; i<n; i++)
//                 data[i] = fgetc(fpi);
//             fclose(fpi);
//         } else {
//             printf("ERROR opening image: %s\n", ppmname);
//             exit(0);
//         }
//         unlink(ppmname);
//     }
// };

// Temporary image for game
// URL: 
// https://opengameart.org/content/simple-natural-landscape-pixel-art-background
Image img[1] = {"./assets/landscape.jpg"};

// Game Over Screen Images
Image rpsImages[3] = {
    {"./assets/player/rock_y.png"},
    {"./assets/player/paper_y.png"},
    {"./assets/player/scissors_y.png"}
};
GLuint rpsTextures[3];

// MOVED TEXTURE CLASS TO TEXTURE.H
// class Texture {
// public:
//     Image *backImage;
//     GLuint backTexture;
//     float xc[2];
//     float yc[2];
// };

enum TextState {
    INTRO,
    CONTROLS,
    NONE
};

enum PauseSubState {
    PAUSE_NONE,
    PAUSE_SETTINGS,
    PAUSE_EXIT
};

Player player;
Enemy enemy;
int choice = 0;
int enChoice = 0;
static float moving = 10.0f;
static float idle = 2.0f;

// Pause Menu - Colors
const int COLOR_MENU_TITLE      = 0x00ffff; 
const int COLOR_MENU_SELECTED   = 0xff00ff; 
const int COLOR_MENU_DEFAULT    = 0xffffff; 
const int COLOR_SUBHEADER       = 0xffff00; 
const int COLOR_HINT_TEXT       = 0x666666; 

class Global 
{
public:
    int xres, yres;
    Texture tex;
    // Added boolean to detect background movement
    bool isBackgroundMoving;

    // Boolean check for enemy encounter
    bool encounterEnemy;

    // Boolean check for name appearance
    bool showMembers;

    // boolean check for credits screen
    bool showCreditsScreen = false;

    // Use enum to track state
    TextState currentTextState;

    // Temporary health for testing:
    int playerHealth;
    int enemyHealth;

    // Pause Menu - variables
    bool isPaused = false;
    int pauseMenuSelection = 0;
    bool showExitOptions = false;
    int exitMenuSelection = 0;
    int settingsMenuSelection = 0;
    PauseSubState pauseMenuSubState;

    // Game Over Screen - variables
    bool isGameOver;
    bool enemyDefeated;
    int gameOverMenuSelection;

    Global() 
    {
        xres = 576;
        yres = 324;
        isBackgroundMoving = true;
        encounterEnemy = false;
        showMembers = false;
        currentTextState = INTRO;
        playerHealth = 100;
        enemyHealth = 100;
        pauseMenuSubState = PAUSE_NONE;
        settingsMenuSelection = 0;
        exitMenuSelection = 0;
        isGameOver = false;
        enemyDefeated = false;
        gameOverMenuSelection = 0;
    }
} g;

class X11_wrapper 
{
private:
    Display *dpy;
    Window win;
    GLXContext glc;
public:
    X11_wrapper() 
    {
        GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, 
            None };
        setup_screen_res(576, 324);
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
            printf("\n\tcannot connect to X server\n\n");
            exit(EXIT_FAILURE);
        }
        Window root = DefaultRootWindow(dpy);
        XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL) {
            printf("\n\tno appropriate visual found\n\n");
            exit(EXIT_FAILURE);
        } 
        Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.event_mask =
            ExposureMask | KeyPressMask | KeyReleaseMask | 
            PointerMotionMask |
            ButtonPressMask | ButtonReleaseMask |
            StructureNotifyMask | SubstructureNotifyMask;
        win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
                          vi->depth, InputOutput, vi->visual,
                          CWColormap | CWEventMask, &swa);
        set_title();
        
        // Added to X11_wrapper to prevent window resizing (breaks assets)
        XSizeHints hints;
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = hints.max_width = g.xres;
        hints.min_height = hints.max_height = g.yres;
        XSetWMNormalHints(dpy, win, &hints);
        
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);
    }

    void cleanupXWindows() 
    {
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
    }

    void setup_screen_res(const int w, const int h) 
    {
        g.xres = w;
        g.yres = h;
    }

    void reshape_window(int width, int height) 
    {
        //window has been resized.
        setup_screen_res(width, height);
        glViewport(0, 0, (GLint)width, (GLint)height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, g.xres, 0, g.yres, -1, 1);
        set_title();
    }

    void set_title() 
    {
        //Set the window title bar.
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "JANKEN - The One-Handed Journey");
    }

    bool getXPending() 
    {
        return XPending(dpy);
    }

    XEvent getXNextEvent() 
    {
        XEvent e;
        XNextEvent(dpy, &e);
        return e;
    }

    void swapBuffers() 
    {
        glXSwapBuffers(dpy, win);
    }

    void check_resize(XEvent *e) 
    {
        //The ConfigureNotify is sent by 
        //the server if the window is resized.
        if (e->type != ConfigureNotify)
            return;
        XConfigureEvent xce = e->xconfigure;
        if (xce.width != g.xres || xce.height != g.yres) {
            //Window size did change.
            reshape_window(xce.width, xce.height);
        }
    }
} x11;

void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics(void);
void render(void);
void renderPauseMenu();
extern void showIntroScreen();
extern void startGame();
bool renderStartScreen = false;
extern void renderStart();
void renderGameOverScreen();
void checkDeathLogTrigger();
//==========================================================================
//==========================================================================
int main()
{
    renderStartScreen = true;
    startGame();
    
    init_opengl();
    int done = 0;
    while (!done) {
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            check_mouse(&e);
            done = check_keys(&e);
        }
        physics();
        if (renderStartScreen) {
            renderStart();
        } else {
        render();
        }
        x11.swapBuffers();
    }
    cleanup_fonts();
    return 0;
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, g.xres, g.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //Do this to allow texture maps
    glEnable(GL_TEXTURE_2D);glClearColor(0.0, 0.0, 0.0, 0.0);
    //
    //load the images file into a ppm structure.
    //
    g.tex.backImage = &img[0];
    //create opengl texture elements
    glGenTextures(1, &g.tex.backTexture);
    int w = g.tex.backImage->width;
    int h = g.tex.backImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
    
    // Initialize fonts for text rendering
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    
    // Edited to use full texture width/height
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 1.0;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;

    for (int i = 0; i < 3; ++i) {
        glGenTextures(1, &rpsTextures[i]);
        int w = rpsImages[i].width;
        int h = rpsImages[i].height;
        glBindTexture(GL_TEXTURE_2D, rpsTextures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, rpsImages[i].data);
    }    
}

void check_mouse(XEvent *e)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button is down
        }
        if (e->xbutton.button==3) {
            //Right button is down
        }
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        //Mouse moved
        savex = e->xbutton.x;
        savey = e->xbutton.y;
    }
}

int check_keys(XEvent *e)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_Escape) {
            return 1;
        }
        
        // Text Keybinds
        if (key == XK_a) {
            g.currentTextState = INTRO;
        }
        if (key == XK_m) {
            g.showMembers = !g.showMembers;
        }
        if (key == XK_c) {
            g.currentTextState = CONTROLS;
        }
        if (key == XK_BackSpace) {
            g.showCreditsScreen = !g.showCreditsScreen;
        }
        if (key == XK_h) {
            g.isGameOver = true;
            g.enemyDefeated = true; 
            g.gameOverMenuSelection = 0; 
        }

        // Player Health Keybinds for testing purposes
        if (key == XK_2) {
            g.playerHealth = 20;
            g.enemyHealth = 20;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }
        if (key == XK_4) {
            g.playerHealth = 40;
            g.enemyHealth = 40;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }
        if (key == XK_6) {
            g.playerHealth = 60;
            g.enemyHealth = 60;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }
        if (key == XK_8) {
            g.playerHealth = 80;
            g.enemyHealth = 80;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }
        if (key == XK_0) {
            g.playerHealth = 0;
            g.enemyHealth = 0;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }
        if (key == XK_f) {
            g.playerHealth = 100;
            g.enemyHealth = 100;
            player.changeHealthBar(g.playerHealth);
            enemy.changeHealthBar(g.enemyHealth);
        }

        // Test for random gen
        if (key == XK_t) {
            randGen();
        }
    
        if (!g.isBackgroundMoving) {
            // Keybinds for rock paper and scissors
            if (g.playerHealth != 0 && g.enemyHealth != 0) {
                if (key == XK_r) {
                    choice = ROCK;
                    enChoice = randGen();
                    //player.changeImage("assets/player/rock_x.png");
                    //enemy.changeImage("assets/enemy/rock.png");                    
                    logicSimon(choice, enChoice, g.playerHealth, g.enemyHealth);                
                }
                
                if (key == XK_p) {
                    choice = PAPER;
                    enChoice = randGen();
                    //player.changeImage("assets/player/paper_x.png");
                    //enemy.changeImage("assets/enemy/paper.png");
                    logicSimon(choice, enChoice, g.playerHealth, g.enemyHealth);
                }
                if (key == XK_s) {
                    choice = SCISSORS;
                    enChoice = randGen();
                    //player.changeImage("assets/player/scissors_x.png");
                    //enemy.changeImage("assets/enemy/scissors.png");
                    logicSimon(choice, enChoice, g.playerHealth, g.enemyHealth);
                }
                if (key == XK_n) {
                    player.changeImage("assets/player/normal_x.png");
                    enemy.changeImage("assets/enemy/boot.png");
                }
            }
        }

        // Enemy Keybinds
        if (key == XK_space) {
            g.encounterEnemy = !g.encounterEnemy;
            
            if (!g.encounterEnemy) {
                enemy.hitBarrier = false;
                enemy.pos_x = enemy.base_x;
                enemy.pos_y = enemy.base_y;
                g.isBackgroundMoving = true;
                // Reset sprites to normal when movement resumes
                player.changeImage("assets/player/normal_x.png");
                enemy.changeImage("assets/enemy/boot.png");
            } else {
                g.isBackgroundMoving = false;
            }
        }
          //if tab is pressed ; switches screens
        if(key == XK_Tab) {
            renderStartScreen = !renderStartScreen;
        }

        if (e->type == KeyPress) {
            int key = XLookupKeysym(&e->xkey, 0);
        
            // Toggle pause with 'G'
            if (key == XK_g) {
                g.isPaused = !g.isPaused;
                g.pauseMenuSelection = 0;
                g.exitMenuSelection = 0;
                g.settingsMenuSelection = 0;
                g.pauseMenuSubState = PAUSE_NONE;
                return 0;
            }
        
            // If pause menu is active
            if (g.isPaused) {
                // Exit submenu
                if (g.pauseMenuSubState == PAUSE_EXIT) {
                    if (key == XK_Up || key == XK_Down) {
                        g.exitMenuSelection = 1 - g.exitMenuSelection;
                    } else if (key == XK_Return) {
                        if (g.exitMenuSelection == 0) {
                            renderStartScreen = true;
                            g.isPaused = false;
                        } else {
                            x11.cleanupXWindows();
                            exit(0); 
                        }
                    } else if (key == XK_BackSpace) {
                        g.pauseMenuSubState = PAUSE_NONE;
                    }
                    return 0;
                }
        
                // Settings submenu
                if (g.pauseMenuSubState == PAUSE_SETTINGS) {
                    if (key == XK_Up || key == XK_Down) {
                        g.settingsMenuSelection = 1 - g.settingsMenuSelection;
                    } else if (key == XK_Return) {
                        // Add toggle logic here later
                    } else if (key == XK_BackSpace) {
                        g.pauseMenuSubState = PAUSE_NONE;
                    }
                    return 0;
                }
        
                // Main pause menu
                if (g.pauseMenuSubState == PAUSE_NONE) {
                    if (key == XK_Up) {
                        g.pauseMenuSelection = (g.pauseMenuSelection + 2) % 3;
                    } else if (key == XK_Down) {
                        g.pauseMenuSelection = (g.pauseMenuSelection + 1) % 3;
                    } else if (key == XK_Return) {
                        if (g.pauseMenuSelection == 0) {
                            g.isPaused = false;
                        } else if (g.pauseMenuSelection == 1) {
                            g.pauseMenuSubState = PAUSE_SETTINGS;
                        } else if (g.pauseMenuSelection == 2) {
                            g.pauseMenuSubState = PAUSE_EXIT;
                        }
                    }
                    return 0;
                }
            }
        }
        // Game Over Screen
        if (g.isGameOver) {
            if (key == XK_Up || key == XK_Down) {
                g.gameOverMenuSelection ^= 1;
                if (!g.enemyDefeated) g.gameOverMenuSelection = 1; // lock to option 1
            }
            if (key == XK_Return) {
                if (g.gameOverMenuSelection == 0 && g.enemyDefeated) {
                    // Start next level
                    printf("Loading next level...\n");
                    g.isGameOver = false;
                    g.playerHealth = 100;
                    g.enemyHealth = 100;
                    player.changeHealthBar(100);
                    enemy.changeHealthBar(100);
                    // any other reset logic here
                } else {
                    // Return to main menu
                    renderStartScreen = true;
                    g.isGameOver = false;
                }
            }
            return 0;
        }
        
    }
    return 0;
}

void physics()
{
    if (g.isBackgroundMoving && !g.encounterEnemy) {
        g.tex.xc[0] += 0.001;
        g.tex.xc[1] += 0.001;
    }

    checkDeathLogTrigger();
}

void render()
{
    if (g.isPaused) {
        renderPauseMenu();
        return;
    }

    if (g.isGameOver) {
        renderGameOverScreen();
        return;
    }    

    if (g.showCreditsScreen == true) {
        creditsScreen();
    } else {
            creditsScreenReset();
            glClearColor(1.0, 1.0, 1.0, 1.0); /* added so it sets the
                                              background color back 
                                              to white */
        
            static bool isPlayerInit = false;
            if (!isPlayerInit) {
                player.init("assets/player/normal_x.png");
                player.init_hp(g.playerHealth);
                isPlayerInit = true;
            }
        
            static bool isEnemyInit = false;
            if (!isEnemyInit) {
                enemy.init("assets/enemy/boot.png");
                enemy.init_hp();
                isEnemyInit = true;
            }
            
            glClear(GL_COLOR_BUFFER_BIT);
            
            // Render background first
            glColor3f(1.0, 1.0, 1.0);
            glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
            glBegin(GL_QUADS);
                glTexCoord2f(g.tex.xc[0], g.tex.yc[1]);
                glVertex2i(0, 0);
                glTexCoord2f(g.tex.xc[0], g.tex.yc[0]);
                glVertex2i(0, g.yres);
                glTexCoord2f(g.tex.xc[1], g.tex.yc[0]);
                glVertex2i(g.xres, g.yres);
                glTexCoord2f(g.tex.xc[1], g.tex.yc[1]);
                glVertex2i(g.xres, 0);
            glEnd();
        
            render_box();
        
            if (g.isBackgroundMoving && !g.encounterEnemy) {
                player.update(moving);
            } else {
                player.pos_y = player.base_y;
                player.update(idle);
            }
        
            player.render_player();
        
            if (g.encounterEnemy) {
                render_top();
                enemy.update(moving);
                if (!enemy.hitBarrier) {
                    enemy.update(moving);
                } else {
                    enemy.pos_y = enemy.base_y;
                    enemy.update(idle);
                }
                enemy.render_enemy();
                enemy.render_hp();
                player.render_hp();
            } else {
                g.playerHealth = 100;
                player.changeHealthBar(g.playerHealth);
                enemy.hitBarrier = false;
                enemy.pos_x = enemy.base_x;
                enemy.pos_y = enemy.base_y;
            }
        
            Rect rKian;
            rKian.bot = g.yres - 20;
            rKian.left = 520;
            rKian.center = 0;
            if (g.showMembers == true) {
                kianText(&rKian);
            }
        
            Rect rSimon;
            rSimon.bot = g.yres - 35;
            rSimon.left = 520;
            rSimon.center = 0;
            if (g.showMembers == true) {
                simonText(&rSimon);
            }
            
            Rect rSteven;
            rSteven.bot = g.yres - 50;
            rSteven.left = 520;
            rSteven.center = 0;
            if (g.showMembers == true) {
                stevenText(&rSteven);
            }
        
            Rect rJaden;
            rJaden.bot = g.yres - 65;
            rJaden.left = 520;
            rJaden.center = 0;
            if (g.showMembers == true) {
                jadenBox(&rJaden);
            }
        
            Rect rGarrett;
            rGarrett.bot = g.yres - 80;
            rGarrett.left = 520;
            rGarrett.center = 0;
            if (g.showMembers == true) {
                garrettText(&rGarrett);
            }
        
            Rect rec;
        
            Rect rControl;
            rControl.left = g.xres/2;
            rControl.bot = g.yres - 25;
            rControl.center = 1;
            controlText(&rControl);
        
            switch (g.currentTextState) {
                case INTRO:
                    render_text(&rec, intro, 3);
                    break;
                case CONTROLS:
                    render_text(&rec, controls, 3);
                    break;
                case NONE:
                    break;
            }

    }
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

    // Render background (same as main screen)
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
    ggprint16(&rText, 32, 0xffffff, "Janken - Game!");

    // "To!" - Falls next, below "Janken - Game!"
    rText.bot = (g.yres / 2) + yOffset2;
    ggprint16(&rText, 32, 0xffffff, "To!");

    // "Welcome!" - Falls first, above "To!"
    rText.bot = (g.yres / 2) + yOffset1;
    ggprint16(&rText, 32, 0xffffff, "Welcome!");

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

// Pause menu - rendering
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