#include "Texture.h"

Texture::Texture() : isGenTexNameGl(false), pixels(0) {}
int Texture::getWidth()const{ return size.x(); }
int Texture::getHeight()const{ return size.y(); }