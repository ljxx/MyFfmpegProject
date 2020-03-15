//
// Created by 杨理想 on 2020-03-15.
//

#ifndef MYFFMPEGPROJECT_BASECHANNEL_H
#define MYFFMPEGPROJECT_BASECHANNEL_H

extern "C" {

};

/**
 * BaseChannel是AudioChannel和VideoChannel的父类
 */
class BaseChannel {
public:
    BaseChannel() {

    }

    //虚函数，父类的析构函数，一定要virtual，定义虚函数
    virtual ~BaseChannel() {

    }
};

#endif //MYFFMPEGPROJECT_BASECHANNEL_H
