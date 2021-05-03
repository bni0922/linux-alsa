#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "/usr/include/alsa/asoundlib.h"
 
// 쓰레드 동작시 실행될 함수
void *firstThreadRun(unsigned char *ap_rec_buffer)
{
    while(1)
    {
        printf("start First Thread\n");	
        snd_pcm_t *p_capture_handle;
        snd_pcm_hw_params_t *p_hw_params;
        unsigned int freq = 16000;  // 16KHz
        if(snd_pcm_open(&p_capture_handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE,0)<0){
      		perror("Cound not open output audio dev");
      		return;
        }	   
        snd_pcm_hw_params_malloc (&p_hw_params);
        snd_pcm_hw_params_any (p_capture_handle, p_hw_params);	// set default value
        snd_pcm_hw_params_set_access (p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
	
        snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
        snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); // 1 is Mono
        snd_pcm_hw_params (p_capture_handle, p_hw_params);
        snd_pcm_hw_params_free (p_hw_params);
	
        printf("녹음을 합니다.\n"); 
        snd_pcm_prepare (p_capture_handle);
        snd_pcm_readi (p_capture_handle, ap_rec_buffer, 1600); 
       // printf("녹음이 완료 되었습니다.\n");

        snd_pcm_drop(p_capture_handle);
        snd_pcm_close (p_capture_handle);
    }
}
 
void *secondThreadRun(unsigned char *ap_play_buffer)
{
    while(1)
    {
        printf("start Second Thread\n");
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

       printf("녹음된 소리를 재생합니다.\n");
       snd_pcm_prepare(p_playback_handle);
       snd_pcm_writei(p_playback_handle, ap_play_buffer, 1600);
       usleep(10000000);
      // printf("재생을 완료 합니다.\n");
	
       snd_pcm_drop(p_playback_handle);
       snd_pcm_close(p_playback_handle);
    }
}

int main()
{
    pthread_t firstThread, seconThread;
    int threadErr;
    unsigned char rec_buffer[3200];
    
    // 쓰레드를 만들고 쓰레드 함수 실행
    if(threadErr = pthread_create(&firstThread,NULL,firstThreadRun,rec_buffer))
    {
        // 에러시 에러 출력
        printf("Thread Err = %d",threadErr);
    }



    if(threadErr = pthread_create(&seconThread,NULL,secondThreadRun,rec_buffer))
    {
        // 에러시 에러 출력
        printf("Thread Err = %d",threadErr);
    }
    
    while(1);
}
