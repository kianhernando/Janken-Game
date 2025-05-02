#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include "fonts.h"
#include "textures.h"

// Forward declarations
class Global;
class X11_wrapper;

// External declarations
extern Global g;
extern X11_wrapper x11;

void stevenText(Rect *rSteven);
void showIntroScreen();
void startGame();
void renderStart();
void startControlsScreen();
