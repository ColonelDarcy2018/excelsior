package com.itexcelsior.as.x.rtmp.camera;

import android.hardware.Camera;
import android.util.Log;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 视频预览回调函数，将摄像头返回的视频数据进行h264编码并发送到指定srs服务器
 */
public class RtmpPreviewCallback implements Camera.PreviewCallback{

    private static String TAG="ZXW@RtmpPreviewCallback";

    private ExecutorService executor= Executors.newSingleThreadExecutor();
    //采集到每帧数据时间
    long previewTime = 0;
    //开始编码时间
    long encodeTime = 0;
    //采集数量
    int count = 0;
    //编码数量
    int encodeCount = 0;

    @Override
    public void onPreviewFrame(final byte[] bytes, Camera camera) {
        long endTime=System.currentTimeMillis();
        executor.execute(new Runnable() {
            @Override
            public void run() {
                encodeTime=System.currentTimeMillis();
                FFmpegRtmpCameraNative.get().encodeFrame(bytes);
                Log.w(TAG,"编码第："+(encodeCount++)+"帧，耗时："+(System.currentTimeMillis()-encodeTime));
            }
        });
        Log.d(TAG,"采集第:" + (++count) + "帧，距上一帧间隔时间:"
                + (endTime - previewTime) + "  " + Thread.currentThread().getName());
        previewTime = endTime;
    }
}
