
#ifdef __ANDROID__
#elif _WIN32
//===========================================================
//      Sound class.
//      Copyright 2003
//      Created by Roman E. Marchenko, vortex@library.ntu-kpi.kiev.ua
//      $Id: Sound.cpp,v 1.2 2003/09/26 21:04:57 vortex Exp $
//===========================================================

#include "Sound.h"
#include <windows.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alu.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <map>
#include <string>
#include <sstream>

//#pragma comment(lib, "alut.lib")
//#pragma comment(lib, "OpenAL32.lib")
//#pragma comment(lib, "ogg.lib")
//#pragma comment(lib, "vorbisfile.lib")

using namespace std;

// Defines
#define NUM_OF_DYNBUF	2		// num buffers in queue
#define DYNBUF_SIZE		10024	// Buffer size
#define ERRMSG(Msg) MessageBox(NULL, Msg, "Error", MB_OK | MB_ICONEXCLAMATION)

// Typedefs
typedef struct 
{
  unsigned int	ID;
  string  Filename;
  unsigned int	Rate;
  unsigned int	Format;
} SndInfo;

typedef map<ALuint, SndInfo> TBuf;

// Vars
TBuf		Buffers;
ALCdevice	*pDevice;
ALCcontext	*pContext;

//-----------------------------------------------------------
//    Standalone OpenAL function implementation
//-----------------------------------------------------------

ALboolean CheckALCError()
{
  ALenum ErrCode;
  string Err = "ALC error: ";
  if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
  {
    Err += (char *)alcGetString(pDevice, ErrCode);
    ERRMSG(Err.data());
    return AL_FALSE;
  }
  return AL_TRUE;
}

ALboolean CheckALError()
{
  ALenum ErrCode;
  string Err = "OpenAL error: ";
  if ((ErrCode = alGetError()) != AL_NO_ERROR)
  {
    Err += (char *)alGetString(ErrCode);
    ERRMSG(Err.data());
    return AL_FALSE;
  }
  return AL_TRUE;
}

bool InitializeOpenAL()
{

  // Position of the Listener.
  ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

  // Velocity of the Listener.
  ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

  // Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
  // Also note that these should be units of '1'.
  ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

  // Open default sound device
  pDevice = alcOpenDevice(NULL);
  // Check for errors
  if (!pDevice)
  {
    ERRMSG("Default sound device not present");
    return false;
  }
  // Creating rendering context
  pContext = alcCreateContext(pDevice, NULL);
  if (!CheckALCError()) return false;

  alcMakeContextCurrent(pContext);

  // Set listener properties
  alListenerfv(AL_POSITION,    ListenerPos);
  alListenerfv(AL_VELOCITY,    ListenerVel);
  alListenerfv(AL_ORIENTATION, ListenerOri);
  return true;
}

void DestroyOpenAL()
{
  // Clear all buffers and sources
  for (TBuf::iterator i = Buffers.begin(); i != Buffers.end(); i++)
    alDeleteBuffers(1, &i->second.ID);
  // Shut down context
  alcMakeContextCurrent(NULL);
  // Destroy context
  alcDestroyContext(pContext);
  // Close sound device
  alcCloseDevice(pDevice);
}

string GetSndInfo()
{
  ostringstream SOut;
  SOut << "ALC info: \n";
  SOut << "DEFAULT_DEVICE_SPECIFIER: " << (char *)alcGetString(pDevice, ALC_DEFAULT_DEVICE_SPECIFIER) << '\n';
  SOut << "DEVICE_SPECIFIER: " << (char *)alcGetString(pDevice, ALC_DEVICE_SPECIFIER) << '\n';
  SOut << "EXTENSIONS: " << (char *)alcGetString(pDevice, ALC_EXTENSIONS) << '\n' << '\n';
  SOut << "AL info: \n";
  SOut << "VERSION: " << (char *)alGetString(AL_VERSION) << '\n';
  SOut << "RENDERER: " << (char *)alGetString(AL_RENDERER) << '\n';
  SOut << "VENDOR: " << (char *)alGetString(AL_VENDOR) << '\n';
  SOut << "EXTENSIONS: " << (char *)alGetString(AL_EXTENSIONS) << '\n' << ends;
  return SOut.str();
}

