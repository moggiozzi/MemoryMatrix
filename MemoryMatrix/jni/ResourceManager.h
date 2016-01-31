#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#ifndef GLHELPER_H_
#include "GLHelper.h"
#endif

#ifdef __ANDROID__
#include "android/bitmap.h"
#endif
#include <map>

typedef std::map<uint, Texture*> Textures;
class ResourceManager{
  static bool isInit;
  static uint lastTextureID;
  static Textures textures;
#ifdef __ANDROID__
  static JavaVM *jvm;
  static JNIEnv *env;
  static AAssetManager *assetManager;

  static jclass    bitmapFactoryClass;
  static jmethodID decodeByteArrayID;
  static jclass    bitmapClass;
  static jmethodID getPixelsID;
public:
  static bool init(ANativeActivity* activity);
#elif _WIN32
public:
  static bool init();
#elif __linux__
#endif

  static Texture* getTexture(uint id);
  static int loadImage(const char *path, Texture *tex, int format = GL_RGBA);
  static char* getBytes(const char *fileName, uint &length);
  static bool freeData(int id);
};

#endif
