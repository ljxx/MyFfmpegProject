package com.ylx.myffmpegproject;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    private SurfaceView surfaceView;
    private NEPlay player;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.surfaceView);
        player = new NEPlay();
        player.setDataSource(new File(Environment.getExternalStorageDirectory() + File.separator + "demo.mp4").getAbsolutePath());
        player.setErrorListener(new NEPlay.MyErrorListener() {
            @Override
            public void onError(int errorCode) {
                switch (errorCode) {
                    case -1:
                        break;
                }
            }
        });

        player.setOnpreparedListener(new NEPlay.OnpreparedListener() {
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.e(TAG, "开始播放");
                        Toast.makeText(MainActivity.this, "开始播放！", Toast.LENGTH_SHORT).show();
                    }
                });

                //播放 调用到native去
                //start play
                player.start();
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();

        player.prepare();

    }
}