//-----------------------------------------------------------
//    ov_callback specific functions
//-----------------------------------------------------------

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  File->read((char *)ptr, size * nmemb);
  return File->gcount();
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  ios_base::seekdir Dir;
  File->clear();
  switch (whence) 
  {
  case SEEK_SET: Dir = ios::beg;  break;
  case SEEK_CUR: Dir = ios::cur;  break;
  case SEEK_END: Dir = ios::end;  break;
  default: return -1;
  }
  File->seekg((streamoff)offset, Dir);
  return (File->fail() ? -1 : 0);
}

long TellOgg(void *datasource)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  return File->tellg();
}

int CloseOgg(void *datasource)
{
  return 0;
}

//-----------------------------------------------------------
//    Sound class
//-----------------------------------------------------------

Sound::Sound()
{
  for (int i = 0; i < 3; i++)
  {
    mPos[i] = mVel[i] = 0.0f;
  }
  mInfo		= NULL; 
  mVF			= NULL;
  mComment	= NULL;
}

Sound::~Sound()
{
}

bool Sound::Open(const char *fileName, bool Streamed)
{
  // Check file existance
  ifstream a(fileName);
  if (!a.is_open()) return false;
  a.close();

  mLooped	= false;

  // Create source
  alGenSources(1, &mSourceID);
  if (!CheckALError()) return false;

  alSourcef (mSourceID, AL_PITCH,		1.0f);
  alSourcef (mSourceID, AL_GAIN,		1.0f);
  alSourcefv(mSourceID, AL_POSITION,	mPos);
  alSourcefv(mSourceID, AL_VELOCITY,	mVel);
  alSourcei (mSourceID, AL_LOOPING,	mLooped);

  // Extract ext
  int len = strnlen(fileName, 255);
  if ( strncmp(&fileName[len-3],"wav",3) == 0 )
    return LoadWavFile(fileName);
  if ( strncmp(&fileName[len-3],"ogg",3) == 0 )
  {
    mStreamed = Streamed;
    return LoadOggFile(fileName, Streamed);
  }
  return false;
}

bool Sound::IsStreamed()
{
  return mStreamed;
}

bool Sound::LoadWavFile(const string &Filename)
{
  SndInfo		buffer;
  ALenum		format;
  ALvoid		*data;
  ALsizei		size;
  ALsizei		freq;
  ALboolean	loop;
  ALuint		BufID = 0;

  // Fill buffer struct
  buffer.Filename = Filename;
  // Check for existance of sound
  for (TBuf::iterator i = Buffers.begin(); i != Buffers.end(); i++)
  {
    if (i->second.Filename == Filename) BufID = i->first;
  }

  // If Such buffer not exist yet
  if (!BufID)
  {
    // Generate buffers
    alGenBuffers(1, &buffer.ID);
    if (!CheckALError()) return false;
    // Load wav data
    alutLoadWAVFile((ALbyte *)Filename.data(), &format, &data, &size, &freq, &loop);
    if (!CheckALError()) return false;

    buffer.Format			= format;
    buffer.Rate				= freq;
    // Fill buffer
    alBufferData(buffer.ID, format, data, size, freq);
    if (!CheckALError()) return false;
    // Unload wav
    alutUnloadWAV(format, data, size, freq);
    if (!CheckALError()) return false;

    Buffers[buffer.ID] = buffer;
  }
  else 
    buffer = Buffers[BufID];

  alSourcei (mSourceID, AL_BUFFER,	buffer.ID);

  // Place buffer to Buffers map
  return true;
}

