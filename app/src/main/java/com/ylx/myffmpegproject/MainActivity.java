package com.ylx.myffmpegproject;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    private SurfaceView surfaceView;
    private SeekBar seekBar; //进度条-与播放总时长有关系
    private NEPlay player;
    private boolean isTouch; //释放点击
    private boolean isSeek; //释放拖动

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.surfaceView);
        seekBar = findViewById(R.id.seekBar);
        player = new NEPlay();
        player.setSurfaceView(surfaceView);
        player.setDataSource(new File(Environment.getExternalStorageDirectory() + File.separator + "demo.mp4").getAbsolutePath());
        player.setErrorListener(new NEPlay.MyErrorListener() {
            @Override
            public void onError(final int errorCode) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "出错了，错误码：" + errorCode, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        });

        player.setOnpreparedListener(new NEPlay.OnpreparedListener() {
            @Override
            public void onPrepared() {
                int duration = player.getDuration();
                //如果是直播，duration是0
                //不为0，可以显示seekBar
                if (duration != 0) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {

                            seekBar.setVisibility(View.VISIBLE);

                            Log.e(TAG, "开始播放");
                            Toast.makeText(MainActivity.this, "开始播放！", Toast.LENGTH_SHORT).show();
                        }
                    });
                }

                //播放 调用到native去
                //start play
                player.start();
            }
        });

        player.setOnProgressListener(new NEPlay.OnProgressListener() {
            @Override
            public void onProgress(final int progress) {
                //progress:当前的播放进度
                Log.e("MainActivity", progress + "");
                //duration
                //如果没有人为干预进度条，让进度条自然的正常播放
                if(!isTouch) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            int duration = player.getDuration();
                            if(duration != 0) {
                                if(isSeek) {
                                    isSeek = false;
                                    return;
                                }
                                seekBar.setProgress(progress * 100 / duration);
                            }
                        }
                    });
                }
            }
        });

        //跟随播放进度自动刷新进度：拿到没给时间点相对总播放时长的百分比progress
        //1、总时长 getDurationNative
        //2、当前播放时间：随播放进度动态变化的
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isTouch = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                isSeek = true;
                isTouch = false;

                //获取seekbar的当前进度（百分比）
                int seekBarProgresss = seekBar.getProgress();
                //将seekbar的进度转换成真实的播放进度
                int duration = player.getDuration();
                int playProgress = seekBarProgresss * duration / 100;
                //将播放进度传给底层ffmpeg
                player.seekTo(playProgress);
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();

        player.prepare();

    }

    @Override
    protected void onStop() {
        super.onStop();
        player.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        player.release();
    }
}
