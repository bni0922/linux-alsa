
/**********************************************************
  Voice Recording Program for ASR based on ALSA libs.

  author : djshin@vrsoft.com
  date   : 2015-08-08

  * get libasound 
      sudo apt-get install libasound2-dev
  * how to compile
      gcc -o taret source.c -lasound

  revision history
  2015-08-09 :   
  2018-11-08 : revision for x1000
  2019-11-09 : audio record & playback

**********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>

#include "VRSPlay.h"

#define SND_DEVICE_NAME 	"plughw:0,0"
#define MONO			1
#define STEREO			2
#define SAMPLING_RATE		16000

#define CHANNEL_NUM		MONO

#define FRAME_SIZE		SAMPLE_PER_FRAME * CHANNEL_NUM 



//global variables
int 			g_playback_buffer_frames;
unsigned int 		g_playback_rate;
snd_pcm_t 		*g_playback_handle;
snd_pcm_format_t	g_playback_format;


int VRSPlay_SND_PCM_SET()
{
   g_playback_buffer_frames = FRAME_SIZE_PLAY;
   g_playback_rate = SAMPLING_RATE;
   g_playback_format = SND_PCM_FORMAT_S16_LE;
}


int VRSPlay_SND_PCM_OPEN()
{
	int err;
	snd_pcm_hw_params_t 	*hw_params;

	int periods = 2;
	snd_pcm_uframes_t periodsize = 2048;  /* Periodsize (bytes) */
  
	VRSPlay_SND_PCM_SET();

	err = snd_pcm_open(&g_playback_handle, SND_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
	if(err<0)
	{
		fprintf(stderr, "cannot open audio device %s (%s)\n",
              SND_DEVICE_NAME,
              snd_strerror(err));
		exit(1);
    }
	fprintf(stdout, "audio device named %s is opened\n", 
            SND_DEVICE_NAME);


	err = snd_pcm_hw_params_malloc(&hw_params);
	if(err<0)
	{
		fprintf(stderr, "cannot allocate hw params (%s)\n",
              snd_strerror(err));
		exit(1);
    }
	fprintf(stdout, "hw_params allocated\n");

	err = snd_pcm_hw_params_any(g_playback_handle, hw_params);
	if(err<0)
	{
		fprintf(stderr, "cannot initialize hw params (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params initialized\n");

	err = snd_pcm_hw_params_set_access(g_playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(err<0)
	{
		fprintf(stderr, "cannot set access type (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params access setted\n");

	err = snd_pcm_hw_params_set_format(g_playback_handle, hw_params, g_playback_format);
	if(err<0)
	{
		fprintf(stderr, "cannot set format (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params format setted\n");

	err = snd_pcm_hw_params_set_rate_near(g_playback_handle, hw_params, 16000, 0);
	if(err<0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params rate setted\n");

	err = snd_pcm_hw_params_set_channels(g_playback_handle, hw_params, 1);
	if(err<0)
	{
		fprintf(stderr, "cannot set channel count (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params channels setted\n");


	err = snd_pcm_hw_params_set_buffer_size(g_playback_handle, hw_params, g_playback_buffer_frames);
	if(err<0)
	{
		fprintf(stderr, "cannot set buffer size\n");
		exit(1);
	}
	fprintf(stdout, "hw_params buffer size setted\n");

#if 0
	err = snd_pcm_hw_params_set_periods(g_playback_handle, hw_params, periods, 0);
	if(err<0)
	{
		fprintf(stderr, "cannot set periods\n");
		exit(1);
	}
	fprintf(stdout, "hw_params periods setted\n");

	err = snd_pcm_hw_params_set_buffer_size(g_playback_handle, hw_params, (periodsize * periods)>>2);
	if(err<0)
	{
		fprintf(stderr, "cannot set buffer size\n");
		exit(1);
	}
	fprintf(stdout, "hw_params buffer size setted\n");


#endif


	//sleep(5);

	err = snd_pcm_hw_params(g_playback_handle, hw_params);
	if(err<0)
	{
		fprintf(stderr, "cannot set parameters (%s)\n",
              snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "hw_params setted\n");

	snd_pcm_hw_params_free(hw_params);

	fprintf(stdout, "hw_params freed\n");

	return 0;
	err = snd_pcm_prepare(g_playback_handle);
	if(err<0)
	{
		fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
              snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "playback audio interface prepared\n");

	return 0;
}



int VRSPlay_SND_PCM_PUT_BUFFER(short *buffer, int len)
{
   int err;
   //err = snd_pcm_writei(g_playback_handle, buffer, g_playback_buffer_frames);
   err = snd_pcm_writei(g_playback_handle, buffer, len);
}

int VRSPlay_SND_PCM_PREPARE()
{
   int err;

   err = snd_pcm_prepare(g_playback_handle);
   if(err<0)
   {
      fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
              snd_strerror(err));
      exit(1);
   }
}
   
int VRSPlay_SND_PCM_CLOSE()
{
   snd_pcm_close(g_playback_handle);
   fprintf(stdout, "audio interface closed\n");
}

int VRSPlay_SND_PCM_STATE()
{
	if (snd_pcm_state(g_playback_handle) == SND_PCM_STATE_RUNNING)
	{
		return 1;
	}
	return 0;
}

int VRSPlay_SND_PCM_DRAIN()
{
	snd_pcm_nonblock(g_playback_handle, 0);
	snd_pcm_drain(g_playback_handle);
	snd_pcm_nonblock(g_playback_handle, 0);
}


int VRSPlay_SND_PCM_DROP()
{
	snd_pcm_drop(g_playback_handle);
}

int VRSPlay_SND_PCM_PAUSE()
{
   int err;
   err = snd_pcm_pause(g_playback_handle, 1);
   if(err)
   {
      printf("################ ALSA PAUSE ERROR % [%s]\n", snd_strerror(err));
   }
   return 0;
}

int VRSPlay_SND_PCM_RESUME()
{
   int err;
   err = snd_pcm_pause(g_playback_handle, 0);
   if(err)
   {
      printf("################ ALSA RESUME ERROR % [%s]\n", snd_strerror(err));
   }
   return 0;
}


int ulaw2linear(unsigned char ulawbyte)
{
	static int exp_lut[8] = {0,132,396,924,1980,4092,8316,16764};
	int sign, exponent, mantissa, sample;

	ulawbyte = ~ulawbyte;
	sign = (ulawbyte & 0x80);
	exponent = (ulawbyte >> 4) & 0x07;
	mantissa = ulawbyte & 0x0F;
	sample = exp_lut[exponent] + (mantissa << (exponent + 3));
	if (sign != 0) sample = -sample;

	return(sample);
}



void playUlawFile(char *fname)
{
	FILE *fp;
	unsigned long flen;
	unsigned char ulaw_buf[FRAME_SIZE_PLAY];
	short pcm_buf[FRAME_SIZE_PLAY];
	int nflame;
	int remainder;
	int i, j;
	int k;

	system("/vrsoft/gpio_set.sh 1"); // amp on

	VRSPlay_SND_PCM_PREPARE();
	fp = fopen(fname, "rb");
	if(fp!=NULL)
	{
		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);

		// Skip ulaw header
		fseek(fp, 24L, SEEK_SET);
		flen -= 24;
		printf("file name is %s, size=%d\n", fname, flen);

		nflame = flen / FRAME_SIZE_PLAY;
		remainder = flen - (nflame * FRAME_SIZE_PLAY);

		for(i=0;i<nflame;i++)
		{
			fread(ulaw_buf, sizeof(unsigned char), FRAME_SIZE_PLAY, fp);
			for(j=0; j<FRAME_SIZE_PLAY; j++)
			{
				pcm_buf[j] = (short)ulaw2linear(ulaw_buf[j]);
			}
			VRSPlay_SND_PCM_PUT_BUFFER(pcm_buf, FRAME_SIZE_PLAY);
		}
		if(remainder!=0)
		{
			fread(ulaw_buf, sizeof(unsigned char), remainder, fp);
			for(j=0; j<remainder; j++)
			{
				pcm_buf[j] = (short)ulaw2linear(ulaw_buf[j]);
			}
			for(j=remainder; j<FRAME_SIZE_PLAY; j++)
			{
				pcm_buf[j] = 0;
			}

			VRSPlay_SND_PCM_PUT_BUFFER(pcm_buf, FRAME_SIZE_PLAY);
		}

		printf ("pcm_state : %d\n", VRSPlay_SND_PCM_STATE());

		while(1)
		{
			if(VRSPlay_SND_PCM_STATE()!=1)
			{
				VRSPlay_SND_PCM_PREPARE();
				printf("play out all data\n");
				break;
			}
		}
		printf ("pcm_state : %d\n", VRSPlay_SND_PCM_STATE());

		fclose(fp);
	}

	system("/vrsoft/gpio_set.sh 0"); // amp off
}


void playPcmFile(char *pcm_file)
{
	FILE *fp;
	unsigned long flen;
	unsigned char ulaw_buf[FRAME_SIZE_PLAY];
	short pcm_buf_8k[FRAME_SIZE_PLAY];
	short pcm_buf_16k[FRAME_SIZE_PLAY];
	int nflame, nSample;
	int remainder;
	int i, j;
	int k;
	int frame_size_half;

	system("/vrsoft/gpio_set.sh 1"); // amp on

	VRSPlay_SND_PCM_PREPARE();
	fp = fopen(pcm_file, "rb");
	if(fp!=NULL)
	{
		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		printf("file name is %s, size=%d\n", pcm_file, flen);
		nSample = flen / 2;

		frame_size_half = (FRAME_SIZE_PLAY/2);

		nflame = nSample / frame_size_half;

		remainder = nSample - (nflame * frame_size_half);

		for(i=0;i<nflame;i++)
		{
			fread(pcm_buf_8k, sizeof(short), frame_size_half, fp);
			for(j=0; j<frame_size_half; j++)
			{
				pcm_buf_16k[2*j] = pcm_buf_8k[j];
				pcm_buf_16k[2*j + 1] = pcm_buf_8k[j];
			}
			VRSPlay_SND_PCM_PUT_BUFFER(pcm_buf_16k, FRAME_SIZE_PLAY);
		}
		if(remainder!=0)
		{
			fread(pcm_buf_8k, sizeof(short), remainder, fp);
			for(j=0; j<remainder; j++)
			{
				pcm_buf_16k[2*j] = pcm_buf_8k[j];
				pcm_buf_16k[2*j+1] = pcm_buf_8k[j];
			}

			for(j=remainder*2; j<FRAME_SIZE_PLAY; j++)
			{
				pcm_buf_16k[j] = 0;
			}

			VRSPlay_SND_PCM_PUT_BUFFER(pcm_buf_16k, FRAME_SIZE_PLAY);
		}

		printf ("pcm_state : %d\n", VRSPlay_SND_PCM_STATE());

		while(1)
		{
			if(VRSPlay_SND_PCM_STATE()!=1)
			{
				VRSPlay_SND_PCM_PREPARE();
				printf("play out all data\n");
				break;
			}
		}
		printf ("pcm_state : %d\n", VRSPlay_SND_PCM_STATE());

		fclose(fp);
	}

	system("/vrsoft/gpio_set.sh 0"); // amp off
}


