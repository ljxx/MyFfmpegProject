#include <jni.h>
#include <string>
#include "JavaCallHelper.h"
#include "NEFFmpeg.h"

//extern "C" {
//#include <libavutil/avutil.h>
//}

//extern "C" JNIEXPORT jstring JNICALL
//Java_com_ylx_myffmpegproject_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(av_version_info());
//}

JavaVM *javaVM = 0;
JavaCallHelper *javaCallHelper = 0;
NEFFmpeg *ffmpeg = 0;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ylx_myffmpegproject_NEPlay_prepareNative(JNIEnv *env, jobject instance, jstring data_source) {
    // TODO: implement prepareNative()

    const char *dataSource = env->GetStringUTFChars(data_source, 0);

    javaCallHelper = new JavaCallHelper(javaVM, env, instance);
    ffmpeg = new NEFFmpeg(javaCallHelper, const_cast<char *>(dataSource));
    ffmpeg->prepare();

    env->ReleaseStringUTFChars(data_source, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ylx_myffmpegproject_NEPlay_startNative(JNIEnv *env, jobject thiz) {
    // TODO: implement startNative()
    if(ffmpeg) {
        ffmpeg->start();
    }
}