/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  System interface for sound.
 *
 *-----------------------------------------------------------------------------
 */

extern "C"
{
	#ifdef HAVE_CONFIG_H
	#include "config.h"
	#endif

	#include <math.h>
	#ifdef HAVE_UNISTD_H
	#include <unistd.h>
	#endif

	#include "z_zone.h"

	#include "m_swap.h"
	#include "i_sound.h"
	#include "m_argv.h"
	#include "m_misc.h"
	#include "w_wad.h"
	#include "lprintf.h"
	#include "s_sound.h"

	#include "doomdef.h"
	#include "doomstat.h"
	#include "doomtype.h"

	#include "d_main.h"

	#include "mmus2mid.h"
	#include "timidity.h"
	#include "m_fixed.h"
}

#include "audio.h"


// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.

// Variables used by Boom from Allegro
// created here to avoid changes to core Boom files
int snd_card = 1;
int mus_card = 1;
int detect_voices = 0; // God knows

static boolean sound_inited = false;
static boolean first_sound_init = true;

// Needed for calling the actual sound output.
static int SAMPLECOUNT	= 1024;
#define MAX_CHANNELS    8

// MWM 2000-01-08: Sample rate in samples/second
//int snd_samplerate=11025;
int snd_samplerate=8000;

// The actual output device.
int audio_fd;


#define MUSIC_BUFFER_SAMPLES 2048
static short music_buffer[MUSIC_BUFFER_SAMPLES * 2];
static unsigned int current_music_buffer = 0;

static unsigned int music_looping = 0;
static unsigned int music_volume = 0;
static unsigned int music_init = 0;

static unsigned int music_sample_counts[2];

static const unsigned int MUSIC_CHANNEL = MAX_CHANNELS;

void flipSongBuffer();


typedef struct
{
	unsigned int	lump_num;
	unsigned char*	data_ptr;
	unsigned int	len;
} resampled_audio_info;

static const unsigned int MAX_RESAMPLED_AUDIO_CACHE = NUMSFX;

static resampled_audio_info* resampled_audio_cache[MAX_RESAMPLED_AUDIO_CACHE] = {NULL};


typedef struct 
{
	// SFX id of the playing sound effect.
	// Used to catch duplicates (like chainsaw).
	int id;
	// The channel step amount...
	unsigned int step;
	// ... and a 0.16 bit remainder of last step.
	unsigned int stepremainder;
	unsigned int samplerate;
	// The channel data pointers, start and end.
	const unsigned char* data;
	const unsigned char* enddata;
	// Time/gametic that the channel started playing,
	//  used to determine oldest, which automatically
	//  has lowest priority.
	// In case number of active sounds exceeds
	//  available channels.
	int starttime;
	// Hardware left and right channel volume lookup.
	int vol;

	//int *leftvol_lookup;
	//int *rightvol_lookup;
} channel_info_t;

channel_info_t channelinfo[MAX_CHANNELS + 1];



//Find a previously stored resampled audio.
unsigned char* findResampledAudioByLumpNum(unsigned int lumpNum, unsigned int& outLen)
{
		for(unsigned int i = 0; i < MAX_RESAMPLED_AUDIO_CACHE; i++)
	{
		resampled_audio_info* info = resampled_audio_cache[i];
		if(info && info->lump_num == lumpNum)
		{
			//Found it.
			outLen = info->len;
			return info->data_ptr;
		}
	}
	
	outLen = 0;
	return NULL;
}


//Resample U8 PCM to new sample rate. Return cached if already converted.
unsigned char* resamplePcm(unsigned int lumpNum, const unsigned char* inPcm, unsigned int inLen, unsigned short outFreq,  unsigned int& outLen)
{
	resampled_audio_info* info = new resampled_audio_info;

	unsigned short inFreq = ((unsigned short*)inPcm)[1];

	unsigned int inSamples = (inLen - 8);
	
	double ratio = (double)outFreq / (double)inFreq;
	double step = ((double)inFreq / (double)outFreq);

	unsigned int outSamples = (unsigned int)ceil(ratio * inSamples);
	
	info->len = (outSamples + 8);
	info->lump_num = lumpNum;
	info->data_ptr = (unsigned char*)Z_Malloc(info->len, PU_STATIC, 0);

	((unsigned short*)info->data_ptr)[1] = outFreq;


	fixed_t fStep = step * FRACUNIT;
	fixed_t fPos = 0;

	for(unsigned int i = 0; i < outSamples; i++)
	{
		unsigned int srcPos = (fPos >> FRACBITS);

		fPos += fStep;

		//unsigned int srcPos = (unsigned int)floor((i * step) + 0.5);

		if(srcPos >= inSamples)
			srcPos = inSamples-1;

		info->data_ptr[i+8] = inPcm[srcPos + 8];
	}

	for(i = 0; i < MAX_RESAMPLED_AUDIO_CACHE; i++)
	{
		if(!resampled_audio_cache[i])
		{
			resampled_audio_cache[i] = info;
			break;
		}
	}

	outLen = info->len;
	return info->data_ptr;
}


