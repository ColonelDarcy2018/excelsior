package com.itexcelsior.as.x.rtmp.camera;

public class FFmpegRtmpCameraNative {


    private static FFmpegRtmpCameraNative instance=null;

    private FFmpegRtmpCameraNative(){}

    public synchronized static FFmpegRtmpCameraNative get(){

        if (instance==null){
            instance=new FFmpegRtmpCameraNative();
        }
        return instance;
    }

    static {
        System.loadLibrary("native-lib");
    }


    public native int initVideo(String url,int width,int height);

    public native int encodeFrame(byte[] buffer);

    public native int close();

}
