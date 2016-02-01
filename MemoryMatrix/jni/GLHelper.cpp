//#include "Global.h"
#include "GLHelper.h"
#include "ResourceManager.h"
#define _USE_MATH_DEFINES // for math constants such as M_PI
#include <math.h>

#define gradToRad(x) (x * 0.0174532925199)
#define radToGrad(x) (x * 57.295779513082)
#define isglerr(str) {int err; if((err=glGetError())!=GL_NO_ERROR) LOGI("%s error 0x%X", str, err);}

bool GLHelper::isInit=false;
float GLHelper::points_[MAX_POINTS_COUNT];
Texture GLHelper::fontTexture;
float GLHelper::ratioX=1;
float GLHelper::ratioY=1;
GLfloat GLHelper::xToGl(int x) { return ( (float)x/width-0.5f)*2.0f*ratioX; }
GLfloat GLHelper::yToGl(int y) { return ( ((float)height-y)/height-0.5f)*2.0f*ratioY; }
int GLHelper::glToX(GLfloat x) { return (x/ratioX + 1.0) * width / 2.0; }
int GLHelper::glToY(GLfloat y) { return (1.0 - y/ratioY) * height / 2.0; }

void GLHelper::setWidth(int w){width=w;setRatio();}
void GLHelper::setHeight(int h){height=h;setRatio();}
#ifdef __ANDROID__

#include <GLES/glext.h>
#include "android/native_window.h" // ANativeWindow_setBuffersGeometry(
EGLDisplay GLHelper::display;
EGLSurface GLHelper::surface;
EGLContext GLHelper::context;
#ifndef GL_CLAMP
#define GL_CLAMP GL_CLAMP_TO_EDGE
#endif

int GLHelper::width;
int GLHelper::height;

bool GLHelper::init(ANativeWindow* window){
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig config;

  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(display, 0, 0);
  eglChooseConfig(display, attribs, &config, 1, &numConfigs);
  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
  ANativeWindow_setBuffersGeometry(window, 0, 0, format);
  surface = eglCreateWindowSurface(display, config, window, NULL);
  context = eglCreateContext(display, config, NULL, NULL);
  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    LOGW("Unable to eglMakeCurrent");
    return false;
  }
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);
  width  = w;
  height = h;
  ResourceManager::loadImage("res/font.png",&fontTexture);
  setParams();
  isInit = true;
  return true;
}

void GLHelper::terminate(){
  if (display != EGL_NO_DISPLAY) {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) {
      eglDestroyContext(display, context);
    }
    if (surface != EGL_NO_SURFACE) {
      eglDestroySurface(display, surface);
    }
    eglTerminate(display);
  }
  display = EGL_NO_DISPLAY;
  context = EGL_NO_CONTEXT;
  surface = EGL_NO_SURFACE;
}

#elif _WIN32

#include <GL/freeglut.h>

int GLHelper::width=640;
int GLHelper::height=480;

bool GLHelper::init() {
  setParams();
  ResourceManager::loadImage("res/font.png",&fontTexture);
  return true;
}

#elif __linux__
#endif
void GLHelper::swapBuffers(){
#ifdef __ANDROID__
  eglSwapBuffers(GLHelper::display, GLHelper::surface);
#elif _WIN32
  glutSwapBuffers();
  glutPostRedisplay(); // ???
#elif __linux__
#endif
}

void GLHelper::setRatio(){
  if ( width < height ) {
    ratioX = (float)width/height;
    ratioY = 1.0;
  }else{
    ratioX = 1.0;
    ratioY = (float)height/width;
  }
}
void GLHelper::setParams(){
  glLoadIdentity();
  setRatio();
  glScalef( 1.0f/ratioX, 1.0f/ratioY, 1.0f ); // чтоб не возникали деформации при повороте текстур
  glEnable(GL_BLEND);
  //glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//смешение
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glDisable(GL_DEPTH_TEST);
  isglerr("Error glParams");
}

