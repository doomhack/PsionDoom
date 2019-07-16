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
  unsigned char channels;
  unsigned short samples;
  unsigned int size;
  void (*callback)(unsigned char *stream, const int len);
} AudioSpec;


int OpenAudio(AudioSpec *desired);

void StopAudio();

//******************************************************************

class CGameAudio : CBase
{
public:
	static CGameAudio* NewL(TInt aSamplesPerFrame, AudioSpec* audioSpec);
	~CGameAudio();
	void Destruct();
	void SoundUpdate();

	void StartAudioLoop();

	void StopAudioLoop();

private:
	CGameAudio(TInt aSamplesPerFrame, AudioSpec* audioSpec);
	void ConstructL();

	RDevSound		iDevSound;
	TRequestStatus	iStatus;



	TUint8*			iAlawSoundBuffer;

	TInt			iSamplesPerFrame;

	AudioSpec		iAudioSpec;

	volatile bool	bQuitAudio;
};


#endif			/* __AUDIO_H */
