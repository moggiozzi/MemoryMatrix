#include "AudioHelper.h"
#include "Sound.h"

Sounds AudioHelper::sounds;
int AudioHelper::lastSoundId=0;
bool AudioHelper::isInit=false;

#ifdef __ANDROID__
#include <assert.h>
// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// engine interfaces
SLObjectItf AudioHelper::engineObject;
SLEngineItf AudioHelper::engineEngine;
// output mix interfaces
SLObjectItf AudioHelper::outputMixObject;
AAssetManager *AudioHelper::assetManager;

bool AudioHelper::init(ANativeActivity* activity){
  if (isInit)
  	return true;
  lastSoundId = 0;
	assetManager = activity->assetManager;
  SLresult result;
  // create engine
  result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
  assert(SL_RESULT_SUCCESS == result);
  // realize the engine
  result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);
  // get the engine interface, which is needed in order to create other objects
  result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
  assert(SL_RESULT_SUCCESS == result);
  // create output mix
  result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
  assert(SL_RESULT_SUCCESS == result);
  // realize the output mix
  result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);
  isInit = true;
  return isInit;
}
void destroy(){
}
// create asset audio player
SLObjectItf AudioHelper::createAssetAudioPlayer(const char * fileName)
{
  SLresult result;
  SLObjectItf fdPlayerObject = NULL;

  AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_UNKNOWN);
  if (NULL == asset) {
    LOGI("asset %s is null", fileName);
    return NULL;
  }
  // open asset as file descriptor
  off_t start, length;
  int fd = AAsset_openFileDescriptor(asset, &start, &length);
  assert(0 <= fd);
  AAsset_close(asset);
  // configure audio source
  SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
  SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
  SLDataSource audioSrc = {&loc_fd, &format_mime};
  // configure audio sink
  SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
  SLDataSink audioSnk = {&loc_outmix, NULL};
  // create audio player
  const SLInterfaceID ids[2] = {SL_IID_SEEK, SL_IID_VOLUME};
  const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
  result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
    2, ids, req);
  assert(SL_RESULT_SUCCESS == result);
  // realize the player
  result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);
  //    // get the seek interface
  //    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
  //    assert(SL_RESULT_SUCCESS == result);
  //    // get the volume interface
  //    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
  //    assert(SL_RESULT_SUCCESS == result);
  //    // enable whole file looping
  //    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
  //    assert(SL_RESULT_SUCCESS == result);
  return fdPlayerObject;
}

bool AudioHelper::open(const char *file, int& soundId, bool isStreamed){
  if (!isInit)
    return false;
	SLObjectItf fdPlayerObject = createAssetAudioPlayer(file);
  if(fdPlayerObject != NULL){
    soundId = lastSoundId;
    sounds.insert(std::make_pair<int,SLObjectItf>(lastSoundId++,fdPlayerObject));
  }
}

void AudioHelper::play(int soundId, bool isLoop){
  if (!isInit)
    return;
	SLresult result;
	SLObjectItf fdPlayerObject;
	SLPlayItf fdPlayerPlay;
  Sounds::iterator it = sounds.find(soundId);
	if(it != sounds.end()){
		fdPlayerObject = it->second;
	}
  result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
  assert(SL_RESULT_SUCCESS == result);
  result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_STOPPED);
  result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_PLAYING);
}

void AudioHelper::stop(int soundId){
  if (!isInit)
    return;
	SLresult result;
	SLObjectItf fdPlayerObject;
	SLPlayItf fdPlayerPlay;
  Sounds::iterator it = sounds.find(soundId);
	if(it == sounds.end())
		return;
	fdPlayerObject = it->second;
  result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
  assert(SL_RESULT_SUCCESS == result);
  result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_STOPPED);
}
void AudioHelper::update(){}
void AudioHelper::stopAll(){
	SLresult result;
	SLObjectItf fdPlayerObject;
	SLPlayItf fdPlayerPlay;
  for(Sounds::iterator it = sounds.begin(); it!= sounds.end(); ++it){
    fdPlayerObject = it->second;
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_STOPPED);
  }
}
void AudioHelper::pauseAll(){
	SLresult result;
	SLObjectItf fdPlayerObject;
	SLPlayItf fdPlayerPlay;
  for(Sounds::iterator it = sounds.begin(); it!= sounds.end(); ++it){
    fdPlayerObject = it->second;
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_PAUSED);
  }
}
void AudioHelper::resumeAll(){
	SLresult result;
	SLObjectItf fdPlayerObject;
	SLPlayItf fdPlayerPlay;
  for(Sounds::iterator it = sounds.begin(); it!= sounds.end(); ++it){
    fdPlayerObject = it->second;
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    SLuint32 state;
    result = (*fdPlayerPlay)->GetPlayState(fdPlayerPlay, &state);
    if ( state == SL_PLAYSTATE_PAUSED )
      result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_PLAYING);
  }
}
#elif _WIN32
bool AudioHelper::init(){
  isInit = InitializeOpenAL();
  lastSoundId = 0;
  return isInit;
}

bool AudioHelper::destroy(){
  DestroyOpenAL();
  return true;
}

bool AudioHelper::open(const char *file, int &soundId, bool isStreamed){
  if (!isInit)
    return false;
  Sound *sound = new Sound();
  if (!sound || !sound->Open(file, isStreamed)) {
    LOGI("Sound %s not open", file);
	delete sound;
	return false;
  }
  soundId = lastSoundId;
  sounds[lastSoundId++] = sound;
  return true;
}

Sound* AudioHelper::getSound(int soundId){
  std::map<int,Sound*>::iterator it = sounds.find(soundId);
  if (it!=sounds.end())
    return it->second;
  static bool shouldErrReport=true;
  if (shouldErrReport) {
    shouldErrReport = false;
    LOGI("Sound with id=%d not found", soundId);
  }
  return 0;
}

void AudioHelper::play(int soundId, bool isLoop){
  Sound *sound = getSound(soundId);
  if ( sound )
    sound->Play(isLoop);
}

void AudioHelper::stop(int soundId){
  Sound *sound = getSound(soundId);
  if ( sound )
    sound->Stop();
}

void AudioHelper::update(){
  for(Sounds::iterator it = sounds.begin(); it!= sounds.end(); ++it)
    if (it->second->IsStreamed())
      it->second->Update();
}

#endif
