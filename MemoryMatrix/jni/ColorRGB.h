#pragma once
#ifndef COLOR_RGB_H_
#define COLOR_RGB_H_

#include <stdint.h>

class ColorRGB
{
public:
  ColorRGB::ColorRGB(uint8_t _r, uint8_t _g, uint8_t _b);
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#endif