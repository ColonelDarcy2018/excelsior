#include "ffmpeg_common.h"



///摄像头Rtmp推流开始
AVFormatContext *ofmt_ctx;
AVCodecContext *codec_ctx;
AVStream *av_stm;
int yuv_width;
int yuv_height;
int y_length;
int uv_length;

int fps = 15;

/**
 * 初始化
 * @param env
 * @param thiz
 * @param url 输出路径/rtmp推流地址
 * @param width
 * @param height
 * @return
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_itexcelsior_as_x_rtmp_camera_FFmpegRtmpCameraNative_initVideo(JNIEnv *env, jobject thiz,
                                                                       jstring _url, jint _width,
                                                                       jint _height) {
    const char *out_path = env->GetStringUTFChars(_url, 0);
    logd(out_path);

    int width = _width;
    int height = _height;

    //计算yuv数据的长度
    yuv_width = width;
    yuv_height = height;
    y_length = width * height;
    uv_length = width * height / 4;

    int ret;

    //注册全部设备
    av_register_all();
    //Do global initialization of network components
//    avformat_network_init();

    //初始化编解码器
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        loge("Can not find encoder!\n");
        return -1;
    }
    //为输出格式分配AVFormatContext   初始化输出编码上下文
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_path);

    //获取编码器上下文
    codec_ctx = avcodec_alloc_context3(codec);
    //编码器的ID号，这里为264编码器，可以根据video_st里的codecID 参数赋值
    codec_ctx->codec_id = codec->id;
    //像素的格式，也就是说采用什么样的色彩空间来表明一个像素点
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//    codec_ctx->pix_fmt = AV_PIX_FMT_NV21;
    //编码器编码的数据类型
    codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    //编码目标的视频帧大小，以像素为单位
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->framerate = (AVRational) {fps, 1};
    //帧率的基本单位
    codec_ctx->time_base = (AVRational) {1, fps};
    //目标的码率，即采样的码率；
    codec_ctx->bit_rate = 400000;
    //50
    codec_ctx->gop_size = 50;

    /* Some formats want stream headers to be separate. */
    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    codec_ctx->qcompress = 0.6; ///< amount of qscale change between easy & hard scenes (0.0-1.0)
    //最大和最小量化系数
    codec_ctx->qmin = 10;
    codec_ctx->qmax = 51;
    //B帧
    codec_ctx->max_b_frames = 0;
    // Set H264 preset and tune
    AVDictionary *param = 0;
    /**
     * 这个非常重要，如果不设置延时非常的大
     * ultrafast,superfast, veryfast, faster, fast, medium
     * slow, slower, veryslow, placebo.　这是x264编码速度的选项
    */
    av_dict_set(&param, "preset", "superfast", 0);
    av_dict_set(&param, "tune", "zerolatency", 0);
    ///Initialize the AVCodecContext to use the given AVCodec
    ret=avcodec_open2(codec_ctx,codec,&param);
    if (ret<0){
        loge("Failed to open encoder!\n");
        avError(ret);
        return -1;
    }
    //Add a new stream to output
    av_stm=avformat_new_stream(ofmt_ctx,codec);
    if (av_stm == NULL){
        loge("添加流失败，返回NULL");
        return -1;
    }
    av_stm->time_base.num=1;
    av_stm->time_base.den=fps;
    av_stm->codecpar->codec_tag=0;
    avcodec_parameters_from_context(av_stm->codecpar,codec_ctx);

    ret=avio_open(&ofmt_ctx->pb,out_path,AVIO_FLAG_READ_WRITE);
    if (ret<0){
        loge("Failed to open output file! \n");
        avError(ret);
        return -1;
    }

    avformat_write_header(ofmt_ctx,NULL);

    return 0;

}

//This structure describes decoded (raw) audio or video data
AVFrame *av_frm;

