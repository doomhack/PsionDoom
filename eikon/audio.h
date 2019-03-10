/*******************************************************************
 *
 *	File:		Audio.h
 *
 *	Author:		Peter van Sebille (peter@yipton.demon.co.uk)
 *
 *	(c) Copyright 2001, Peter van Sebille
 *	All Rights Reserved
 *
 *******************************************************************/

#ifndef __AUDIO_H
#define __AUDIO_H


#include <e32base.h>
#include <w32std.h>
#include <d32snd.h>


//We're just going to do a little fake SDL audio wrapper thing here.

typedef struct
{
  int freq;
  unsigned short format;
  unsigned char channels;
  unsigned char silence;
  unsigned short samples;
  unsigned int size;
  void (*callback)(void *userdata, unsigned char *stream, int len);
  void *userdata;
} SDL_AudioSpec;


int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);

//******************************************************************

class CGameAudio : CBase
{
public:
	static CGameAudio* NewL(TInt aSamplesPerFrame, SDL_AudioSpec* audioSpec);
	~CGameAudio();
	void Destruct();
	void SoundUpdate();
	TInt ProcessSoundSamples(TInt16* aBuffer);
	TInt FirstNoOfSamples();

	void StartAudioLoop();

private:
	CGameAudio(TInt aSamplesPerFrame, SDL_AudioSpec* audioSpec);
	void ConstructL();

	void BuildAlawTable();

	RDevSound		iDevSound;
	TRequestStatus	iStatus;



	TUint8*			iAlawSoundBuffer;
	TInt16*			iPcmSoundBuffer;

	TUint32			iBufferTimeUs;
	
	TUint8			iAlawLookupTable[8192];

	TInt			iSamplesPerFrame;

	SDL_AudioSpec   iAudioSpec;
};


#endif			/* __AUDIO_H */
