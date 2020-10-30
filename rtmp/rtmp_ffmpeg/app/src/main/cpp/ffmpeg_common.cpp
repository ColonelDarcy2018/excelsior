//
// Created by zxw on 2020/9/30.
//
#include "ffmpeg_common.h"

/**
 * 查询ffmpeg配置信息
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_itexcelsior_as_x_rtmp_ffmpeg_MainActivity_getAvcodecConfiguartion(JNIEnv *env,
                                                                           jobject thiz) {

    char info[1500] = {0};

    sprintf(info, "%s\n", avcodec_configuration());

    return env->NewStringUTF(info);
}


/**
 * 打印错误日志
 * @param errNum
 * @return
 */
int avError(int errNum) {
    char buf[1024];
    //获取错误信息
    av_strerror(errNum, buf, sizeof(buf));
    loge(std::string().append("发生异常：").append(buf).c_str());
    return -1;
}


/**
 * 打印设备信息
 * @param fmt_ctx
 */
void print_device_info(AVFormatContext *fmt_ctx){
    AVInputFormat *iformat;
    AVDeviceInfoList *deviceInfoList;
    AVDeviceInfo *deviceInfo;
    int device_num,i;
    char *msg;
    logi("----------------------设备信息打印开始----------------------");
    while (true){
        iformat=av_input_audio_device_next(iformat);
        if(iformat->next == NULL) break;
        if (iformat){
            iformat->get_device_list(fmt_ctx,deviceInfoList);
            device_num=deviceInfoList->nb_devices;
            if (device_num>0){
                for (i = 0; i < device_num; ++i) {
                    deviceInfo=deviceInfoList->devices[i];
                    msg="|-设备名称：";
                    strcat(msg,deviceInfo->device_name);
                    strcat(msg,"-|\n");
                    strcat(msg,"|-设备描述：");
                    strcat(msg,deviceInfo->device_description);
                    strcat(msg,"-|\n");
                    logi(msg);
                }
            }else{
                loge("设备数量异常，小于等于0 ..............");
            }
        } else{
            loge("iformat is NULL ..............");
        }
    }

    logi("----------------------设备信息打印完毕----------------------");

}


/**
 * 打开设备
 * @param fmt_ctx
 * @param options
 * @return
 */
bool open_device(AVFormatContext *fmt_ctx, AVDictionary *options) {
    AVInputFormat *iformat = NULL;

    //get format 设置采集方式 此方法需要改为安卓环境下的设备缩写名称   lavfi
//    iformat = av_find_input_format("android_camera");
    iformat=av_input_audio_device_next(NULL);

    while (iformat){
        av_input_audio_device_next(iformat);
    }

    //TODO 此处需要改为安卓环境的设备名称
    char *devicename = ":0";

    //open device
    int ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options);

    char errors[1024] = {0,};
    if (ret < 0) {
        av_strerror(ret, errors, 1024);

        printf("Failed to open device,[%d]%s\n", ret, errors);

        return false;
    }
    return true;
}