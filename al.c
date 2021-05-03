#include <stdio.h>
#include <unistd.h>
#include "/usr/include/alsa/asoundlib.h"

int RecordSound(unsigned char *ap_rec_buffer)
{	
	snd_pcm_t *p_capture_handle;
	snd_pcm_hw_params_t *p_hw_params;
	unsigned int freq = 16000;  // 16KHz

	if(snd_pcm_open(&p_capture_handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE,0)<0){
      		perror("Cound not open output audio dev");
      		return 0;
	}	   
	snd_pcm_hw_params_malloc (&p_hw_params);
	snd_pcm_hw_params_any (p_capture_handle, p_hw_params);	// set default value
	snd_pcm_hw_params_set_access (p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
	
	snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
	snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); // 1 is Mono
        snd_pcm_hw_params (p_capture_handle, p_hw_params);
	snd_pcm_hw_params_free (p_hw_params);
	
	printf("10초간 녹음을 합니다.\n"); 
	snd_pcm_prepare (p_capture_handle);
	snd_pcm_readi (p_capture_handle, ap_rec_buffer, 160000);
	printf("녹음이 완료 되었습니다.\n");

	snd_pcm_drop(p_capture_handle);
	snd_pcm_close (p_capture_handle);
	
	return 1;
}

void PlaySound(unsigned char *ap_play_buffer)
{
	snd_pcm_t *p_playback_handle;
	snd_pcm_hw_params_t *hw_params;
	unsigned int freq = 16000;  // 16KHz

	if(snd_pcm_open(&p_playback_handle, "plughw:1,0",SND_PCM_STREAM_PLAYBACK,0)<0){
      		perror("Cound not open output audio dev");
      		return;
	}

	snd_pcm_hw_params_malloc(&hw_params);
	snd_pcm_hw_params_any(p_playback_handle,hw_params);  // set default value
	snd_pcm_hw_params_set_access(p_playback_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED); //SND_PCM_ACCESS_RW_INTERLEAVED : enable snd_pcm_readi/snd_pcm_writei
	snd_pcm_hw_params_set_format(p_playback_handle,hw_params,(snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
	snd_pcm_hw_params_set_rate_near(p_playback_handle,hw_params, &freq,0);
	snd_pcm_hw_params_set_channels(p_playback_handle,hw_params, 1); // 1 is Mono
	snd_pcm_hw_params(p_playback_handle,hw_params);
	snd_pcm_hw_params_free (hw_params);

	printf("10초간 녹음된 소리를 재생합니다.\n");
	snd_pcm_prepare(p_playback_handle);
	snd_pcm_writei(p_playback_handle, ap_play_buffer, 160000);
	usleep(10000000); // wait... 10 sec
	printf("재생을 완료 합니다.\n");
	
	snd_pcm_drop(p_playback_handle);
	snd_pcm_close(p_playback_handle);
}

int main()
{ 
	unsigned char rec_buffer[320000];  // 2bytes * 16000KHz * 10sec
	if(RecordSound(rec_buffer)== 1)	PlaySound(rec_buffer);
		
	return 0;
}
