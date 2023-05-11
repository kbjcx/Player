#include <stdio.h>
extern "C"{
#include <libavcodec//avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

/*!
 * 将获取的RGB信息写入文件生成图像
 * @param frame
 * @param width
 * @param height
 * @param index
 */
void save_frame(AVFrame* frame, int width, int height, int index) {
    FILE* file;
    char sz_filename[32];
    int y;
    
    // open file
    sprintf(sz_filename, "frame%d.ppm", index);
    file = fopen(sz_filename, "wb");
    
    if (file == nullptr) {
        return;
    }
    
    // write header
    fprintf(file, "P6 %d %d 255", width, height);
    // write pixel data
    for (y = 0; y < height; ++y) {
        fwrite(frame->data[0] + y * frame->linesize[0], 1, width * 3, file);
    }
    
    // close file
    fclose(file);
}
int get_video_img() {
    const char* file_path = "E://CPP/opencv_test/Source/test.mp4";

// 文件容器
    AVFormatContext* format_context;

// 编解码器
    AVCodec* codec;
// 解码后的数据
    AVFrame* frame;
    AVFrame* frame_RGB;
// 编码后的数据
    AVPacket* packet;
    uint8_t* out_buffer;
    
    static struct SwsContext* img_convert_context;
    int video_stream, i, num_bytes;
    int ret, got_picture = 1;

// av_register_all();

// allocate an AVFormatContext
    format_context = avformat_alloc_context();
    
    ret = avformat_open_input(&format_context, file_path, nullptr, nullptr);
    
    if (ret < 0) {
        printf("can't open the file, error code %d \n", ret);
        return ret;
    }
    
    if (avformat_find_stream_info(format_context, nullptr) < 0) {
        printf("find stream info error \n");
        return -1;
    }
    
    video_stream = -1;

// 循环查找视频中包含的流信息, 直到找到视频类型的流
// 将其记录下来, 保存到video_stream变量中
// 这里只处理视频流, 不管音频流
    for (i = 0; i < format_context->nb_streams; ++i) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream = i;
        }
    }

// 如果video_stream = -1 说明没有找到视频流
    if (video_stream == -1) {
        printf("didn't find a video stream \n");
        return -1;
    }

// 编解码器容器
    AVCodecContext* codec_context = nullptr;
    codec_context =  avcodec_alloc_context3(nullptr);
    ret = avcodec_parameters_to_context(codec_context,
                                        format_context->streams[video_stream]->codecpar);
    
    if (ret < 0) {
        printf("error code %d \n", ret);
        return ret;
    }
// 查找解码器
    AVCodec* video_decoder = nullptr;
    AVCodecID video_decoder_id = codec_context->codec_id;
    printf("video decoder id: %d \n", video_decoder_id);
    video_decoder = avcodec_find_decoder(video_decoder_id);
    
    if (video_decoder == nullptr) {
        printf("decoder not found \n");
        return -1;
    }
    printf("name: %s \n",
           video_decoder->name);
    
    
// 打开解码器
    ret = avcodec_open2(codec_context, video_decoder, nullptr);
    
    if (ret < 0) {
        printf("can not open decoder \n");
        return -1;
    }
    
    frame = av_frame_alloc();
    frame_RGB = av_frame_alloc();
    printf("width: %d, height: %d, pix_fmt %d \n",
           codec_context->width,
           codec_context->height,
           codec_context->pix_fmt);
    img_convert_context = sws_getContext(codec_context->width,
                                         codec_context->height,
                                         codec_context->pix_fmt,
                                         codec_context->width,
                                         codec_context->height,
                                         AV_PIX_FMT_RGB24,
                                         SWS_BICUBIC,
                                         nullptr,
                                         nullptr,
                                         nullptr);
    num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                         codec_context->width,
                                         codec_context->height,
                                         1);
    if (num_bytes < 0) {
        printf("av_image_get_ buffer_size failed \n");
        return num_bytes;
    }
    
    out_buffer = (uint8_t*) av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(frame_RGB->data, frame_RGB->linesize, out_buffer,
                         AV_PIX_FMT_RGB24, codec_context->width,
                         codec_context->height, 1);
    
    
    int y_size = codec_context->width * codec_context->height;
    
    AVPacket* video_packet = av_packet_alloc();
// 为AVPacket分配固定大小的payload内存
    av_new_packet(video_packet, y_size);

// 输出视频信息
    av_dump_format(format_context, 0, file_path, 0);
    
    int index = 0;
    int read_end = 0;
    
    while (true) {
        if (read_end == 1) {
            break;
        }
        
        ret = av_read_frame(format_context, video_packet);
        
        if (video_packet->stream_index != video_stream) {
            av_packet_unref(video_packet);
            continue;
        }
        
        if (ret == AVERROR_EOF) {
            avcodec_send_packet(codec_context, video_packet);
            av_packet_unref(video_packet);
            
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
                    // 解码出YUV
                    sws_scale(img_convert_context, (const uint8_t* const*) frame->data,
                              frame->linesize, 0, codec_context->height,
                              frame_RGB->data, frame_RGB->linesize);
                    save_frame(frame_RGB, codec_context->width, codec_context->height,
                               index++);
                    if (index > 50) {
                        return 0;
                    }
                }
                else {
                    printf("other error \n");
                    return -1;
                }
            }
        }
        else if (ret == 0) {
            retry:
            if (avcodec_send_packet(codec_context, video_packet) == AVERROR(EAGAIN)) {
                printf("Receive_frame and send_packet both returned EAGAIN,"
                       "which is an API violation. \n");
                //这里可以考虑休眠 0.1 秒，返回 EAGAIN 通常是 ffmpeg 的内部 api 有bug
                goto retry;
            }
            else {
                // 释放av_packet里面的编码数据
                av_packet_unref(video_packet);
                // 循环从解码器中读取数据, 直到没有数据可读
                for(;;) {
                    // 读取AVFrame
                    ret = avcodec_receive_frame(codec_context, frame);
                    if (AVERROR(EAGAIN) == ret) {
                        break;
                    }
                    else if (AVERROR_EOF == ret) {
                        read_end = 1;
                        break;
                    }
                    else if (ret >= 0) {
                        // 解码出第一帧YUV, 打印一些信息
                        sws_scale(img_convert_context, (const uint8_t* const*)frame->data,
                                  frame->linesize, 0, codec_context->height,
                                  frame_RGB->data, frame_RGB->linesize);
                        save_frame(frame_RGB, codec_context->width, codec_context->height,
                                   index++);
                        if (index > 50) {
                            return 0;
                        }
                    }
                    else {
                        printf("other fail \n");
                        return ret;
                    }
                }
            }
        }
    }
    av_packet_free(&video_packet);
    av_free(out_buffer);
    av_free(frame_RGB);
    avcodec_close(codec_context);
    avformat_close_input(&format_context);
}

int main() {
    get_video_img();
}
