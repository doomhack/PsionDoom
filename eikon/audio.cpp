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
 */


RThread*		pSoundThread = NULL;

void OpenAudioOnThread(AudioSpec *desired)
{
	CGameAudio* pAudio = CGameAudio::NewL(desired->samples, desired);

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




/*******************************************
 *
 * CGameAudio
 *
 *******************************************/

CGameAudio::CGameAudio(TInt aSamplesPerFrame, AudioSpec* audioSpec) : iSamplesPerFrame(aSamplesPerFrame)
{
	iAudioSpec = *audioSpec;
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
	soundConfig.iVolume = EVolumeLoud;
	soundConfig.iVolumeValue = 1;
	User::LeaveIfError(iDevSound.SetConfig(sc));
	User::LeaveIfError(iDevSound.PreparePlayAlawBuffer());

	iAlawSoundBuffer = new TUint8[iSamplesPerFrame];
	iPcmSoundBuffer = new TInt16[iSamplesPerFrame];

	BuildAlawTable();
}

void CGameAudio::BuildAlawTable()
{
	for(int i = 0; i < 8192; i++)
	{
		short s = (short)((i - 4096) << 3);
		iAlawLookupTable[i] = linear2alaw2(s);
	}
}


void CGameAudio::StartAudioLoop()
{
	do
	{
		iAudioSpec.callback((unsigned char*)iPcmSoundBuffer, iSamplesPerFrame * sizeof(short));
		
		ProcessSoundSamples(iPcmSoundBuffer);
		SoundUpdate();
	} while(true);
}

void CGameAudio::SoundUpdate()
{
	const TPtrC8		ptr((const TUint8*) iAlawSoundBuffer, iSamplesPerFrame);

	iStatus = KRequestPending;
	iDevSound.PlayAlawData(iStatus, ptr);

	User::WaitForRequest(iStatus);

}

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


static short search(int val, short *table, short size)
{
	short i;

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
   
   pcm_val = (short)(pcm_val >> 3);

   if (pcm_val >= 0) {
      mask = 0xD5;		/* sign (7th) bit = 1 */
   } else {
      mask = 0x55;		/* sign bit = 0 */
      pcm_val = (short)(-pcm_val - 1);
   }
   
   /* Convert the scaled magnitude to segment number. */
   seg = search(pcm_val, seg_aend, 8);
   
   /* Combine the sign, segment, and quantization bits. */
   
   if (seg >= 8)		/* out of range, return maximum value. */
      return (unsigned char) (0x7F ^ mask);
   else {
      aval = (unsigned char) (seg << SEG_SHIFT);
      if (seg < 2)
	 aval |= (pcm_val >> 1) & QUANT_MASK;
      else
	 aval |= (pcm_val >> seg) & QUANT_MASK;
      return (unsigned char)(aval ^ mask);
   }
}