bool Sound::LoadOggFile(const string &Filename, bool Streamed)
{
  int				i, DynBuffs = 1, BlockSize;
  // OAL specific
  SndInfo			buffer;
  ALuint			BufID = 0;
  // OggVorbis specific structures
  ov_callbacks	cb;

  // Fill cb struct
  cb.close_func	= CloseOgg;
  cb.read_func	= ReadOgg;
  cb.seek_func	= SeekOgg;
  cb.tell_func	= TellOgg;

  // Create OggVorbis_File struct
  mVF = new OggVorbis_File;

  // Open Ogg file
  OggFile.open(Filename.c_str(), ios_base::in | ios_base::binary);

  // Generate local buffers
  if (ov_open_callbacks(&OggFile, mVF, NULL, -1, cb) < 0)
  {
    // This is not ogg bitstream. Return
    return false;
  }

  // Check for existance of sound
  if (!Streamed)
  {
    for (TBuf::iterator i = Buffers.begin(); i != Buffers.end(); i++)
    {
      if (i->second.Filename == Filename) BufID = i->first;
    }
    BlockSize = ov_pcm_total(mVF, -1) * 4;
  }
  else
  {
    BlockSize	= DYNBUF_SIZE;
    DynBuffs	= NUM_OF_DYNBUF;
    alSourcei(mSourceID, AL_LOOPING, AL_FALSE);
  }

  // Return vorbis_comment and vorbis_info structures
  mComment		= ov_comment(mVF, -1);
  mInfo			= ov_info(mVF, -1);

  // Fill buffer infos
  buffer.Rate		= mInfo->rate;
  buffer.Filename	= Filename;
  buffer.Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  // Fill buffers with data each block by DYNBUF_SIZE bytes
  if (Streamed || !BufID)
  {
    for (i = 0; i < DynBuffs; i++)
    {
      // Create buffers
      alGenBuffers(1, &buffer.ID);
      if (!CheckALError()) return false;
      Buffers[buffer.ID] = buffer;
      // Read amount (DYNBUF_SIZE) data into each buffer
      ReadOggBlock(buffer.ID, BlockSize);
      if (!CheckALError()) return false;

      if (Streamed) // Place buffer to queue
      {
        alSourceQueueBuffers(mSourceID, 1, &buffer.ID);
        if (!CheckALError()) return false;
      }
      else 
        alSourcei(mSourceID, AL_BUFFER, buffer.ID);
    }
  }
  else
  {
    alSourcei(mSourceID, AL_BUFFER, Buffers[BufID].ID);
  }

  return true;
}

bool Sound::ReadOggBlock(ALuint BufID, size_t Size)
{
  // vars
  char		eof = 0;
  int			current_section;
  long		TotalRet = 0, ret;
  char		*PCM;

  if (Size < 1) return false;
  PCM = new char[Size];

  // Read loop
  while (TotalRet < Size) 
  {
    ret = ov_read(mVF, PCM + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

    // if end of file or read limit exceeded
    if (ret == 0) break;
    else if (ret < 0) 		// Error in bitstream
    {
      //
    }
    else
    {
      TotalRet += ret;
    }
  }
  if (TotalRet > 0)
  {
    alBufferData(BufID, Buffers[BufID].Format, (void *)PCM, 
      TotalRet, Buffers[BufID].Rate);
    CheckALError();
  }
  delete [] PCM;
  return (ret > 0);
}

void Sound::Play(bool isLoop)
{
  mLooped = isLoop;
  alSourcei (mSourceID, AL_LOOPING,	mLooped);
  alSourcePlay(mSourceID);
}

void Sound::Close()
{
  alSourceStop(mSourceID);
  if (alIsSource(mSourceID)) alDeleteSources(1, &mSourceID);
  if (!mVF)
  {	
    ov_clear(mVF);
    delete mVF;
  }
}

void Sound::Update()
{
  if (!mStreamed) return;

  int				Processed = 0;
  ALuint			BufID;

  alGetSourcei(mSourceID, AL_BUFFERS_PROCESSED, &Processed);

  // We still have processed buffers
  while (Processed--)
  {
    alSourceUnqueueBuffers(mSourceID, 1, &BufID);
    if (!CheckALError()) return;
    if (ReadOggBlock(BufID, DYNBUF_SIZE) != 0)
    {
      alSourceQueueBuffers(mSourceID, 1, &BufID);
      if (!CheckALError()) return;
    }
    else
    {
      ov_pcm_seek(mVF, 0);
      alSourceQueueBuffers(mSourceID, 1, &BufID);
      if (!CheckALError()) return;

      if (!mLooped) Stop();
    }
  }
}

void Sound::Stop()
{
  alSourceStop(mSourceID);
}

void Sound::Move(float X, float Y, float Z)
{
  ALfloat Pos[3] = { X, Y, Z };
  alSourcefv(mSourceID, AL_POSITION, Pos);
}

#endif