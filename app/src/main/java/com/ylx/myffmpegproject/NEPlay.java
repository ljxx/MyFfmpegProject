package com.ylx.myffmpegproject;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * createTime：2020-03-15  13:39
 * author：YLiXiang
 * description：
 */
public class NEPlay implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("native-lib");
    }


    //直播地址或媒体文件路径
    private String dataSource;

    private SurfaceHolder surfaceHolder;


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
     * 资源释放
     */
    public void release() {
        surfaceHolder.removeCallback(this);
        releaseNative();
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

    public void setSurfaceView(SurfaceView surfaceView) {
        if(null != surfaceHolder) {
            surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    /**
     * 画布创建回调
     * @param holder
     */
    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    /**
     * 画布刷新
     * @param holder
     * @param format
     * @param width
     * @param height
     */
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        setSurfaceNative(holder.getSurface());
    }

    /**
     * 画布销毁
     * @param holder
     */
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    /**
     * 停止播放
     */
    public void stop() {
        stopNative();
    }

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

    private native void setSurfaceNative(Surface surface);

    private native void releaseNative();

    private native void stopNative();
}
