#include <glog/logging.h>
#include <rmconfig.h>
#include <rmserial.h>
#include <rmtime.h>
#include <runtime.h>
#include <armor_finder/armor_finder.h>
#include <csignal>
#include <opencv2/opencv.hpp>

using namespace std;
//检测关闭程序键盘中断
static volatile int keepRunning = 1;

void sig_handler(int sig) {
    if (sig == SIGINT) {
        keepRunning = 0;
    }
}

serial::Serial* active_port;
//程序运行时钟
RmTime rmTime;
//程序运行时信息
RmRunTime* runtime_up;
RmRunTime* runtime_down;
//程序串口封装
RmSerial* serial_all;
//装甲板击打主程序
ArmorFinder* armor_finder_up;
ArmorFinder* armor_finder_down;

static void OnInit(const char* cmd) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(cmd);

    rmTime.init();
    runtime_up = new RmRunTime("rmconfig_up.json");
    runtime_down = new RmRunTime("rmconfig_down.json");
    if (!runtime_up->status_ok || !runtime_down->status_ok) {
        keepRunning = 0;
        return;
    }

    active_port = new serial::Serial(runtime_up->config->uart_port, 115200,
                                     serial::Timeout::simpleTimeout(1000));

    serial_all = new RmSerial(runtime_up, active_port);

    armor_finder_up = new ArmorFinder(runtime_up, serial_all);
    armor_finder_down = new ArmorFinder(runtime_down, serial_all);
}

static void OnClose() {
    if (runtime_up != nullptr) delete runtime_up;
    if (runtime_down != nullptr) delete runtime_down;
    if (active_port != nullptr) delete active_port;
}

void Run(RmRunTime* runtime, ArmorFinder* armor_finder) {
    if (runtime->config->use_video) {
        if (!runtime->video->read(runtime->src)) {
            runtime->status_ok = false;
            return;
        }
        cv::resize(runtime->src, runtime->src, cv::Size(640, 360));
    } else {
        runtime->cam->read(runtime->src);
        // config.camConfig.undistort(src);
    }
    if (runtime->config->show_origin) {
        cv::imshow(runtime->config->configPath+":origin", runtime->src);
    }
    armor_finder->run(runtime->src);
}
int main(int argc, char** argv) {
    signal(SIGINT, sig_handler);
    OnInit(argv[0]);

    while (keepRunning) {
        serial_all->update_config();
        std::thread up_thread(Run,runtime_up,armor_finder_up);
        std::thread down_thread(Run,runtime_down,armor_finder_down);

        up_thread.join();
        down_thread.join();

        if (!runtime_up->status_ok || !runtime_down->status_ok) break;

        if (runtime_up->config->has_show || runtime_down->config->has_show)
            cv::waitKey(1);
    }
    LOG(INFO) << "exiting...";
    OnClose();
    return 0;
}