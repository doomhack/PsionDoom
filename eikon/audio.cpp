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



#include "audio.h"

unsigned char linear2alaw2(short pcm_val);


/*
 * In ER5 we can directly interface with the sound driver. In ER6 the preferred
 * way is to interace with the MediaServer. Apart from not having figured out
 * the MediaServer API, I suspect it will require client-server interaction.
 * Most likely that will be killing for EMame's performance. 
 *
 * I have taken yet another shortcut...
 * I noticed that in ER6 the sound driver's include files are not longer present
 * in \epoc32\include, however, the WINS sound driver binaries (esound.pdd and 
 * esdrv.ldd) are still there. I simply copied the ER5 include files over
 * to ER6, with the one change that in ER6 RDevSound should derive from 
 * RBusLogicalChannel rather than RLogicalChannel. If I then explicitly load 
 * the 2 sound drivers, sound works just fine in ER6/WINS. I haven't got a clue
 * on whether this will work on target.
 */


RThread*		pSoundThread = NULL;

void SDL_OpenAudioOnThread(SDL_AudioSpec *desired)
{
	CGameAudio* pAudio = CGameAudio::NewL(desired->samples, desired);

	if(pAudio)
	{
		pAudio->StartAudioLoop();
	}
}


TInt SDL_OpenAudioThreadFunc(TAny* aParams)
{

    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TRAPD(err, SDL_OpenAudioOnThread( (SDL_AudioSpec*)aParams ););

    delete cleanupStack;


    return KErrNone;
}


int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *unused)
{
	pSoundThread = new (ELeave)RThread();

	pSoundThread->Create(_L("Sound Thread"), SDL_OpenAudioThreadFunc, 16384, KMinHeapSize, 1*1024*1024, (TAny*)desired);
	
	pSoundThread->Resume();

	return 1;
}




/*******************************************
 *
 * CGameAudio
 *
 *******************************************/

const TInt KDefaultBufSize = 0x8000;




CGameAudio::CGameAudio(TInt aSamplesPerFrame, SDL_AudioSpec* audioSpec) : iSamplesPerFrame(aSamplesPerFrame)
{
	iAudioSpec = *audioSpec;
}


CGameAudio* CGameAudio::NewL(TInt aSamplesPerFrame, SDL_AudioSpec* audioSpec)
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
	soundConfig.iAlawBufferSize = iSamplesPerFrame * 2;
	soundConfig.iVolume = EVolumeLoud;
	soundConfig.iVolumeValue = 1;
	User::LeaveIfError(iDevSound.SetConfig(sc));
	User::LeaveIfError(iDevSound.PreparePlayAlawBuffer());

	iAlawSoundBuffer = new TUint8[iSamplesPerFrame];
	iPcmSoundBuffer = new TInt16[iSamplesPerFrame];

	iBufferTimeUs = (iSamplesPerFrame / 8) * 1000;

	BuildAlawTable();
}

void CGameAudio::BuildAlawTable()
{
	for(int i = 0; i < 8192; i++)
	{
		short s = (i - 4096) << 3;
		iAlawLookupTable[i] = linear2alaw2(s);
	}
}


void CGameAudio::StartAudioLoop()
{
	while(true)
	{
		iAudioSpec.callback(iAudioSpec.userdata, (unsigned char*)iPcmSoundBuffer, iSamplesPerFrame * sizeof(short));
		ProcessSoundSamples(iPcmSoundBuffer);
		SoundUpdate();
	}
}


TInt CGameAudio::FirstNoOfSamples()
{
	return iSamplesPerFrame;
}

void CGameAudio::SoundUpdate()
{
	const TPtrC8		ptr((const TUint8*) iAlawSoundBuffer, iSamplesPerFrame);

	iStatus = KRequestPending;
	iDevSound.PlayAlawData(iStatus, ptr);

	User::WaitForRequest(iStatus);
}

#if 0

TInt CGameAudio::ProcessSoundSamples(TInt16* aBuffer)
{	
	TUint8* alawSoundbuffer = iAlawSoundBuffer;

	for (TUint i=0 ; i<iSamplesPerFrame; i+=8)
	{
		//*alawSoundbuffer++ = linear2alaw2(*aBuffer++);

		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		*alawSoundbuffer++ = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
	}

	return iSamplesPerFrame;
}

#else


TInt CGameAudio::ProcessSoundSamples(TInt16* aBuffer)
{	
	unsigned int* alawSoundbuffer = (unsigned int*)iAlawSoundBuffer;

	const unsigned int* aLawBuffEnd = (unsigned int*)&iAlawSoundBuffer[iSamplesPerFrame];

	do
	{

		unsigned int ps1 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		unsigned int ps2 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		unsigned int ps3 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		unsigned int ps4 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];


		*alawSoundbuffer++ =( ps1      )	| 
							( ps2 << 8 )	| 
							( ps3 << 16)	|
							( ps4 << 24);


		ps1 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		ps2 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		ps3 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];
		ps4 = iAlawLookupTable[ (*aBuffer++ >> 3) + 4096];

		*alawSoundbuffer++ =( ps1      )	| 
							( ps2 << 8 )	| 
							( ps3 << 16)	|
							( ps4 << 24);


	} while(alawSoundbuffer < aLawBuffEnd);

	return iSamplesPerFrame;
}

#endif



/******************************************************************************
 *
 * The following sound conversion routines have been copied and pasted
 * from the "spAudio - Audio I/O Library".
 *
 ******************************************************************************/
 
/*
 *
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 * 
 *
 */

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */


static int search(int val, short *table, int size)
{
	int		i;

	for (i = 0; i < size; i++) {
		if (val <= *table++)
			return (i);
	}
	return (size);
}

static short seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF,
			    0x1FF, 0x3FF, 0x7FF, 0xFFF};


unsigned char linear2alaw2(short pcm_val)	/* 2's complement (16-bit range) */
{
   short	 mask;
   short	 seg;
   unsigned char aval;
   
   pcm_val = pcm_val >> 3;

   if (pcm_val >= 0) {
      mask = 0xD5;		/* sign (7th) bit = 1 */
   } else {
      mask = 0x55;		/* sign bit = 0 */
      pcm_val = -pcm_val - 1;
   }
   
   /* Convert the scaled magnitude to segment number. */
   seg = search(pcm_val, seg_aend, 8);
   
   /* Combine the sign, segment, and quantization bits. */
   
   if (seg >= 8)		/* out of range, return maximum value. */
      return (unsigned char) (0x7F ^ mask);
   else {
      aval = (unsigned char) seg << SEG_SHIFT;
      if (seg < 2)
	 aval |= (pcm_val >> 1) & QUANT_MASK;
      else
	 aval |= (pcm_val >> seg) & QUANT_MASK;
      return (aval ^ mask);
   }
}