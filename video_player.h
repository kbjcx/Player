#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QThread>
extern "C" {
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/pixfmt.h"
    #include "libavcodec/avcodec.h"
}

class VideoPlayer : public QThread {
    Q_OBJECT
public:
    VideoPlayer();
    ~VideoPlayer() override;
    
protected:
    void run() override;
    
    void save_frame(AVFrame* frame, int width, int height, int index);
};

#endif
