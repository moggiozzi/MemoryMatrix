#ifndef GLPRIMITIVES_H_
#define GLPRIMITIVES_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif
#ifndef PRIMITIVES_H_
#include "Primitives.h"
#endif
#ifndef TEXTURE_H_
#include "Texture.h"
#endif
#ifndef COLOR_RGB_H_
#include "ColorRGB.h"
#endif

#ifdef __ANDROID__
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES/gl.h>
#elif _WIN32
#include <windows.h>
#include <gl/gl.h>
#elif __linux__
#endif

class GLHelper{
  static bool isInit;
#define MAX_POINTS_COUNT 64
  static float points_[MAX_POINTS_COUNT];

  static int width;
  static int height;
  static float ratioX;
  static float ratioY;

  static Texture fontTexture;
  
  static void setRatio();
public:
  static GLfloat xToGl(int x);
  static GLfloat yToGl(int y);
  static int glToX(GLfloat x);
  static int glToY(GLfloat y);
#ifdef __ANDROID__
  static EGLDisplay display;
  static EGLSurface surface;
  static EGLContext context;
  static bool init(ANativeWindow* window);
#elif _WIN32
  static bool init();
#elif __linux__
#endif

  static void swapBuffers();
  static void setParams();
  static void terminate();

  static int getWidth(){return width;}
  static int getHeight(){return height;}
  static void setWidth(int w);
  static void setHeight(int h);
  static float getRatioX(){return ratioX;}
  static float getRatioY(){return ratioY;}

  static void clear(GLfloat r=0, GLfloat g=0, GLfloat b=0, GLfloat a=1);
  static void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a=1);
  static void setColor(ColorRGB &c);
  static void setLineWidth(GLuint w);
  static void drawCircle2d(GLint x, GLint y, GLint r, uint points_number=16);
  static void drawLine2d(GLint x1, GLint y1, GLint x2, GLint y2);
  static void drawCircleSector2d(GLint x, GLint y, GLint r, GLfloat a1, GLfloat a2, uint points_number=16);
  static void drawTriangle2d(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3);
  static void drawRect2d(GLint x1, GLint y1, GLint w, GLint h);
  
  static void drawTexture(Texture* texture, int dx, int dy, int dw=-1, int dh=-1,
    int tx=0, int ty=0, int tw=-1, int th=-1);
  static void drawTexture(Texture* texture, Rect<int> &dr);
  static void drawTexture(Texture* texture, Vector2<int> &pos, float angle=0);

  static void drawText(const Vector2<int>& pos, const char* text, uint size);
  static void drawText(int x, int y, const char* text, uint size=16);
};

#endif // GLPRIMITIVES_H_
