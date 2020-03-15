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
public:
    NEFFmpeg(JavaCallHelper *javaCallHelper, char *dataSource);
    ~NEFFmpeg();

    void prepare();

    void _prepare();

    void start();

private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char *dataSource = 0;
    pthread_t pid_prepare;

};


#endif //MYFFMPEGPROJECT_NEFFMPEG_H
