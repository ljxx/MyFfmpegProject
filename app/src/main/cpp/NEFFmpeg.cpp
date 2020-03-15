//
// Created by 杨理想 on 2020-03-15.
//

#include "NEFFmpeg.h"

NEFFmpeg::NEFFmpeg(JavaCallHelper *javaCallHelper, char *dataSource) {
    this->javaCallHelper = javaCallHelper;

    //这里的dataSource是从Java传过来的字符串，通过jni接口转成了c++字符串。
    //在jni方法中被释放掉了，导致 dataSource 变成悬空指针的问题（指向一块已经释放了的内存）
    //怎么办呢？？
    //内存拷贝，自己管理它的内存
    //方法：strlen 获取字符串长度，strcpy：拷贝字符串

    //java: "hello"
    //c字符串以 \0 结尾 : "hello\0"
    this->dataSource = new char[strlen(dataSource) + 1];
    stpcpy(this->dataSource, dataSource);

}

NEFFmpeg::~NEFFmpeg() {

    //释放写法
//    if(dataSource) {
//        delete dataSource;
//        dataSource = 0;
//    }
//    if(javaCallHelper) {
//        delete javaCallHelper;
//        javaCallHelper = 0;
//    }

    //封装后的释放调用
    DELETE(dataSource);
    DELETE(javaCallHelper);
}

/**
 * 准备线程pid_prepare真正执行的函数, 相当于java中Thread中的run()方法
 * @param args
 * @return
 */
void *task_prepare(void* args) {

    NEFFmpeg *ffmpeg = static_cast<NEFFmpeg *>(args);
    ffmpeg->_prepare();

    return 0; //一定一定一定要返回0！！！ 防止找不到错误
}

/**
 * 播放准备
 * 可能是主线程
 * ffmpeg源码文件 doc/samples/
 */
void NEFFmpeg::prepare() {
    //可以直接来进行解码api调用吗？
    //xxxxxx。。。。不能！
    //文件：io流问题
    //直播：网络
    //pthread_create： 创建子线程

    //pthread_create(pthread_t* __pthread_ptr, pthread_attr_t const* __attr, void* (*__start_routine)(void*), void*);
    pthread_create(&pid_prepare, 0, task_prepare, this);
}

void NEFFmpeg::_prepare() {

    //0.5 AVFormatContext **ps
    AVFormatContext *formatContext = avformat_alloc_context();
    AVDictionary *dictionary = 0;
    av_dict_set(&dictionary, "timeout", "10000000", 0); //设置超时时间为10秒，这里的单位是微秒

    //1、打开媒体
    int ret = avformat_open_input(&formatContext, dataSource, 0, &dictionary);
    av_dict_free(&dictionary);
    if(ret) {
        LOGE("打开媒体失败：%s", av_err2str(ret));
        //失败 ，回调给java
        //        LOGE("打开媒体失败：%s", av_err2str(ret));
        ////        javaCallHelper jni 回调java方法
        ////        javaCallHelper->onError(ret);
        //          //可能java层需要根据errorCode来更新UI!
        //          //2019.8.12 作业自行尝试实现
        //TODO 作业:反射通知java
        // 1 反射Java 属性（成员/静态）
        // 2 反射Java 方法 （成员/静态）
        // 3 子线程反射

//        javaCallHelper->onError(ret);

        return;
    }

    //2、查找媒体中的信息
    ret = avformat_find_stream_info(formatContext, 0);
    if (ret < 0) {
        LOGE("查找媒体中的流信息失败：%s", av_err2str(ret));
        //TODO 作业:反射通知java
        return;
    }

    //这里的 i 就是后面 166行的 packet->stream_index
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //3获取媒体流（音频或视频）
        AVStream *stream = formatContext->streams[i];
        //4获取编解码这段流的参数
        AVCodecParameters *codecParameters = stream->codecpar;
        //5 通过参数中的id（编解码的方式），来查找当前流的解码器
        AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
        if(!codec) {
            LOGE("查找当前流的解码器失败");
            //TODO 作业:反射通知java
            return;
        }
        //6 创建解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        //7 设置解码器上下文的参数
        ret = avcodec_parameters_to_context(codecContext, codecParameters);
        if (ret < 0) {
            //TODO 作业:反射通知java
            LOGE("设置解码器上下文的参数失败：%s", av_err2str(ret));
            return;
        }

        //8 打开解码器
        ret = avcodec_open2(codecContext, codec, 0);
        if (ret) {
            //TODO 作业:反射通知java
            LOGE("打开解码器失败：%s", av_err2str(ret));
            return;
        }
        //判断流类型（音频还是视频？）
        if(codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audioChannel = new AudioChannel();
        } else if(codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            videoChannel = new VideoChannel();
        }
    } //end for

    if(!audioChannel && !videoChannel) {
        //既没有音频也没有视频
        //TODO 作业:反射通知java
        LOGE("没有音视频");
        return;
    }

    //准备好了，反射通知java
    if(javaCallHelper) {
        javaCallHelper->onPrepare(THREAD_CHILD);
    }

}

void NEFFmpeg::start() {

}
