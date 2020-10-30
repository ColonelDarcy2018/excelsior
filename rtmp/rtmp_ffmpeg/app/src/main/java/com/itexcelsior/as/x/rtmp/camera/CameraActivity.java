package com.itexcelsior.as.x.rtmp.camera;

import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.itexcelsior.as.x.rtmp.ffmpeg.R;

public class CameraActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static String TAG = "ZXW@CameraActivity";

    private CameraHandler cameraHandler;

//    private  String url="rtmp://49.232.93.219/live/test";
    private static String url="rtmp://192.168.5.190/live/test";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        SurfaceView sv=(SurfaceView) findViewById(R.id.sv);
        Camera.PreviewCallback callback=new RtmpPreviewCallback();
        cameraHandler = CameraHandler.build(sv,callback,this);

        /**初始化rtmp传输上下文配置*/
        FFmpegRtmpCameraNative.get().initVideo(url,720,480);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //关闭Ffmpeg相关上下文
        FFmpegRtmpCameraNative.get().close();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        cameraHandler.setPreviewDisplay();//开始预览
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        cameraHandler.setPreviewDisplay();//开始预览
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        cameraHandler.releaseCamera();
    }

}