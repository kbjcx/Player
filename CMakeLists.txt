cmake_minimum_required(VERSION 3.21)
project(player)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "F:/QT/5.12.12/mingw73_64")

find_package(Qt5 COMPONENTS
             Core
             Gui
             Widgets
             REQUIRED)

include_directories(E:/CPP/ffmpeg4.4.4/include)
link_directories(E:/CPP/ffmpeg4.4.4/lib)

add_executable(player main.cpp widget.h widget.cpp video_player.h video_player.cpp)
target_link_libraries(player
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets
                      )
target_link_libraries(player
                        avcodec
                        avdevice
                        avfilter
                        avformat
                        avutil
                        postproc
                        swresample
                        swscale
)
