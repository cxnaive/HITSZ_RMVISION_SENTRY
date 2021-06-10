#include <runtime.h>

#include <cmath>
#include <map>

double getPointLength(const cv::Point2f& p) {
    return sqrt(p.x * p.x + p.y * p.y);
}
cudaVideoWriter initVideoWriter(const std::string& filename_prefix) {
    std::string file_name = filename_prefix + ".avi";
    cudaVideoWriter video =
        cv::cudacodec::createVideoWriter(file_name, cv::Size(640, 640), 60);
}
std::map<std::string, cudaVideoWriter> video_writers;
cudaVideoWriter getVideoWriter(std::string prefix) {
    auto iter = video_writers.find(prefix);
    if (iter != video_writers.end()) return iter->second;
    cudaVideoWriter now = initVideoWriter("video/" + prefix);
    video_writers[prefix] = now;
    return now;
}
void saveVideos(cv::Mat& img, std::string prefix) {
    if (img.empty()) return;
    cudaVideoWriter writer = getVideoWriter(prefix);
    writer->write(img);
}

RmRunTime::RmRunTime(std::string configPath) {
    config = new RmConfig(configPath);
    if (config->use_video) {
        video = new VideoWrapper(config->video_path);
        video->init();
    } else {
        cam = new Camera(config->camera_sn, config->camConfig);
        cam->init();
        if (!cam->init_is_successful()) {
            LOG(ERROR) << "Camera " + configPath + " Init Failed!";
            status_ok = false;
            return;
        }
        cam->setParam(config->ARMOR_CAMERA_EXPOSURE, config->ARMOR_CAMERA_GAIN);
        cam->start();
    }
    status_ok = true;
}

RmRunTime::~RmRunTime() {
    //config->write_to_file();
    if (cam != nullptr) delete cam;
    if (video != nullptr) delete video;
    if (config != nullptr) delete config;
}
