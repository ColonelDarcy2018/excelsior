//
// Created by Administrator on 2020/9/30.
//

#ifndef RTMP_FFMPEG_FFMPEG_COMMON_H
#define RTMP_FFMPEG_FFMPEG_COMMON_H

#include <android/log.h>
#include <string>
#include <jni.h>
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>

#include "libavutil/imgutils.h"
#include "libavutil/time.h"
}


#define logd(content)   __android_log_write(ANDROID_LOG_DEBUG,"zxw",content)
#define loge(content)   __android_log_write(ANDROID_LOG_ERROR,"zxw",content)
#define logw(content)   __android_log_write(ANDROID_LOG_WARN,"zxw",content)
#define logi(content)   __android_log_write(ANDROID_LOG_INFO,"zxw",content)

/**
 * 打印设备信息
 * @param fmt_ctx
 */
void print_device_info(AVFormatContext *fmt_ctx);
/**
 * 打印错误日志
 * @param errNum
 * @return
 */
int avError(int errNum);


#endif //RTMP_FFMPEG_FFMPEG_COMMON_H
