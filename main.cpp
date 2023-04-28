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
    
    return QApplication::exec();
}
