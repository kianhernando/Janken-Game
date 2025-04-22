#pragma once
#include "image.h"
#include <GL/gl.h>

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