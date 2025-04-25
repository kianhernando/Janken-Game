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
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
    BATTLECONTROLS,
    SIMPLIFYCONTROLS,
    NONE
};

int checkPlayerState;
int healthToCompare;
Player player;
Enemy enemy;
int choice = 0;
int enChoice = 0;
static float moving = 10.0f;
static float idle = 2.0f;

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
    
    // boolean check for render screen
    bool renderStartScreen = false;

    // boolean check for screen pause
    bool isPaused = false;

    bool showExitOptions = false;

    bool isGameOver;
    bool enemyDefeated;

    // Use enum to track state
    TextState currentTextState;

    // Temporary health for testing:
    int playerHealth;
    int enemyHealth;

    int pauseMenuSelection = 0;
    int exitMenuSelection = 0;
    int settingsMenuSelection = 0;
    int gameOverMenuSelection;
    
    PauseSubState pauseMenuSubState;

    bool showPlayerWins = false;

    bool showSelection;

    Global() 
    {
        xres = 576;
        yres = 324;
        isBackgroundMoving = true;
        encounterEnemy = false;
        showMembers = false;
        currentTextState = INTRO;
        isGameOver = false;
        enemyDefeated = false;

        playerHealth = 100;
        enemyHealth = 100;

        settingsMenuSelection = 0;
        exitMenuSelection = 0;
        gameOverMenuSelection = 0;
        pauseMenuSubState = PAUSE_NONE;
        showSelection = false;
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
void renderPauseMenu(int xres, int yres, int pauseMenuSelection, 
                    int exitMenuSelection, int settingsMenuSelection,
                    PauseSubState pauseMenuSubState);
extern void showIntroScreen();
extern void startGame();
extern void renderStart();
extern void renderGameOverScreen(int xres, int yres, bool enemyDefeated, 
                                int gameOverMenuSelection);
extern void checkDeathLogTrigger(bool &isGameOver, bool &enemyDefeated);
//==========================================================================
//==========================================================================
int main()
{
    g.renderStartScreen = true;
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
        if (g.renderStartScreen) {
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
        if (key == XK_z) {
            g.showCreditsScreen = !g.showCreditsScreen;
        }
        if (key == XK_b) {
            g.showPlayerWins = !g.showPlayerWins;
        }
        if (key == XK_h) {
            g.isGameOver = true;
            g.enemyDefeated = true;
            g.gameOverMenuSelection = 0;
        }

        // Player Health Keybinds for testing purposes
        if (key == XK_2) {
            // Commented out for testing logic 
            //g.playerHealth = 20;
            g.enemyHealth = 20;
            // Commented out for testing logic 
            //player.changeHealthBar(g.playerHealth);
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
            //g.enemyHealth = 100;
            player.changeHealthBar(g.playerHealth);
            //enemy.changeHealthBar(g.enemyHealth);
        }

        // Test for random gen
        if (key == XK_t) {
            randGen();
            randItemGen();
        }
    
        if (!g.isBackgroundMoving) {
            // Keybinds for rock paper and scissors
            if (g.playerHealth != 0 && g.enemyHealth != 0) {
                if (key == XK_r) {
                    choice = ROCK;
                    enChoice = randGen();
                    //player.changeImage("assets/player/rock_x.png");
                    //enemy.changeImage("assets/enemy/rock.png");
                    checkPlayerState = logicSimon(choice, enChoice, 
                            g.playerHealth); 
                }
                
                if (key == XK_p) {
                    choice = PAPER;
                    enChoice = randGen();
                    //player.changeImage("assets/player/paper_x.png");
                    //enemy.changeImage("assets/enemy/paper.png");
                    //logicSimon(choice, enChoice, g.playerHealth);
                    checkPlayerState = logicSimon(choice, enChoice, 
                            g.playerHealth); 
                }
                
                if (key == XK_q) {
                    g.showSelection = !g.showSelection;
                }

                if (key == XK_s) {
                    choice = SCISSORS;
                    enChoice = randGen();
                    //player.changeImage("assets/player/scissors_x.png");
                    //enemy.changeImage("assets/enemy/scissors.png");
                    //logicSimon(choice, enChoice, g.playerHealth);
                    checkPlayerState = logicSimon(choice, enChoice, 
                            g.playerHealth); 
                }
                if (key == XK_n) {
                    player.changeImage("assets/player/normal_x.png");
                    enemy.changeImage("assets/enemy/boot.png");
                }
                if (checkPlayerState == 1) {
                    g.currentTextState = BATTLECONTROLS;
                    if (key == XK_a) {
                        healthToCompare = battleChoiceFunc(g.playerHealth, 
                            g.enemyHealth);
                        checkPlayerState = 0;
                    }
                }
                if (checkPlayerState == 0) {
                    g.currentTextState = SIMPLIFYCONTROLS;
                }
                int ehealth = grabEnemyHealth(g.enemyHealth);
                int phealth = grabPlayerHealth(g.playerHealth);
                compareHealth(phealth, ehealth);
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
        if (key == XK_Tab) {
            g.renderStartScreen = !g.renderStartScreen;
    
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
                             g.renderStartScreen = true;
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
                     g.renderStartScreen = true;
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

    checkDeathLogTrigger(g.isGameOver, g.enemyDefeated);
}

void render()
{
    if (g.isPaused) {
        renderPauseMenu(g.xres, g.yres, g.pauseMenuSelection, 
                       g.exitMenuSelection, g.settingsMenuSelection,
                       g.pauseMenuSubState);
        return;
    }
 
    if (g.isGameOver) {
        renderGameOverScreen(g.xres, g.yres, g.enemyDefeated, 
                            g.gameOverMenuSelection);
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

            if (g.showPlayerWins == true) {
              playerWins();
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
                case SIMPLIFYCONTROLS:
                    render_text(&rec, simplifyControls, 1);
                    break;
                case BATTLECONTROLS:
                    render_text(&rec, battleChoice, 3);
                    break;
                case NONE:
                    break;
            }

            if (g.showSelection) {
                initSelections();
            }
    }
}

