//
// Created by 杨理想 on 2020-03-15.
//

#include "VideoChannel.h"

/**
 * 丢包（AVPacket）
 * @param q
 */
void dropAVPacket(queue<AVPacket *> &q) {
    while (!q.empty()) {
        AVPacket *avPacket = q.front();
        // I 帧，B 帧， p 帧
        //不能丢I帧
        if(avPacket->flags != AV_PKT_FLAG_KEY) {
            //丢弃非I帧
            BaseChannel::releaseAVPacket(&avPacket);
            q.pop();
        } else {
            break;
        }
    }
}


/**
 * 丢包（AVFrame）
 * @param q
 */
void dropAVFrame(queue<AVFrame *> &q) {
    if (!q.empty()) {
        AVFrame *avFrame = q.front();
        BaseChannel::releaseAVFrame(&avFrame);
        q.pop();
    }
}
VideoChannel::VideoChannel(int id, AVCodecContext *codecContext, int fps, AVRational time_base) : BaseChannel(id, codecContext, time_base) {
    this->fps = fps;
    packets.setSyncHandle(dropAVPacket);
    frames.setSyncHandle(dropAVFrame);
}

VideoChannel::~VideoChannel() {

}

void *task_video_decode(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    //2 datasource
    videoChannel->video_decode();
    return 0; //一定一定返回0
}

void *task_video_play(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    //2 datasource
    videoChannel->video_play();
    return 0; //一定一定返回0
}

void VideoChannel::start() {
    isPlaying = 1;
    //设置队列状态为工作状态
    packets.setWork(1);
    frames.setWork(1);
    //可以进行解码播放?
    //解码
    pthread_create(&pid_video_decode, 0, task_video_decode, this);
    //播放
    pthread_create(&pid_video_play, 0, task_video_play, this);

}

void VideoChannel::stop() {

}

/**
 * 真正的视频解码
 */
void VideoChannel::video_decode() {
    AVPacket *packet = 0;

    while (isPlaying) {
        int ret = packets.pop(packet);
        if(!isPlaying) {
            //如果停止了播放，跳出循环
            break;
        }
        if(!ret) {
            //取数据包失败
            continue;
        }
        //拿到了视频数据包（编码压缩了的），需要把数据包给解码器进行解码

        ret = avcodec_send_packet(codecContext, packet);
        if(ret) {
            //往解码器发送数据包失败，跳出循环
            break;
        }

        releaseAVPacket(&packet); //是否packet,后面不需要了

        AVFrame * frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        if(ret == AVERROR(EAGAIN)){
            //重来
            continue;
        } else if(ret != 0) {
            break;
        }

        //ret == 0 数据收发正常，成功获取到了解码后的视频原始数据
        //对frame进行处理（渲染播放）直接写？
        /**
         * 内存泄漏2
         * 控制frames队列
         * 这里用while,而不是if，是防止上面取的数据丢帧
         */
        while (isPlaying && frames.size() > 100) {
            av_usleep(10* 1000);
            continue;
        }

        frames.push(frame);

    } //end while
    releaseAVPacket(&packet);
}

void VideoChannel::video_play() {
    AVFrame *frame = 0;
    //要注意对原始数据进行格式转换：yuv > rgba

    uint8_t *dst_data[4];
    int dst_linesize[4];
    SwsContext *sws_ctx = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
                                         codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,SWS_BILINEAR, NULL, NULL, NULL);
    //给dst_data dst_linesize 申请内存
    av_image_alloc(dst_data,dst_linesize,codecContext->width, codecContext->height, AV_PIX_FMT_RGBA, 1);

    //根据fps（传入的流的平均帧率来控制每一帧的延时时间
    //sleep : fps ->时间
    //单位：秒
    double delay_time_per_frame = 1.0/fps;

    while (isPlaying) {
        int ret = frames.pop(frame);
        if(!isPlaying) {
            //如果停止了播放，跳出循环
            break;
        }
        if(!ret) {
            //取数据包失败
            continue;
        }

        //取到yuv原始数据，下面要进行格式转换
        sws_scale(sws_ctx, frame->data, frame->linesize, 0, codecContext->height, dst_data, dst_linesize);

        //进行休眠
        //每一帧还有自己的额外延时时间

        ///extra_delay = repeat_pict / (2*fps)
        double extra_delay = frame->repeat_pict / (2*fps);
        double  real_delay = delay_time_per_frame + extra_delay;
        //单位是：微秒
//        av_usleep(real_delay * 1000000); //直接以视频的播放规则播放
        //不能了，需要根据音频的播放时间来判断
        //获取视频的播放时间
        double video_time = frame->best_effort_timestamp * av_q2d(time_base); //时间单位

        //音视频同步永远都是你追我赶丢状态
        if(!audioChannel) {
            //没有音频（类似gif）
            av_usleep(real_delay * 1000000);
        } else{
            double audioTime = audioChannel->audio_time;
            //获取音视频播放的时间差
            double time_diff = video_time - audioTime;
            if(time_diff > 0) {
                LOGE("视频比音频快：%lf", time_diff);
                //视频比音频快，需要等音频(sleep)
                //自然播放状态，没有人为干预的情况下time_diff不会很大
                //但是如果有拖动seek后，time_diff可能会比较大， 导致视频休眠太久
                if(time_diff > 1) {
                    //等音频慢慢赶上 延时2倍
                    av_usleep((real_delay * 2) * 1000000);
                } else {
                    av_usleep((real_delay + time_diff) * 1000000);
                }
            } else if(time_diff < 0) {
                LOGE("音频比视频快：%lf", fabs(time_diff));
                //音频比视频快, 追音频(尝试丢视频包)
                //视频包：packets 和 frames
                if(fabs(time_diff) >= 0.05) { //fabs 取绝对值
                    //时间差如果大于0.05，有明显等延迟感
                    //丢包：要操作队列中数据！一定要小心！
                    packets.sync();
//                    frames.sync();
                    continue;
                }

            } else {
                LOGE("音视频完美同步了！");
            }
        }

        //dst_data：AV_PIX_FMT_RGBA 目标格式对数据
        //渲染到屏幕中, 需要回调出去->native-lib里
        //渲染一副图像，需要什么信息？
        //宽，高 >图像的尺寸
        //图像的内容（数据） >图像怎么画
        //需要：1.data; 2.linesize; 3.width; 4.height
        renderCallback(dst_data[0], dst_linesize[0], codecContext->width, codecContext->height);
        releaseAVFrame(&frame); //释放

    }
    releaseAVFrame(&frame);
    isPlaying = 0;
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
}

void VideoChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}