/* cph
 * stopchan
 * Stops a sound, unlocks the data
 */

static void stopchan(int i)
{
  if (channelinfo[i].data) /* cph - prevent excess unlocks */
  {
    channelinfo[i].data=NULL;
  }
}

//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//
static int addsfx(int sfxid, int channel, const unsigned char* data, size_t len)
{
	stopchan(channel);
	
	channelinfo[channel].data = data;
	/* Set pointer to end of raw data. */
	channelinfo[channel].enddata = channelinfo[channel].data + len - 1;
	channelinfo[channel].samplerate = (channelinfo[channel].data[3]<<8)+channelinfo[channel].data[2];
	channelinfo[channel].data += 8; /* Skip header */
	
	channelinfo[channel].stepremainder = 0;
	// Should be gametic, I presume.
	channelinfo[channel].starttime = gametic;
	
	// Preserve sound SFX id,
	//  e.g. for avoiding duplicates of chainsaw.
	channelinfo[channel].id = sfxid;
	
	return channel;
}

static void updateSoundParams(int slot, int volume, int seperation, int pitch)
{

#ifdef RANGECHECK
	if ((handle < 0) || (handle >= MAX_CHANNELS))
		I_Error("I_UpdateSoundParams: handle out of range");
#endif
	// Set stepping
	// MWM 2000-12-24: Calculates proportion of channel samplerate
	// to global samplerate for mixing purposes.
	// Patched to shift left *then* divide, to minimize roundoff errors
	// as well as to use SAMPLERATE as defined above, not to assume 11025 Hz

    if (pitched_sounds)
	    channelinfo[slot].step = (((channelinfo[slot].samplerate<<16)/snd_samplerate)-65536);
    else
		channelinfo[slot].step = ((channelinfo[slot].samplerate<<16)/snd_samplerate);

    // Separation, that is, orientation/stereo.
	//  range is: 1 - 256
    //seperation += 1;

    // Per left/right channel.
    //  x^2 seperation,
    //  adjust volume properly.
    //leftvol = volume - ((volume*seperation*seperation) >> 16);
    //seperation = seperation - 257;
    //rightvol= volume - ((volume*seperation*seperation) >> 16);

    if (volume < 0 || volume > 127)
		I_Error("volume out of bounds");


    // Sanity check, clamp volume.
    //if (rightvol < 0 || rightvol > 127)
	//	I_Error("rightvol out of bounds");

    //if (leftvol < 0 || leftvol > 127)
	//	I_Error("leftvol out of bounds");

    // Get the proper lookup table piece
    // for this volume level???
	//channelinfo[slot].leftvol_lookup = &vol_lookup[leftvol*256];
	//channelinfo[slot].rightvol_lookup = &vol_lookup[rightvol*256];

	channelinfo[slot].vol = volume;
}

void I_UpdateSoundParams(int handle, int volume, int seperation, int pitch)
{
  updateSoundParams(handle, volume, seperation, pitch);
}

//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels(void)
{
  // Okay, reset internal mixing channels to zero.
  for (unsigned int i=0; i<MAX_CHANNELS + 1; i++)
  {
	  memset(&channelinfo[i],0,sizeof(channel_info_t));
  }
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}



