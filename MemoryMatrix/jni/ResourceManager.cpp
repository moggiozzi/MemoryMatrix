#include "ResourceManager.h"
#include "GLHelper.h"

bool ResourceManager::isInit = false;
uint ResourceManager::lastTextureID;
Textures ResourceManager::textures;

#ifdef __ANDROID__
JavaVM *ResourceManager::jvm;
JNIEnv *ResourceManager::env;
AAssetManager *ResourceManager::assetManager;
jclass    ResourceManager::bitmapFactoryClass;
jmethodID ResourceManager::decodeByteArrayID;
jclass    ResourceManager::bitmapClass;
jmethodID ResourceManager::getPixelsID;

bool ResourceManager::init(ANativeActivity* activity) {
  lastTextureID = 0;
  textures.clear();

  env = activity->env;
  jvm = activity->vm;
  jvm->AttachCurrentThread(&env, NULL);
  assetManager = activity->assetManager;

  bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
  isnull("bitmapFactory", bitmapFactoryClass);

  //decodeByteArray(byte[] data, int offset, int length)
  decodeByteArrayID = env->GetStaticMethodID(bitmapFactoryClass, "decodeByteArray",
    "([BII)Landroid/graphics/Bitmap;");
  isnull("decodeByteArrayID", decodeByteArrayID);

  bitmapClass = env->FindClass("android/graphics/Bitmap");
  isnull("bitmapClass", bitmapClass);

  //getPixels(pixels, offset, stride, x, y, width, height)
  getPixelsID = env->GetMethodID(bitmapClass, "getPixels", "([IIIIIII)V");
  isnull("getPixelsID", getPixelsID);
  isInit = true;
  return isInit;
}

int ResourceManager::loadImage(const char *path, Texture *tex, int format) {
  LOGI("loading %s", path);
  AAsset *asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);
  uint bufSize = AAsset_getLength(asset);
  char* buffer = (char*) AAsset_getBuffer(asset);
  jbyteArray bytes = env->NewByteArray(bufSize);
  env->SetByteArrayRegion(bytes, 0, bufSize, (jbyte*) buffer);
  AAsset_close(asset);

  jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeByteArrayID, bytes, 0, bufSize);
  jthrowable exc = env->ExceptionOccurred();
  if (exc != NULL) {
    LOGI("Exception in decoding file %s!", path);
    env->ExceptionDescribe();
    env->ExceptionClear();
  }
  env->DeleteLocalRef(bytes);
  if (exc != NULL || bitmap == NULL) {
    LOGI("while load %s exception occured",path);
    return -1;
  }
  AndroidBitmapInfo bitmapInfo;
  int ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
  if (bitmap == NULL || ret != 0) {
    LOGI("%s is not loaded", path);
    return -1;
  }
  jintArray pixels = env->NewIntArray(bitmapInfo.width * bitmapInfo.height);
  env->CallVoidMethod(bitmap, getPixelsID, pixels, 0, bitmapInfo.width, 0, 0,
    bitmapInfo.width, bitmapInfo.height); // ??? CallObjectMethod
  uint dataSize = bitmapInfo.width*bitmapInfo.height;
  uint *textureData = new uint[dataSize];
  env->GetIntArrayRegion(pixels,0,dataSize,(jint*)textureData);

  //todo разрбраться с порядком байтов
  for(uint i=0;i<dataSize;i++){
    textureData[i] =
      (textureData[i] & 0xff00ff00) |
      (textureData[i]<<16 & 0x00ff0000) |
      (textureData[i]>>16 & 0x000000ff);
  }
  //todo

  tex->size.set( bitmapInfo.width, bitmapInfo.height );
  tex->format = format;
  tex->pixels = (char*)textureData;
  glGenTextures(1, &tex->texNameGl);
  glBindTexture(GL_TEXTURE_2D, tex->texNameGl);
  glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->getWidth(), tex->getHeight(), 0,
    tex->format, GL_UNSIGNED_BYTE, tex->pixels);
  isglerr("Error glTexImage2d");
  lastTextureID++;
  textures.insert(std::pair<uint,Texture*>(lastTextureID,tex));
  LOGI("loading %s is OK; format:%d", path, format);
  return lastTextureID;
}

char* ResourceManager::getBytes(const char *fileName, uint &length){
  LOGI("loading %s", fileName);
  jmethodID mid;
  AAsset *asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_UNKNOWN);
  if(asset==NULL){
    length=0;
    return 0;
  }
  uint bufSize = AAsset_getLength(asset);
  char* buffer = new char[bufSize];
  int ri = AAsset_read(asset, buffer, bufSize);
  if(ri==0){
    length=0;
    return 0;
    delete buffer;
  }
  length = bufSize;
  return buffer;
}

#elif _WIN32
#include <IL/il.h>
bool ResourceManager::init(){
  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
  {
    /* wrong DevIL version */
    return false;
  }
  ilInit(); /* Initialization of DevIL */
  return true;
}

int ResourceManager::loadImage(const char *path, Texture *tex, int format){
  ilGenImages(1, (ILuint*)&tex->id);
  ilBindImage(tex->id);
  bool success = ilLoadImage((const ILstring)path) != 0;
  if (success) {
    //success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    if (!success) {
      LOGI("Error ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)");
      return false;
    }
    tex->format = format;
    tex->size.setX( ilGetInteger(IL_IMAGE_WIDTH) );
    tex->size.setY( ilGetInteger(IL_IMAGE_HEIGHT) );
    tex->pixels = (char*)ilGetData();
    glGenTextures(1, &tex->texNameGl);
    glBindTexture(GL_TEXTURE_2D, tex->texNameGl);
    isglerr("Err glBindTexture");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->getWidth(), tex->getHeight(), 0,
      tex->format, GL_UNSIGNED_BYTE, tex->pixels);
    isglerr("Err glTexImage2D");
  } else {
    LOGI("Error: ilLoadImage %s", path);
    return 0;
  }
  lastTextureID++;
  textures.insert(std::pair<uint,Texture*>(lastTextureID,tex));
  LOGI("loading %s is OK; format:%d", path, format);
  return lastTextureID;
}

#elif __linux__
#endif

Texture* ResourceManager::getTexture(uint id) {
  std::map<uint,Texture*>::iterator it = textures.find(id);
  if (it!=textures.end())
    return it->second;
  LOGI("texture with id=%d not exist",id);
  return 0;
}

bool ResourceManager::freeData(int id){
  Texture *t = getTexture(id);
  if(t){
    if(t->isGenTexNameGl)
      glDeleteTextures(1, &t->texNameGl);
#ifdef __ANDROID__
    delete[] t->pixels;
#elif defined(_WIN32) || defined(__linux__)
    ilDeleteImages(1, (ILuint*)&t->id);
#endif
    return true;
  }
  return false;
}
