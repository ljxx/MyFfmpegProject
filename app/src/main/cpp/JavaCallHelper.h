//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_JAVACALLHELPER_H
#define MYFFMPEGPROJECT_JAVACALLHELPER_H


#include <jni.h>
#include "macro.h"

class JavaCallHelper {

public:
    JavaCallHelper(JavaVM *javaVM_, JNIEnv *env_, jobject instance_);
    ~JavaCallHelper();

    void onPrepare(int threadMode);

private:
    JavaVM *javaVM;
    JNIEnv *env;
    jobject instance;
    jmethodID jmd_prepared;

};


#endif //MYFFMPEGPROJECT_JAVACALLHELPER_H