//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int I_StartSound(int id, int channel, int vol, int sep, int pitch, int priority)
{
	const unsigned char* data = NULL;
	int lump;
	unsigned int len = 0;

	if ((channel < 0) || (channel >= MAX_CHANNELS))
#ifdef RANGECHECK
		I_Error("I_StartSound: handle out of range");
#else
		return -1;
#endif

	lump = S_sfx[id].lumpnum;

	data = findResampledAudioByLumpNum(lump, len);

	if(!data)
	{
		// We will handle the new SFX.
		// Set pointer to raw data.
		unsigned int lumpLen = W_LumpLength(lump);

		// e6y: Crash with zero-length sounds.
		// Example wad: dakills (http://www.doomworld.com/idgames/index.php?id=2803)
		// The entries DSBSPWLK, DSBSPACT, DSSWTCHN and DSSWTCHX are all zero-length sounds
		if (lumpLen<=8) 
			return -1;

		/* Find padded length */
		//len -= 8;
		// do the lump caching outside the SDL_LockAudio/SDL_UnlockAudio pair
		// use locking which makes sure the sound data is in a malloced area and
		// not in a memory mapped one
		const unsigned char* wadData = (const unsigned char*)W_CacheLumpNum(lump);

		data = (const unsigned char*)resamplePcm(lump, wadData, lumpLen, 8000, len);
	}

	// Returns a handle (not used).
	addsfx(id, channel, data, len-8);
	updateSoundParams(channel, vol, sep, pitch);

	return channel;
}



void I_StopSound (int handle)
{
#ifdef RANGECHECK
	if ((handle < 0) || (handle >= MAX_CHANNELS))
		I_Error("I_StopSound: handle out of range");
#endif

	stopchan(handle);
}


boolean I_SoundIsPlaying(int handle)
{
#ifdef RANGECHECK
	if ((handle < 0) || (handle >= MAX_CHANNELS))
		I_Error("I_SoundIsPlaying: handle out of range");
#endif
	return channelinfo[handle].data != NULL;
}


boolean I_AnySoundStillPlaying(void)
{
	boolean result = false;
	int i;
	
	for (i=0; i<MAX_CHANNELS; i++)
		result |= channelinfo[i].data != NULL;

	return result;
}


//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the given
//  mixing buffer, and clamping it to the allowed
//  range.
//
// This function currently supports only 16bit.
//

static void I_UpdateSound(void *unused, unsigned char *stream, int len)
{
	// Mix current sound data.
	register unsigned char sample;
	register int    dl;

	// Pointers in audio stream, left, right, end.
	signed short*   leftout;
	signed short*   leftend;
	// Step in stream, left and right, thus two.
	int       step;

	// Mixing channel index.
	int       chan;

    // Left and right channel
    //  are in audio stream, alternating.
    leftout = (signed short *)stream;

    // Determine end, for left channel only
    //  (right channel is implicit).
    leftend = leftout + (len >> 1);

    // Mix sounds into the mixing buffer.
    // Loop over step*SAMPLECOUNT,
    //  that is 512 values for two channels.
    while (leftout != leftend)
    {
		// Reset left/right value.
		dl = 0;
		//dr = 0;
		//dl = *leftout;
		//dr = *rightout;

		// Love thy L2 chache - made this a loop.
		// Now more channels could be set at compile time
		//  as well. Thus loop those  channels.
		for ( chan = 0; chan < (MAX_CHANNELS); chan++ )
		{
			// Check channel, if active.
			if (channelinfo[chan].data)
			{
				// Get the raw data from the channel.
				// no filtering
				sample = *channelinfo[chan].data;
				// linear filtering
				//sample = (((unsigned int)channelinfo[chan].data[0] * (0x10000 - channelinfo[chan].stepremainder))
				//	+ ((unsigned int)channelinfo[chan].data[1] * (channelinfo[chan].stepremainder))) >> 16;

				//dl += channelinfo[chan].vol_lookup[sample];
				int volume = channelinfo[chan].vol;

				int scaledSample = ((sample - 127) * volume);
				scaledSample += (scaledSample >> 1);
				
				dl += scaledSample;
				

				// Increment index ???
				channelinfo[chan].stepremainder += channelinfo[chan].step;
				// MSB is next sample???
				channelinfo[chan].data += channelinfo[chan].stepremainder >> 16;
				// Limit to LSB???
				channelinfo[chan].stepremainder &= 0xffff;

				// Check whether we are done.
				if (channelinfo[chan].data >= channelinfo[chan].enddata)
				{
					stopchan(chan);
				}					
			}
		}

		// Check channel, if active.
		if (channelinfo[MUSIC_CHANNEL].data)
		{
			// Get the raw data from the channel.
			short sample2 = *((short*)channelinfo[MUSIC_CHANNEL].data);

			dl += sample2;


			// Increment index ???
			channelinfo[MUSIC_CHANNEL].stepremainder += (channelinfo[MUSIC_CHANNEL].step);
			// MSB is next sample???
			channelinfo[MUSIC_CHANNEL].data += (channelinfo[MUSIC_CHANNEL].stepremainder >> 15);
			// Limit to LSB???
			channelinfo[MUSIC_CHANNEL].stepremainder &= 0xffff;

			// Check whether we are done.
			if (channelinfo[MUSIC_CHANNEL].data >= channelinfo[MUSIC_CHANNEL].enddata)
			{
				stopchan(MUSIC_CHANNEL);

				flipSongBuffer();
			}
		}


		if (dl > SHRT_MAX)
			*leftout = SHRT_MAX;
		else if (dl < SHRT_MIN)
			*leftout = SHRT_MIN;
		else
			*leftout = (signed short)dl;

		leftout++;
	}
}

