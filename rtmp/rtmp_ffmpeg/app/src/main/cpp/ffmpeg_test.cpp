//
// Created by Administrator on 2020/10/15.
//
#include "ffmpeg_common.h"


static AVFormatContext* open_dev(){
    int ret=0;

    //ctx
    AVFormatContext *fmt_ctx=NULL;
    AVDictionary *options=NULL;

    //设备名称
    char *devicename="back";

    //注册设备 Initialize libavdevice and register all the input and output devices.
    avdevice_register_all();

//    fmt_ctx = avformat_alloc_context();
    AVInputFormat *iformat=NULL;
    /*while (true){
        //audio: lavfi   video: fbdev lavfi video4linux2,v4l2
        iformat =av_input_video_device_next(iformat);
        if(iformat->next == NULL) break;
        if (iformat){
        } else{
            loge("iformat is NULL ..............");
            break;
        }
    }*/

    //获取格式
    iformat=av_find_input_format("video4linux2");

    AVDeviceInfoList *deviceInfoList;
//    avdevice_list_devices(fmt_ctx,&deviceInfoList);

    //获取设备名称
    ret=avdevice_list_input_sources(iformat,devicename,NULL,&deviceInfoList);
    if (ret<0){
        avError(ret);
    }

    //打开设备
    if (ret = avformat_open_input(&fmt_ctx,devicename,iformat,&options)<0){
        avError(ret);
    }

    return fmt_ctx;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_itexcelsior_as_x_rtmp_ffmpeg_MainActivity_test(JNIEnv *env, jobject thiz) {
    open_dev();
}