void GLHelper::clear(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
  glClearColor(r,g,b,a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void GLHelper::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
  glColor4f(r,g,b,a);
}
void GLHelper::setColor(ColorRGB &c){
  glColor4f( c.r, c.g, c.b, 1.0 );
}

void GLHelper::setLineWidth(GLuint w){
  GLfloat fw = xToGl(w) > yToGl(w) ? xToGl(w) : yToGl(w);
  glLineWidth(fw);
}

void GLHelper::drawLine2d(GLint x1, GLint y1, GLint x2, GLint y2){
  glEnableClientState(GL_VERTEX_ARRAY);
  points_[0]=xToGl(x1);
  points_[1]=yToGl(y1);
  points_[2]=xToGl(x2);
  points_[3]=yToGl(y2);
  glVertexPointer( 2, GL_FLOAT, 0, points_ ); //2 координаты на точку
  glDrawArrays( GL_LINE_STRIP, 0, 2 );
  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLHelper::drawCircle2d(GLint x, GLint y, GLint r, uint points_number){
  glEnableClientState(GL_VERTEX_ARRAY);
  if((points_number+2)*2>MAX_POINTS_COUNT) points_number=(MAX_POINTS_COUNT-2)/2;
  points_[0]=xToGl(x); //центр круга
  points_[1]=yToGl(y);
  float angle;
  for(unsigned int i=0; i<points_number+1; i++){	//+1 замыкающая точка
    angle = static_cast<GLfloat>(2*M_PI*i/points_number);
    points_[i*2+2] = xToGl(x + r*cos(angle));
    points_[i*2+3] = yToGl(y + r*sin(angle));
  }
  glVertexPointer( 2, GL_FLOAT, 0, points_ ); //2 координаты на точку
  glDrawArrays( GL_TRIANGLE_FAN, 0, points_number+2 );
  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLHelper::drawCircleSector2d(GLint x, GLint y, GLint r, GLfloat a1, GLfloat a2, uint points_number){
  glEnableClientState(GL_VERTEX_ARRAY);
  if((points_number+2)*2>MAX_POINTS_COUNT) points_number=(MAX_POINTS_COUNT-2)/2;
  points_[0]=xToGl(x); //центр круга
  points_[1]=yToGl(y);
  a1 = static_cast<GLfloat>(gradToRad(a1));
  a2 = static_cast<GLfloat>(gradToRad(a2));
  float angle = a1;
  float angleStep;
  if( a2 > a1 )
    angleStep = (a2-a1)/points_number;
  else
    angleStep = static_cast<GLfloat>((2*M_PI+a2-a1)/points_number);
  for(uint i=0; i<points_number+1; i++){
    points_[i*2+2] = xToGl(x + r*cos(angle));
    points_[i*2+3] = yToGl(y + r*sin(angle));
    angle += angleStep;
  }
  glVertexPointer( 2, GL_FLOAT, 0, points_ ); //2 координаты на точку
  glDrawArrays( GL_TRIANGLE_FAN, 0, points_number+2 );
  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLHelper::drawTriangle2d(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3){
  glEnableClientState(GL_VERTEX_ARRAY);
  points_[0]=xToGl(x1);
  points_[1]=yToGl(y1);
  points_[2]=xToGl(x2);
  points_[3]=yToGl(y2);
  points_[4]=xToGl(x3);
  points_[5]=yToGl(y3);
  glVertexPointer( 2, GL_FLOAT, 0, points_ ); //2 координаты на точку
  glDrawArrays( GL_TRIANGLE_FAN, 0, 3 );
  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLHelper::drawRect2d(GLint x1, GLint y1, GLint x2, GLint y2){
  glEnableClientState(GL_VERTEX_ARRAY);
  points_[0]=xToGl(x1);
  points_[1]=yToGl(y1);
  points_[2]=xToGl(x2);
  points_[3]=yToGl(y1);
  points_[4]=xToGl(x2);
  points_[5]=yToGl(y2);
  points_[6]=xToGl(x1);
  points_[7]=yToGl(y2);
  glVertexPointer( 2, GL_FLOAT, 0, points_ ); //2 координаты на точку
  glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
  glDisableClientState(GL_VERTEX_ARRAY);
}

void GLHelper::drawTexture(Texture* texture, Rect<int> &dr){
  GLHelper::drawTexture(texture, dr.x(), dr.y(), dr.getWidth(), dr.getHeight());
}

void GLHelper::drawTexture(Texture* texture, Vector2<int>& pos, float angle){
  if ( angle == 0.0f )
  {
    GLHelper::drawTexture(texture, pos.x(), pos.y());
    return;
  }
  glPushMatrix();
  int dx = pos.x(), dy = pos.y(), dw = texture->getWidth(), dh = texture->getHeight();
  glTranslatef(xToGl(dx+dw/2),yToGl(dy+dh/2),0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glTranslatef(-xToGl(dx+dw/2),-yToGl(dy+dh/2),0);
  GLHelper::drawTexture(texture, dx, dy);
  glPopMatrix();
}

void GLHelper::drawTexture(Texture* texture, int dx, int dy, int dw, int dh,
                           int tx, int ty, int tw, int th){
  if(tw==-1){
    tw = texture->getWidth();
    th = texture->getHeight();
  }
  if(dw==-1){
    dw=texture->getWidth();
    dh=texture->getHeight();
  }
  glEnable (GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture->texNameGl);
  isglerr("Error glBindTexture");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//GL_CLAMP_TO_EDGE);
  isglerr("Error glTexParameteri");
  glEnableClientState(GL_VERTEX_ARRAY);
  points_[0] = xToGl(dx); 			points_[1] = yToGl(dy+dh);
  points_[2] = xToGl(dx);				points_[3] = yToGl(dy);
  points_[4] = xToGl(dx+dw);    points_[5] = yToGl(dy);
  points_[6] = xToGl(dx+dw); 	  points_[7] = yToGl(dy+dh);
  glVertexPointer(2, GL_FLOAT, 0, points_);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  float pp[] = {
    (float)tx/texture->getWidth(),     (float)(ty+th)/texture->getHeight(), //0,1,
    (float)tx/texture->getWidth(),     (float)ty/texture->getHeight(),      //0,0
    (float)(tx+tw)/texture->getWidth(),(float)ty/texture->getHeight(),      //1,0
    (float)(tx+tw)/texture->getWidth(),(float)(ty+th)/texture->getHeight(), //1,1,
  };
  glTexCoordPointer(2, GL_FLOAT, 0, pp);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable (GL_TEXTURE_2D);
  isglerr("err");
}

void GLHelper::drawText(const Vector2<int>& pos, const char* text, uint size){
  drawText(pos.x(), pos.y(), text, size);
}
void GLHelper::drawText(int x, int y, const char* text, uint size){
  uint len = strnlen(text,255);
  uint fontTextureSize = fontTexture.getWidth();
  uint glyphSize = fontTextureSize/16;
  float textureCoords[8];
  glEnable (GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fontTexture.texNameGl);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//GL_CLAMP_TO_EDGE);
  isglerr("Error glTexParameteri");
  glVertexPointer(2, GL_FLOAT, 0, points_);
  glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  int i,j;
  for(uint k=0;k<len;k++){
    i=text[k]/16;
    j=text[k]%16;
    textureCoords[0]=(float)j*glyphSize/fontTextureSize;	textureCoords[1]=(float)i*glyphSize/fontTextureSize;
    textureCoords[2]=(float)j*glyphSize/fontTextureSize;	textureCoords[3]=(float)(i+1)*glyphSize/fontTextureSize;
    textureCoords[4]=(float)(j+1)*glyphSize/fontTextureSize;textureCoords[5]=(float)(i+1)*glyphSize/fontTextureSize;
    textureCoords[6]=(float)(j+1)*glyphSize/fontTextureSize;textureCoords[7]=(float)i*glyphSize/fontTextureSize;

    points_[0] = xToGl(x+k*size);	   points_[1] = yToGl(y);
    points_[2] = xToGl(x+k*size);	   points_[3] = yToGl(y+size);
    points_[4] = xToGl(x+(k+1)*size); points_[5] = yToGl(y+size);
    points_[6] = xToGl(x+(k+1)*size); points_[7] = yToGl(y);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
  isglerr("Error drawText");
}