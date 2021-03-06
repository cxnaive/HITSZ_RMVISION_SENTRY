#ifndef H_RMSERIAL
#define H_RMSERIAL
#include <datatypes.h>
#include <rmconfig.h>
#include <serial/serial.h>
#include <runtime.h>
#include <mutex>

struct McuConfig {
    uint8_t state;        // 当前状态，自瞄-大符-小符
    uint8_t anti_top;     // 是否为反陀螺模式
    uint8_t enemy_color;  // 敌方颜色
    float curr_yaw;       // 当前云台yaw角度
    float curr_pitch;     // 当前云台pitch角
    float bullet_speed;   // 当前弹速
    int delta_x;          // 能量机关x轴补偿量
    int delta_y;          // 能量机关y轴补偿量
};

class RmSerial {
   public:
    RmRunTime* runtime;
    serial::Serial* active_port;
    std::mutex receive_mtx;
    McuConfig receive_config_data;
    RmSerial(RmRunTime* _runtime, serial::Serial* active_port);
    ~RmSerial();
    bool init_success;
    bool thread_running;
    bool isConnected() { return active_port->isOpen(); }
    bool send_data(const SendData& data);
    bool send_data(uint8_t* data, size_t size);
    void update_config();
    void start_thread();
    void stop_thread();
};

#endif