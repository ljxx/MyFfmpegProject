//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_AUDIOCHANNEL_H
#define MYFFMPEGPROJECT_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, AVCodecContext *codecContext);
    ~AudioChannel();

    void start();

    void stop();
};


#endif //MYFFMPEGPROJECT_AUDIOCHANNEL_H
