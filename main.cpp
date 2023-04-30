#include <QApplication>
#include <QPushButton>
#include "widget.h"
#include <QDebug>
extern "C"{
#include <libavcodec//avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
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

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    int version =avformat_version();
    qDebug() << "version:" << version << endl;
    
    char* file_path = "E:/CPP/QT/Source/test.mp4";
    
    AVFormatContext* format_context;
    AVCodecContext* codec_context;
    AVCodec* codec;
    AVFrame* frame, frame_RGB;
    AVPacket* packet;
    uint8_t* out_buffer;
    
    static struct SwsContext* img_convert_context;
    int video_stream, i, num_bytes;
    int ret, got_picture;
    
    // av_register_all();
    
    // allocate an AVFormatContext
    format_context = avformat_alloc_context();
    
    if (avformat_open_input(&format_context, file_path, nullptr, nullptr) != 0) {
        printf("can't open the file");
        return -1;
    }
    
    if (avformat_find_stream_info(format_context, nullptr) < 0) {
        printf("can't find stream information");
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
        printf("didn't find a video stream");
        return -1;
    }
    
    // 查找解码器
    codec_context = format_context->streams[video_stream]->codecpar;
    
    
    
    
    return QApplication::exec();
}
