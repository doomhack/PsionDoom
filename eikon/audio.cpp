/*******************************************************************
 *
 *	File:		Audio.cpp
 *
 *	Author:		Peter van Sebille (peter@yipton.demon.co.uk)
 *
 *	(c) Copyright 2001, Peter van Sebille
 *	All Rights Reserved
 *
 *******************************************************************/

#include <string.h>
#include "audio.h"

/*
 * In ER5 we can directly interface with the sound driver. In ER6 the preferred
 * way is to interace with the MediaServer. Apart from not having figured out
 * the MediaServer API, I suspect it will require client-server interaction.
 * Most likely that will be killing for EMame's performance. 
 */


RThread*		pSoundThread = NULL;
CGameAudio*		pAudio = NULL;


void OpenAudioOnThread(AudioSpec *desired)
{
	pAudio = CGameAudio::NewL(desired->samples, desired);

	if(pAudio)
	{
		pAudio->StartAudioLoop();
	}
}


TInt OpenAudioThreadFunc(TAny* aParams)
{

    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TRAPD(err, OpenAudioOnThread( (AudioSpec*)aParams ););

    delete cleanupStack;


    return KErrNone;
}


int OpenAudio(AudioSpec *desired)
{
	pSoundThread = new (ELeave)RThread();

	pSoundThread->Create(_L("Sound Thread"), OpenAudioThreadFunc, 16384, KMinHeapSize, 1*1024*1024, (TAny*)desired);

	pSoundThread->Resume();

	return 1;
}

void StopAudio()
{
	if(pAudio != NULL)
		pAudio->StopAudioLoop();
}

/*******************************************
 *
 * CGameAudio
 *
 *******************************************/

CGameAudio::CGameAudio(TInt aSamplesPerFrame, AudioSpec* audioSpec) : iSamplesPerFrame(aSamplesPerFrame)
{
	iAudioSpec = *audioSpec;
	bQuitAudio = false;
}


CGameAudio* CGameAudio::NewL(TInt aSamplesPerFrame, AudioSpec* audioSpec)
{
	CGameAudio*		self = new(ELeave) CGameAudio(aSamplesPerFrame, audioSpec);
	self->ConstructL();
	return self;
}


CGameAudio::~CGameAudio()
{
	Destruct();
}

void CGameAudio::Destruct()
{
	if (iDevSound.Handle())
		iDevSound.Close();

	delete[] iAlawSoundBuffer;
}



void CGameAudio::ConstructL()
{
	User::LeaveIfError(iDevSound.Open());

	TSoundCapsV01			soundCaps;
	TPckg<TSoundCapsV01>	caps(soundCaps);

	iDevSound.Caps(caps);

	TSoundConfigV01			soundConfig;
	TPckg<TSoundConfigV01>	sc(soundConfig);

	iDevSound.Config(sc);
	soundConfig.iAlawBufferSize = iSamplesPerFrame << 1;
	soundConfig.iVolume = EVolumeMedium;
	soundConfig.iVolumeValue = 1;
	User::LeaveIfError(iDevSound.SetConfig(sc));
	User::LeaveIfError(iDevSound.PreparePlayAlawBuffer());

	iAlawSoundBuffer = new TUint8[iSamplesPerFrame];

	iEmptyBuffer = new TUint8[iSamplesPerFrame];
	memset(iEmptyBuffer, 213, iSamplesPerFrame);

	tEmptyBuffer.Set(iEmptyBuffer, iSamplesPerFrame);

	tAlawBuffer.Set(iAlawSoundBuffer, iSamplesPerFrame);

	iTimer.CreateLocal();

	iSkipCount = 0;
}


void CGameAudio::StopAudioLoop()
{
	this->bQuitAudio = true;
}


void CGameAudio::StartAudioLoop()
{
	do
	{
		if(iAudioSpec.callback((unsigned char*)iAlawSoundBuffer, iSamplesPerFrame))
		{
			SoundUpdate(tAlawBuffer);
		}
		else
		{
			if(iSkipCount >= 3)
				SoundUpdate(tEmptyBuffer);
			else
			{
				iSkipCount++;

				iStatus = KRequestPending;
				iTimer.After(iStatus, 100000);
				User::WaitForRequest(iStatus);
			}
		}

	} while(bQuitAudio == false);
}

void CGameAudio::SoundUpdate(const TPtrC8& sndBuffer)
{
	iSkipCount = 0;

	iStatus = KRequestPending;
	iDevSound.PlayAlawData(iStatus, sndBuffer);

	User::WaitForRequest(iStatus);
}
