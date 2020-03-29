//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_AUDIOCHANNEL_H
#define MYFFMPEGPROJECT_AUDIOCHANNEL_H


#include "BaseChannel.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "macro.h"

extern "C" {
#include <libswresample/swresample.h>
};

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, AVCodecContext *codecContext, AVRational time_base);
    ~AudioChannel();

    void start();

    void stop();

    void audio_decode();

    void audio_play();

    int getPCM();

    uint8_t *out_buffers = 0;
    int out_channels;
    int out_sampleSize;
    int out_sampleRate;
    int out_buffers_size;

private:
    SwrContext *swrContext = 0;
    pthread_t pid_audio_decode;
    pthread_t pid_audio_play;
    //引擎object
    SLObjectItf engineObject = 0;
    //引擎对接口
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器的接口
    SLPlayItf bqPlayerPlay = 0;
    //播放器队列接口
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = 0;
};


#endif //MYFFMPEGPROJECT_AUDIOCHANNEL_H
