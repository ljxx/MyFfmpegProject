package com.ylx.myffmpegproject;

/**
 * createTime：2020-03-15  13:39
 * author：YLiXiang
 * description：
 */
public class NEPlay {
    static {
        System.loadLibrary("native-lib");
    }


    //直播地址或媒体文件路径
    private String dataSource;


    public void setDataSource(String dataSource) {

        this.dataSource = dataSource;

    }

    /**
     * 播放准备工作
     */
    public void prepare() {
        prepareNative(dataSource);
    }

    /**
     * 开始播放
     */
    public void start() {
        startNative();
    }

    /**
     * 给JNI回调
     * @param code 通过JNI反射传递过来
     */
    public void onError(int code) {
        if(errorListener != null) {
            errorListener.onError(code);
        }
    }

    MyErrorListener errorListener;

    interface MyErrorListener {
        void onError(int errorCode);
    }

    void setErrorListener(MyErrorListener errorListener) {
        this.errorListener = errorListener;
    }

    /**
     * 供native反射调用
     * 表示播放器准备好了可以开始播放了
     */

    public void onPrepared() {
        if (onpreparedListener != null) {
            onpreparedListener.onPrepared();
        }

    }

    void setOnpreparedListener(OnpreparedListener onpreparedListener) {
        this.onpreparedListener = onpreparedListener;
    }


    interface OnpreparedListener {
        void onPrepared();
    }

    private OnpreparedListener onpreparedListener;


    private native void prepareNative(String dataSource);

    private native void startNative();
}
