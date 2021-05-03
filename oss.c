#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

void playrecord()
{
	int audio;
	char *device = "/dev/dsp1";
	if ( (audio = open(device, O_WRONLY)) < 0 )
	{
 	 	printf("%d", open(device, O_WRONLY));
	}

	int index;
	int length = 640;
	int count = 640;
	char *file = "oss.wav";
	FILE *fout;
	int flength;
	unsigned char buffer[2048];

	if ( NULL == (fout = fopen(file, "r")) )
	{
 		 printf("Could not open device\n");
	}
	flength = fread(buffer, 1, count, fout);

	while ( (flength > 0) && (length > 0) )
	{
 		 if ( -1 == (length = write(audio, buffer, count)))
  		{
   			printf("Write failed\n");
 		}
  		flength = fread(buffer, 1, count, fout);
	}
	fclose(fout);

	if ( -1 == ioctl(audio, SNDCTL_DSP_SYNC, 0) )
	{
  		printf("Sync failed\n");
	}
	close(audio);
	printf("play End\n");
}

void main(){
	int audio;
	char *device = "/dev/dsp1";
	if ( (audio = open(device, O_RDONLY)) < 0 )
	{
 	 	printf("%d", open(device, O_RDONLY));
	}

	int format = AFMT_S16_LE;
	if ( -1 == ioctl(audio, SNDCTL_DSP_SETFMT, &format) )
	{
  		printf("fail ioctl1\n");
	}
	if (AFMT_S16_LE != format)
	{
  		printf("Not supported\n");
	}

	int channels = 2;
	if ( -1 == ioctl(audio, SNDCTL_DSP_CHANNELS, &channels) ) // 0
	{
 		 printf("fail ioctl2\n");
	}
	if (2 != channels)
	{
 		printf("Not supported\n");
	}
	int speed = 16000;
	if ( -1 == ioctl(audio, SNDCTL_DSP_SPEED, &speed) )
	{
 		  printf("fail ioctl3\n");
	}
	if (16000 != speed)
	{
		  printf("Not supported\n");
	}

	int index;
	int length = 640;
	int count = 640;
	char *file = "oss.wav";
	FILE *fin;
	int flength;
	unsigned char buffer[2048];
	if ( NULL == (fin = fopen(file, "w")) )
	{
 		 printf("Could not open device\n");
	}
	for (index = 0; (index < 1000) && (length > 0) && (flength > 0); index++)
	{
 	 	if ( -1 == (length = read(audio, buffer, count)))
 		{
  		  	printf("Read failed\n");
 	 	}
	  	flength = fwrite(buffer, 1, count, fin);
	}
	fclose(fin);
	printf("record ok\n");
	if ( -1 == ioctl(audio, SNDCTL_DSP_SYNC, 0) )
	{
  		printf("Sync failed\n");
	}
	playrecord();
	close(audio);
}
