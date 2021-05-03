#ifndef __VRSPLAYBACK_H_
#define __VRSPLAYBACK_H_

#define FRAME_SIZE_PLAY			1024


extern int VRSPlay_SND_PCM_SET();
extern int VRSPlay_SND_PCM_OPEN();
extern int VRSPlay_SND_PCM_PUT_BUFFER(short *buffer, int len);
extern int VRSPlay_SND_PCM_CLOSE();
extern int VRSPlay_SND_PCM_PREPARE();
extern int VRSPlay_SND_PCM_STATE();
extern int VRSPlay_SND_PCM_DRAIN();
extern int VRSPlay_SND_PCM_PAUSE();
extern int VRSPlay_SND_PCM_RESUME();
extern int VRSPlay_SND_PCM_DROP();

extern void playPcmFile(char *fname);
extern void playUlawFile(char *fname);
#endif
