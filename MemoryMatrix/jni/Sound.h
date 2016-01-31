#ifndef SOUND_H_
#define SOUND_H_

#ifdef __ANDROID__
#elif _WIN32
//===========================================================
//      Sound class.
//      Copyright 2003
//      Created by Roman E. Marchenko, vortex@library.ntu-kpi.kiev.ua
//      $Id: remSnd.h,v 1.2 2003/09/26 21:04:57 vortex Exp $
//===========================================================

#include <AL/al.h>
#include <fstream>
#include <vorbis/vorbisfile.h>

#define NUM_OF_DYNBUF	2		// num buffers in queue

class Sound
{
public:
	ALfloat mVel[3];
	ALfloat mPos[3];
	bool	mLooped;
	
	// Functions
	bool Open(const char * fileName, bool Streamed = true);
	bool IsStreamed();
	void Play(bool isLoop = false);
	void Close();
	void Update();
	void Move(float X, float Y, float Z);
	void Stop();

	// Construction/destruction
	Sound();
	virtual ~Sound();

private:
	// OAL specific vars
	ALuint			mSourceID;
	// vorbisfile specific vars
	OggVorbis_File	*mVF;
	vorbis_comment	*mComment;
	vorbis_info		*mInfo;
	// Common vars
	std::ifstream	OggFile;
	bool			mStreamed;

	// Functions
	bool ReadOggBlock(ALuint BufID, size_t Size);
	bool LoadOggFile (const std::string &Filename, bool Streamed);
	bool LoadWavFile (const std::string &Filename);
};

// Public standalone functions
extern std::string	GetSndInfo();
extern bool			InitializeOpenAL();
extern void			DestroyOpenAL();

#endif

#endif