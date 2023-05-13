#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QThread>
#include <QImage>
#include <utility>


class VideoPlayer : public QThread {
    Q_OBJECT
public:
    explicit VideoPlayer();
    ~VideoPlayer() override;
    
    void set_file_name(QString path) {
        filename_ = path;
    }
    
    void start_play();
    
protected:
    void run() override;
    
private:
    QString filename_;
    
//    void save_frame(AVFrame* frame, int width, int height, int index);
signals:
    void signal_get_frame(QImage); // 每获取一帧就发送此信号
};

#endif
