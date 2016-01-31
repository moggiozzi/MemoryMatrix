//#include <errno.h>
// !!! only in codding
//#include <android/input.h> // for AInputEvent, AINPUT_EVENT_TYPE_MOTION, ...
//#include <android/native_app_glue/android_native_app_glue.h> // for android_app
// !!! only in codding !!!

#include <time.h>
#include "Game.h"
#include "ResourceManager.h"
#include "GLHelper.h"
#include "AudioHelper.h"
#include "Settings.h"
#include <stdlib.h>

clock_t lastTime=0, currentTime=0;

Game game;

#ifdef __ANDROID__
#include <unistd.h> /* sleep() */
#include <android/sensor.h>

JNIEnv *jni;
struct engine {
  struct android_app* app;
  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;
  ASensorEventQueue* sensorEventQueue;
  int animating;
};

const char * saveFile = "/data/data/com.example.memorymatrix/save.dat";

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
  struct engine* engine = (struct engine*)app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION &&
    AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
      float x = AMotionEvent_getX(event, 0);
      float y = AMotionEvent_getY(event, 0);
      game.touch(x,y);
      engine->animating = 1;
      return 1;
  } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
      if ( AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN )
        return game.keyDown(AKeyEvent_getKeyCode(event));
  }
  return 0;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
  struct engine* engine = (struct engine*)app->userData;
  switch (cmd) {
    case APP_CMD_INPUT_CHANGED:
    break;
    case APP_CMD_INIT_WINDOW:
      if (engine->app->window != NULL) {
        ResourceManager::init(engine->app->activity);
        GLHelper::init(engine->app->window);
        AudioHelper::init(engine->app->activity);
        game.init();
        game.load(saveFile);
        game.draw();
      }
      engine->animating = 1;
    break;
    case APP_CMD_TERM_WINDOW:{
    	GLHelper::terminate();
    }break;
    case APP_CMD_WINDOW_RESIZED:
    break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
    break;
    case APP_CMD_CONTENT_RECT_CHANGED:
    break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
      if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
          engine->accelerometerSensor, (1000L/60)*1000);
      }
      engine->animating = 1;
    break;
    case APP_CMD_LOST_FOCUS:
      GLHelper::terminate();
      AudioHelper::stopAll(); /// \todo pause/resume all
      if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
      }
      // Also stop animating.
      engine->animating = 0;
    break;
    case APP_CMD_CONFIG_CHANGED:
    break;
    case APP_CMD_LOW_MEMORY:
    break;
    case APP_CMD_START:{
    	int d = 1;
    }break;
    case APP_CMD_RESUME:{
      int d=1;
    }break;
    case APP_CMD_SAVE_STATE:{
      game.save(saveFile);
    }break;
    case APP_CMD_PAUSE:{
      int p = 1;
    }break;
    case APP_CMD_STOP:{
    	int s=1;
    }break;
    case APP_CMD_DESTROY:{
    	int d=1;
    }break;
  }
}

void android_main(struct android_app* state) {
#ifndef NDEBUG
  // ожидание подключения отладчика
  //volatile bool bGo = false; // поймать
  volatile bool bGo = true; // не ловить
  while(!bGo) {
    sleep(1);
  }
#endif
  struct engine engine;
  // Make sure glue isn't stripped.
  app_dummy();

  memset(&engine, 0, sizeof(engine));
  state->userData = &engine;
  state->onAppCmd = engine_handle_cmd;
  state->onInputEvent = engine_handle_input;
  engine.app = state;

  // Prepare to monitor accelerometer
  engine.sensorManager = ASensorManager_getInstance();
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
    ASENSOR_TYPE_ACCELEROMETER);
  engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
    state->looper, LOOPER_ID_USER, NULL, NULL);

  while (1) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;
    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
      (void**)&source)) >= 0) {
        // Process this event.
        if (source != NULL) {
          source->process(state, source);
        }
        // If a sensor has data, process it now.
        if (ident == LOOPER_ID_USER) {
          if ( engine.accelerometerSensor != NULL) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0) {
              if ( settings.getControlMode() == CM_ACCEL )
              {
                if ( settings.getDeviceDefaultOrientation() == DO_HORIZONTAL )
                  game.accel( -event.acceleration.y, event.acceleration.x, event.acceleration.z);
                else // VERTICAL
                  game.accel( event.acceleration.x, event.acceleration.y, event.acceleration.z);
              }
            }
          }
        }
        if ( getGameState() == GS_EXIT ){
          //GLHelper::terminate();
          //AudioHelper::destroy();
          //state->destroyRequested = 1;
          //ANativeActivity_finish(state->activity);
          game.save(saveFile);
          exit(0); // hack
          return;
        }
    }
    if (engine.animating) {
      currentTime = clock();
      game.update((float)(currentTime-lastTime)/CLOCKS_PER_SEC);
      lastTime = currentTime;
      game.draw();
    }
  }
}
#elif _WIN32

