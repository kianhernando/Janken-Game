#pragma once
#include <GL/glx.h>
#include "image.h"

// ORIGINALLY FROM BACKGROUND.CPP
// ORIGINAL AUTHOR: GORDON GRIESEL
class Texture 
{
public:
	Image *backImage;
	GLuint backTexture;
	float xc[2];
	float yc[2];
};