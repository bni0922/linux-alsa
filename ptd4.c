#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "/usr/include/alsa/asoundlib.h"

pthread_cond_t cond;
pthread_mutex_t mutex;
unsigned char buffer[6400];
unsigned int freq = 16000;  // 16KHz
snd_pcm_t *p_capture_handle;
snd_pcm_t *p_playback_handle;

void snd_pcm_Close(){
     snd_pcm_drop(p_capture_handle);
     snd_pcm_close (p_capture_handle);
     snd_pcm_drop(p_playback_handle);
     snd_pcm_close(p_playback_handle);
}

void snd_pcm_RecordSet(){
     snd_pcm_hw_params_t *p_hw_params;
     if(snd_pcm_open(&p_capture_handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE,0)<0){
      		perror("Cound not open output audio dev");
        }	   
     snd_pcm_hw_params_malloc (&p_hw_params);
     snd_pcm_hw_params_any (p_capture_handle, p_hw_params);//set default value
     snd_pcm_hw_params_set_access(p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
     snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
       
     snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
     snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); // 1 is Mono
     snd_pcm_hw_params (p_capture_handle, p_hw_params);
     snd_pcm_hw_params_free (p_hw_params);	
}
void snd_pcm_PlaySet(){
    snd_pcm_hw_params_t *hw_params;
        if(snd_pcm_open(&p_playback_handle, "plughw:1,0",SND_PCM_STREAM_PLAYBACK,0)<0){
      	perror("Cound not open output audio dev");
        }

       snd_pcm_hw_params_malloc(&hw_params);
       snd_pcm_hw_params_any(p_playback_handle,hw_params); //set default value
       snd_pcm_hw_params_set_access(p_playback_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED); 
//SND_PCM_ACCESS_RW_INTERLEAVED : enable snd_pcm_readi/snd_pcm_writei
       snd_pcm_hw_params_set_format(p_playback_handle,hw_params,(snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
       snd_pcm_hw_params_set_rate_near(p_playback_handle,hw_params, &freq,0);
       snd_pcm_hw_params_set_channels(p_playback_handle,hw_params, 1);//1=Mono
       snd_pcm_hw_params(p_playback_handle,hw_params);
       snd_pcm_hw_params_free (hw_params);
}

// ????????? ????????? ????????? ??????
void *firstThreadRun()
{
    while(1)
    {
        printf("????????? ?????????.\n"); 
        snd_pcm_prepare (p_capture_handle);
        snd_pcm_readi (p_capture_handle, buffer, 3200);
       // printf("????????? ?????? ???????????????.\n");
        pthread_cond_wait(&cond, &mutex);
    }
}
 
void *secondThreadRun()
{
    while(1)
    {
       printf("????????? ????????? ???????????????.\n");
       snd_pcm_prepare(p_playback_handle);
       snd_pcm_writei(p_playback_handle, buffer, 3200);
       snd_pcm_drain(p_playback_handle);
      // printf("????????? ?????? ?????????.\n");
       pthread_cond_signal(&cond);
    }
}

int main()
{
    pthread_t firstThread, seconThread;
    int threadErr;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    snd_pcm_RecordSet();
    snd_pcm_PlaySet();

    // ???????????? ????????? ????????? ?????? ??????
    if(threadErr = pthread_create(&firstThread,NULL,firstThreadRun, NULL))
    {
        // ????????? ?????? ??????
        printf("Thread Err = %d",threadErr);
    }


    if(threadErr = pthread_create(&seconThread,NULL,secondThreadRun , NULL))
    {
        // ????????? ?????? ??????
        printf("Thread Err = %d",threadErr);
    }
    
    while(1);
}