#include <GL/freeglut.h>
#pragma comment(lib, "../OpenAL/lib/alut.lib")
#pragma comment(lib, "../OpenAL/lib/OpenAL32.lib")
#pragma comment(lib, "../OpenAL/lib/ogg.lib")
#pragma comment(lib, "../OpenAL/lib/vorbisfile.lib")
#pragma comment(lib, "../freeglut-2.8.0/lib/freeglut.lib")
#pragma comment(lib, "../DevIL-SDK-x86-1.7.8/lib/DevIL.lib")
#pragma comment(lib, "../DevIL-SDK-x86-1.7.8/lib/ILU.lib")
#pragma comment(lib, "../DevIL-SDK-x86-1.7.8/lib/ILUT.lib")
int nWindow;
int nLoopMain = 0;

void handleMouse( int button, int state, int x, int y){
  if ( button == 0 && state==0 )
    game.touch( x, y );
}

void handleInput( int keyCode, int mouseX, int mouseY ) {
  game.keyDown( keyCode );
}

void handleInput( unsigned char keyCode, int mouseX, int mouseY )
{
  game.keyDown( keyCode );
  if (keyCode=='f')
    glutFullScreenToggle();
}

void Idle(void)
{
  glutPostRedisplay();
}

void handleReshape(int x, int y)
{
  //nWidth = glutGet(GLUT_WINDOW_WIDTH);
  //nHeight = glutGet(GLUT_WINDOW_HEIGHT);
  //glViewport(0,0,nWidth,nHeight);
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //gluOrtho2D(0,nWidth,0,nHeight);
}

void draw(void)
{
  // todo сделать вне функции draw с помощью glutTimerFunc
  //Sleep(300); // for test
  currentTime = clock();
  AudioHelper::update();
  game.update((float)(currentTime-lastTime)/CLOCKS_PER_SEC);
  if ( getGameState() == GS_EXIT )
    glutLeaveMainLoop();
  lastTime = currentTime;

  game.draw();
}

void initGlut(int argc, char* argv[]){
  glutInit( &argc, argv );
#ifdef _DEBUG
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  GLHelper::setWidth(1024);
  GLHelper::setHeight(480);
#else
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_BORDERLESS);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  GLHelper::setWidth(glutGet(GLUT_SCREEN_WIDTH));
  GLHelper::setHeight(glutGet(GLUT_SCREEN_HEIGHT));
#endif

  int wx = (glutGet(GLUT_SCREEN_WIDTH)-GLHelper::getWidth())/2;
  int wy = (glutGet(GLUT_SCREEN_HEIGHT)-GLHelper::getHeight())/2;
  glutInitWindowPosition( wx, wy );
  glutInitWindowSize(GLHelper::getWidth(),GLHelper::getHeight());

  nWindow = glutCreateWindow("MemoryMatrix");

  glutKeyboardFunc( handleInput );
  glutSpecialFunc( handleInput );
  glutMouseFunc( handleMouse );

  glutDisplayFunc( draw );
  glutReshapeFunc( handleReshape );
}

int main(int argc, char* argv[])
{
  srand((uint)time(NULL));
  
  initGlut(argc,argv);
  ResourceManager::init();
  GLHelper::init();
  AudioHelper::init();

  game.init();

  currentTime = lastTime = clock();
  glutMainLoop();

  return 0;
}
#elif __linux__
#endif
