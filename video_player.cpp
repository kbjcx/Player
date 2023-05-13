#include "video_player.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

VideoPlayer::VideoPlayer() = default;

VideoPlayer::~VideoPlayer() noexcept = default;

void VideoPlayer::start_play() {
    // 调用QThread的start函数, 会自动在一个新的线程执行run()函数
    this->start();
}

void VideoPlayer::run() {
    printf("run \n");
//    const char* file_path = filename_.toUtf8().data();
    const char* file_path = "E:/CPP/Player/Source/test.mp4";
    printf("%s \n", file_path);
    int video_stream = -1;
    int ret = -1;
    
    
    AVFormatContext* format_context = nullptr;
    format_context = avformat_alloc_context();
    ret = avformat_open_input(&format_context, file_path, nullptr, nullptr);
    if (ret < 0) {
        printf("can not open the file, error code %d \n", ret);
        return;
    }
    
    ret = avformat_find_stream_info(format_context, nullptr);
    if (ret < 0) {
        printf("find stream info error, error code %d \n", ret);
        return;
    }
    
    // 循环查找视频中的流信息
    for (int i = 0; i < format_context->nb_streams; ++i) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream = i;
        }
    }
    
    // video_stream = -1说明没有找到视频流
    if (video_stream == -1) {
        printf("can not find a video stream \n");
        return;
    }
    
    AVCodecContext* codec_context = nullptr;
    // AVCodec可以在avcodec_alloc_context3或者avcodec_open2的时候传入都可以
    codec_context = avcodec_alloc_context3(nullptr);
    
    ret = avcodec_parameters_to_context(codec_context,
                                        format_context->streams[video_stream]->codecpar);
    if (ret < 0) {
        printf("avcodec parameters to context error code %d \n", ret);
        return;
    }
    
    AVCodec* decoder = nullptr;
    
    AVCodecID decoder_id = codec_context->codec_id;
    printf("video decoder ID: %d \n", decoder_id);
    decoder = avcodec_find_decoder(decoder_id);
    if (decoder == nullptr) {
        printf("can not find decoder \n");
        return;
    }
    
    printf("find decoder: %s \n", decoder->long_name);
    
    ret = avcodec_open2(codec_context, decoder, nullptr);
    if (ret < 0) {
        printf("can not open decoder \n");
        return;
    }
    
    AVFrame* frame = nullptr;
    AVFrame* frame_rgb = nullptr;
    frame = av_frame_alloc();
    frame_rgb = av_frame_alloc();
    
    static struct SwsContext* img_convert_context = nullptr;
    img_convert_context = sws_getContext(codec_context->width,
                                         codec_context->height,
                                         codec_context->pix_fmt,
                                         codec_context->width,
                                         codec_context->height,
                                         AV_PIX_FMT_RGB24,
                                         SWS_BICUBIC,
                                         nullptr, nullptr, nullptr);
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                             codec_context->width,
                                             codec_context->height,
                                             1);
    if (num_bytes < 0) {
        printf("av image get buffer size failed \n");
        return;
    }
    
    uint8_t* out_buffer = (uint8_t*) av_malloc(num_bytes * sizeof(uint8_t));
    // TODO
    av_image_fill_arrays(frame_rgb->data, frame_rgb->linesize, out_buffer,
                         AV_PIX_FMT_RGB24, codec_context->width,
                         codec_context->height, 1);
    
    int y_size = codec_context->width * codec_context->height;
    // TODO
    AVPacket* packet = av_packet_alloc();
    av_new_packet(packet, y_size);
    av_dump_format(format_context, 0, file_path, 0);
    
    int index = 0;
    int read_end = 0;
    
    while (true) {
        if (read_end == 1) {
            break;
        }
        
        ret = av_read_frame(format_context, packet);
        // 跳过非视频流
        if (packet->stream_index != video_stream) {
            av_packet_unref(packet);
        }
        
        if (ret == AVERROR_EOF) {
            avcodec_send_packet(codec_context, packet);
            av_packet_unref(packet);
            
            // 循环从解码器中读取数据
            while (1) {
                ret = avcodec_receive_frame(codec_context, frame);
                if (ret == AVERROR(EAGAIN)) {
                    break;
                }
                else if (ret == AVERROR_EOF) {
                    read_end = 1;
                    break;
                }
                else if (ret > 0) {
                    sws_scale(img_convert_context, (const uint8_t* const*)frame->data,
                              frame->linesize, 0, codec_context->height,
                              frame_rgb->data, frame_rgb->linesize);
                    
                    QImage tmp_img((uchar*)out_buffer, codec_context->width,
                                   codec_context->height,
                                   QImage::Format_RGB32);
                    const QImage& image = tmp_img;
                    printf("get frame \n");
                    emit signal_get_frame(image); // 发送信号
                }
                else {
                    printf("other error \n");
                    return;
                }
                msleep(500);
            }
        }
        else if (ret == 0) {
            while (avcodec_send_packet(codec_context, packet) == AVERROR(EAGAIN)) {
                printf("Receive_frame and send_packet both returned EAGAIN,"
                       "which is an API violation. \n");
                usleep(1000);
            }
            av_packet_unref(packet);
            while (true) {
                ret = avcodec_receive_frame(codec_context, frame);
                if (ret == AVERROR(EAGAIN)) {
                    break;
                }
                else if (ret == AVERROR_EOF) {
                    read_end = 1;
                    break;
                }
                else if (ret >= 0) {
                    sws_scale(img_convert_context, (const uint8_t* const*) frame->data,
                              frame->linesize, 0, frame->height,
                              frame_rgb->data, frame_rgb->linesize);
                    QImage tmp_img((uchar*)out_buffer, codec_context->width,
                                   codec_context->height,
                                   QImage::Format_RGB32);
                    const QImage& image = tmp_img;
                    printf("get frame \n");
                    emit signal_get_frame(image); // 发送信号
                }
                else {
                    printf("other error \n");
                    return;
                }
                msleep(500);
            }
        }
    }
    av_packet_free(&packet);
    av_frame_free(&frame);
    av_frame_free(&frame_rgb);
    av_free(out_buffer);
    avcodec_close(codec_context);
    avformat_close_input(&format_context);
}

/*!
 * @paragraph: 将解码后的AVFrame保存成为PPM格式文件
 * @param frame: 解码后的RGB数据
 * @param width: 图像宽度
 * @param height: 图像高度
 * @param index: 图像编号
 */
//void VideoPlayer::save_frame(AVFrame *frame, int width, int height, int index) {
//    FILE* file;
//    char filename[32];
//    int y;
//
//    // open file
//    sprintf(filename, "frame%d.ppm", index);
//    file = fopen(filename, "wb");
//
//    if (file == nullptr) {
//        return;
//    }
//
//    // write header
//    fprintf(file, "P6 %d %d 255", width, height);
//    // write pixmap data
//    for (y = 0; y < height; ++y) {
//        fwrite(frame->data[0] + y * frame->linesize[0], 1, 3 * width, file);
//    }
//
//    // close file
//    fclose(file);
//}