void I_ShutdownSound(void)
{
	if (sound_inited) 
	{
		lprintf(LO_INFO, "I_ShutdownSound: ");
		lprintf(LO_INFO, "\n");
		sound_inited = false;
	}
}

//static SDL_AudioSpec audio;

void I_InitSound(void)
{
	I_SetChannels();

	SDL_AudioSpec* audio = new SDL_AudioSpec;

	// Secure and configure sound device first.
	lprintf(LO_INFO,"I_InitSound: ");

	// Open the audio device
	audio->freq = snd_samplerate;

	audio->format = 0;

	audio->channels = 1;
	audio->samples = SAMPLECOUNT;
	audio->callback = I_UpdateSound;
	
	if ( SDL_OpenAudio(audio, NULL) < 0 )
	{
		lprintf(LO_INFO,"couldn't open audio with desired format\n");
		return;
	}
	
	SAMPLECOUNT = audio->samples;
	lprintf(LO_INFO," configured audio device with %d samples/slice\n", SAMPLECOUNT);


	if (first_sound_init)
	{
		atexit(I_ShutdownSound);
		first_sound_init = false;
	}

	if (!nomusicparm)
		I_InitMusic();

	// Finished initialization.
	lprintf(LO_INFO,"I_InitSound: sound module ready\n");
}




//
// MUSIC API.

MidIStream* midiStream = NULL;
MidSong* midiSong = NULL;



void I_ShutdownMusic(void)
{

}

void I_InitMusic(void)
{
#ifdef __WINS__
	music_init  = (mid_init ("C:\\Doom\\dgguspat\\timidity.cfg") >= 0);
#else
	music_init = (mid_init ("D:\\Doom\\dgguspat\\timidity.cfg") >= 0);
#endif

	if(!music_init)
		lprintf(LO_INFO,"I_InitMusic: mid_init failed.\n");
	else
		lprintf(LO_INFO,"I_InitMusic: mid_init done.\n");
}


void playSongBuffer(unsigned int buffNum)
{
	if(!music_init)
		return;

	stopchan(MUSIC_CHANNEL);

	//This stops buffer underruns from killing the song.
	if(!music_sample_counts[buffNum])
	{
		music_sample_counts[buffNum] = MUSIC_BUFFER_SAMPLES;
		//return;
	}
		

	unsigned int buffOffset = (buffNum * MUSIC_BUFFER_SAMPLES);

	channelinfo[MUSIC_CHANNEL].samplerate = snd_samplerate;
	
	channelinfo[MUSIC_CHANNEL].stepremainder = 0;
	channelinfo[MUSIC_CHANNEL].starttime = gametic;
	
	channelinfo[MUSIC_CHANNEL].id = 0;

	channelinfo[MUSIC_CHANNEL].step = ((snd_samplerate<<16)/snd_samplerate);

	channelinfo[MUSIC_CHANNEL].vol = 0;

	/* Set pointer to end of raw data. */
	channelinfo[MUSIC_CHANNEL].enddata = (unsigned char*)(music_buffer + buffOffset + (music_sample_counts[buffNum] - 1));

	channelinfo[MUSIC_CHANNEL].data = (unsigned char*)(&music_buffer[buffOffset]);
}

