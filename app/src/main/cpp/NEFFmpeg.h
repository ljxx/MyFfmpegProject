//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_NEFFMPEG_H
#define MYFFMPEGPROJECT_NEFFMPEG_H


#include "JavaCallHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"
#include <cstring>
#include "macro.h"
#include <pthread.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
};

/**
 * 调度室
 */
class NEFFmpeg {

    //友元函数
    friend void *task_stop(void *args);

public:
    NEFFmpeg(JavaCallHelper *javaCallHelper, char *dataSource);
    ~NEFFmpeg();

    void prepare();

    void _prepare();

    void _start();

    void start();
    void setRenderCallback(RenderCallback renderCallback);

    void stop();

private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char *dataSource = 0;
    pthread_t pid_prepare;
    pthread_t pid_start;
    pthread_t pid_stop;
    bool isPlaying;
    AVFormatContext *formatContext = 0;
    RenderCallback renderCallback;

};


#endif //MYFFMPEGPROJECT_NEFFMPEG_H
