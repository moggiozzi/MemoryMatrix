#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef __ANDROID__
#include <jni.h>
#include "android_native_app_glue.h"
#include <android/log.h>

#ifndef NDEBUG
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define isnull(str,var) if(var==NULL)LOGI("%s is NULL",str);else LOGI("%s is OK",str);
#define isglerr(str) {int err; if((err=glGetError())!=GL_NO_ERROR) LOGI("%s error 0x%X", str, err);}
#else // for release
#define DOEMPTYDEFS
#endif

#elif _WIN32

#ifdef _DEBUG
#define LOGI(format,...) (LogMessage(format,__VA_ARGS__))
#define LOGW(format,...) (LogMessage(format,__VA_ARGS__))
void LogMessage ( char* format, ...);
#define isnull(str,var) if(var==NULL)LOGI("%s is NULL",str);else LOGI("%s is OK",str);
#define isglerr(str) {int err; if((err=glGetError())!=GL_NO_ERROR) LOGI("%s error 0x%X", str, err);}
#else
#define DOEMPTYDEFS
#endif

#elif __linux__
//defs for linux
#endif

// for release
#ifdef DOEMPTYDEFS
#define LOGI(...)
#define LOGW(...)
#define isnull(str,var)
#define isglerr(str)
#endif

typedef unsigned int uint;
typedef unsigned char uchar;

#endif // GLOBAL_H_
