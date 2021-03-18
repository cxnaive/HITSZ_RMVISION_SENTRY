#ifndef _H_RUNTIME_
#define _H_RUNTIME_

#include <RoundQueue.h>
#include <camera/cam_wrapper.h>
#include <camera/video_wrapper.h>
#include <constants.h>
#include <glog/logging.h>
#include <rmconfig.h>
#include <rmtime.h>
#include <serial/serial.h>

#include <opencv2/cudacodec.hpp>
#include <opencv2/opencv.hpp>
typedef cv::Ptr<cv::cudacodec::VideoWriter> cudaVideoWriter;

void saveVideos(cv::Mat& img, std::string prefix);
double getPointLength(const cv::Point2f& p);
extern serial::Serial* active_port;

extern RmTime rmTime;

class RmRunTime {
   public:
    bool status_ok;
    RmConfig* config;
    Camera* cam;
    VideoWrapper* video;
    cv::Mat src;
    RmRunTime(std::string configPath);
    ~RmRunTime();
};
#endif