void flipSongBuffer()
{	
	if(!music_init)
		return;

	music_sample_counts[current_music_buffer] = 0;

	current_music_buffer = 1 - current_music_buffer;

	playSongBuffer(current_music_buffer);
}

void I_UpdateMusic()
{
	if(!midiSong || !music_init)
		return;

	unsigned int next_music_buffer = 1 - current_music_buffer;

	if(music_sample_counts[next_music_buffer] == 0)
	{
		unsigned int buffOffset = (next_music_buffer * MUSIC_BUFFER_SAMPLES);


		music_sample_counts[next_music_buffer] = (mid_song_read_wave (midiSong, (signed char*)(&music_buffer[buffOffset]), MUSIC_BUFFER_SAMPLES * 2) >> 1);

		if( (music_sample_counts[next_music_buffer] == 0) && music_looping)
		{
			//Back to start. (Looping)
			mid_song_start(midiSong);

			music_sample_counts[next_music_buffer] = (mid_song_read_wave (midiSong, (signed char*)(&music_buffer[buffOffset]), MUSIC_BUFFER_SAMPLES * 2) >> 1);
		}
	}
}

void I_PlaySong(int handle, int looping)
{
	if(!midiSong || !music_init)
		return;

	music_looping = looping;

	current_music_buffer = 0;

	music_sample_counts[0] = (mid_song_read_wave (midiSong, (signed char*)(&music_buffer[0]), MUSIC_BUFFER_SAMPLES * 2) >> 1);
	music_sample_counts[1] = (mid_song_read_wave (midiSong, (signed char*)(&music_buffer[MUSIC_BUFFER_SAMPLES]), MUSIC_BUFFER_SAMPLES * 2) >> 1);

	playSongBuffer(0);
}

//extern int mus_pause_opt; // From m_misc.c

void I_PauseSong (int handle)
{
	stopchan(MUSIC_CHANNEL);
}

void I_ResumeSong (int handle)
{
	playSongBuffer(current_music_buffer);
}

void I_StopSong(int handle)
{
	if(midiSong)
	{
		mid_song_seek (midiSong, 0);
	}

	stopchan(MUSIC_CHANNEL);
}

void I_UnRegisterSong(int handle)
{
	I_StopSong(0);
	
	if(midiSong)
		mid_song_free(midiSong);

	if(midiStream)
		mid_istream_close(midiStream);

	midiSong = NULL;
	midiStream = NULL;
}

int I_RegisterSong(const void *data, size_t len)
{
	if(!music_init)
		return 0;

	MIDI *mididata = NULL;


	if ( len < 32 )
		return 0; // the data should at least as big as the MUS header

	/* Convert MUS chunk to MIDI? */
	if ( memcmp(data, "MUS", 3) == 0 )
	{
		UBYTE *mid = NULL;
		int midlen = 0;

		mididata = (MIDI*)malloc(sizeof(MIDI));
		mmus2mid((const unsigned char*)data, mididata, 89, 0);
		MIDIToMidi(mididata,&mid,&midlen);
		
		midiStream = mid_istream_open_mem (mid, midlen);

		if(midiStream)
		{
			MidSongOptions options;
			
			options.rate = snd_samplerate;
			options.format = MID_AUDIO_S16LSB;
			options.channels = 1;
			options.buffer_size = 65535;

			midiSong = mid_song_load (midiStream, &options);
			
			if(midiSong)
			{
				mid_song_set_volume (midiSong, music_volume);

				mid_song_start (midiSong);
			}
			else
			{
				lprintf(LO_INFO,"I_RegisterSong: mid_song_load returned NULL\n");
			}
		}
		else
		{
			lprintf(LO_INFO,"I_RegisterSong: mid_istream_open_mem returned NULL\n");
		}

		free(mid);
		free_mididata(mididata);
		free(mididata);
	}

	return 0;
}

// cournia - try to load a music file into SDL_Mixer
//           returns true if could not load the file
int I_RegisterMusic( const char* filename, musicinfo_t *song )
{
	if (!filename)
		return 1;
	
	if (!song)
		return 1;
	
	song->data = 0;
	song->handle = 0;
	song->lumpnum = 0;

	return 0;

}

void I_SetMusicVolume(int volume)
{
	music_volume = (volume * 8);

	if(midiSong)
	{
		mid_song_set_volume (midiSong, music_volume);
	}

}