int count=0;
AVPacket av_pkt;
/**
 * H264编码视频流并输出
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_itexcelsior_as_x_rtmp_camera_FFmpegRtmpCameraNative_encodeFrame(JNIEnv *env, jobject thiz,
                                                                         jbyteArray _buffer) {


//    int64_t start_time=av_gettime();
    jbyte *in=env->GetByteArrayElements(_buffer,NULL);

    int ret=0;
    ///Allocate an AVFrame and set its fields to default values.
    av_frm=av_frame_alloc();

    ///Return the size in bytes of the amount of data required to store an image with the given parameters.
    int picture_size=av_image_get_buffer_size(codec_ctx->pix_fmt,codec_ctx->width,codec_ctx->height,1);

    ///Allocate a memory block with alignment suitable for all memory accesses (including vectors if available on the CPU).
    uint8_t *buffers= (uint8_t *)av_malloc(picture_size);

    av_image_fill_arrays(av_frm->data,av_frm->linesize,buffers,codec_ctx->pix_fmt,codec_ctx->width,codec_ctx->height,1);

    //安卓摄像头数据为NV21格式，此处将其转换为YUV420P格式
    //复制Y分量的数据
    memcpy(av_frm->data[0],in,y_length);
    ///Presentation timestamp in time_base units (time when frame should be shown to user).
    av_frm->pts=count;
    for (int i = 0; i < uv_length; i++) {
        //将v数据存到第三个平面
        *(av_frm->data[2] + i) = *(in + y_length + i * 2);
        //将U数据存到第二个平面
        *(av_frm->data[1] + i) = *(in + y_length + i * 2 + 1);
    }
    av_frm->format=AV_PIX_FMT_YUV420P;
    av_frm->width=yuv_width;
    av_frm->height=yuv_height;

    //例如对于H.264来说。1个AVPacket的data通常对应一个NAL
    //初始化AVPacket
    av_init_packet(&av_pkt);
    //开始编码YUV数据
    ret=avcodec_send_frame(codec_ctx,av_frm);
    if (ret!=0){
        logw("avcodec_send_frame error");
        avError(ret);
        return -1;
    }

    //获取编码后的数据
    ret=avcodec_receive_packet(codec_ctx,&av_pkt);

    //释放编码前的YUV数据
    av_frame_free(&av_frm);
    if (ret != 0 || av_pkt.size<= 0){
        loge("avcodec_receive_packet error");
        avError(ret);
        return -2;
    }
    av_pkt.stream_index=av_stm->index;  /**< stream index in AVFormatContext */
    AVRational time_base= ofmt_ctx->streams[0]->time_base;
    av_pkt.pts=count*(av_stm->time_base.den)/((av_stm->time_base.num)*fps);
    av_pkt.dts=av_pkt.pts;
    av_pkt.duration = (av_stm->time_base.den)/((av_stm->time_base.num)*fps);


    __android_log_print(ANDROID_LOG_WARN, "zxw",
                        "index:%d,pts:%lld,dts:%lld,duration:%lld,time_base:%d,%d",
                        count,
                        (long long) av_pkt.pts,
                        (long long) av_pkt.dts,
                        (long long) av_pkt.duration,
                        time_base.num, time_base.den);

    av_pkt.pos=-1;

    ret=av_interleaved_write_frame(ofmt_ctx,&av_pkt);///Write a packet to an output media file ensuring correct interleaving.
    if (ret != 0) {
        loge("av_interleaved_write_frame failed");
        avError(ret);
    }


//    av_packet_unref(&av_pkt);
    count++;
    env->ReleaseByteArrayElements(_buffer,in,0);

    return 0;
}


///摄像头Rtmp推流结束
extern "C"
JNIEXPORT jint JNICALL
Java_com_itexcelsior_as_x_rtmp_camera_FFmpegRtmpCameraNative_close(JNIEnv *env, jobject thiz) {
    if (av_stm)
        avcodec_close(av_stm->codec);
    if (ofmt_ctx) {
        avio_close(ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
        ofmt_ctx = NULL;
    }
    return 0;
}








