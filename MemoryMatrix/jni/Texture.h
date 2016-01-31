#ifndef TEXTURE_H_
#define TEXTURE_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif
#ifndef PRIMITIVES_H_
#include "Primitives.h"
#endif
#include <vector>

class Texture {
public:
#if !defined(__ANDROID__)
  int id; // id for DevIL data
#endif
  Vector2<int> size;
  int getWidth()const;
  int getHeight()const;
  uint format;
  /// флаг генерации GL текстуры
  bool isGenTexNameGl;
  /// "имя" текстуры GL
  uint texNameGl;
  char *pixels;
  Texture();
};

#endif