//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_BASECHANNEL_H
#define MYFFMPEGPROJECT_BASECHANNEL_H

#include "safe_queue.h"
#include "JavaCallHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
};

/**
 * BaseChannel是AudioChannel和VideoChannel的父类
 */
class BaseChannel {
public:
    BaseChannel(int id, AVCodecContext *codecContext, AVRational time_base, JavaCallHelper *javaCallHelper) : id(id), codecContext(codecContext), time_base(time_base), javaCallHelper(javaCallHelper) {
        packets.setReleaseCallback(releaseAVPacket);
        frames.setReleaseCallback(releaseAVFrame);
    }

    //虚函数，父类的析构函数，一定要virtual，定义虚函数
    virtual ~BaseChannel() {
        packets.clear();
        frames.clear();
        if(codecContext) {
            avcodec_close(codecContext);
            avcodec_free_context(&codecContext);
            codecContext = 0;
        }
    }

    static void releaseAVPacket(AVPacket **packet) {
        if(packet) {
            av_packet_free(packet);
            *packet = 0; //指针解引用
        }
    }

    static void releaseAVFrame(AVFrame **frame) {
        if(frame) {
            av_frame_free(frame);
            *frame = 0; //指针解引用
        }
    }

    //纯虚函数（抽象方法）
    virtual void stop() = 0;
    virtual void start() = 0;

    SafeQueue<AVPacket *> packets;
    SafeQueue<AVFrame *> frames;
    int id;
    bool isPlaying = 0;
    AVCodecContext *codecContext; //解码器上下文
    AVRational time_base;
    double audio_time;
    JavaCallHelper *javaCallHelper = 0;
};

#endif //MYFFMPEGPROJECT_BASECHANNEL_H
