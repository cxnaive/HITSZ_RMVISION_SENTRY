#include <armor_finder/armor_finder.h>
#include <rmserial.h>
#include <runtime.h>

#include <cmath>

bool ArmorFinder::sendTarget(RmSerial* serial, double x, double y, double z,
                             uint16_t shoot_delay) {
    static short x_tmp, y_tmp, z_tmp;
    // uint8_t buff[10];
    SendData data;
    data.start_flag = 's';
    data.x = static_cast<float>(x);
    data.y = static_cast<float>(y);
    data.z = static_cast<float>(z);
    data.u = shoot_delay;

    data.end_flag = 'e';
    if (config->log_send_target)
        LOG(INFO) << runtime->config->configPath << ":Target:" << data.x << " "
                  << data.y;
    return serial->send_data(data);
    // buff[0] = 's';
    // buff[1] = static_cast<char>((x_tmp >> 8) & 0xFF);
    // buff[2] = static_cast<char>((x_tmp >> 0) & 0xFF);
    // buff[3] = static_cast<char>((y_tmp >> 8) & 0xFF);
    // buff[4] = static_cast<char>((y_tmp >> 0) & 0xFF);
    // buff[5] = static_cast<char>((z_tmp >> 8) & 0xFF);
    // buff[6] = static_cast<char>((z_tmp >> 0) & 0xFF);
    // buff[7] = static_cast<char>((shoot_delay >> 8) & 0xFF);
    // buff[8] = static_cast<char>((shoot_delay >> 0) & 0xFF);
    // buff[9] = 'e';
    // cout << (short)(buff[3]<<8 | buff[4]) << endl;
    // return serial.send_data(buff, sizeof(buff));
}
bool ArmorFinder::sendBoxPosition(uint16_t shoot_delay) {
    if (shoot_delay) {
        LOG(INFO) << "next box" << shoot_delay << " ms";
    }

    cv::Point3d trans;
    double dist;
    double yaw, pitch;
    if (config->use_pnp) {
        auto pnp_result = target_box.armorSolvePnP(runtime);
        trans = pnp_result.second;
        dist = trans.z;
        yaw = atan(trans.x / trans.z) * 180 / PI;
        pitch = atan(trans.y / trans.z) * 180 / PI;
    } else {
        dist = target_box.getBoxDistance(runtime);
        cv::Point2f center = target_box.getCenter();
        yaw = atan((center.x - config->IMAGE_CENTER_X + config->ARMOR_DELTA_X) /
                   config->camConfig.fx) *
              180 / PI;
        pitch =
            atan((center.y - config->IMAGE_CENTER_Y) / config->camConfig.fy) *
            180 / PI;
    }
    double dpitch =
        config->ARMOR_PITCH_DELTA_K * dist + config->ARMOR_PITCH_DELTA_B;
    pitch -= dpitch ;
    last_dpitch = dpitch;

    double error = sqrt(pitch * pitch + yaw * yaw);

    if (config->log_send_target) {
        LOG(INFO) << "YAW: " << yaw << " PITCH: " << pitch;
    }
    // calc_fps
    ++fps_cnt;
    double now_time = rmTime.seconds();
    if (now_time - last_time > 2) {
        LOG(INFO) << runtime->config->configPath
                  << ":Armor fps: " << fps_cnt / (now_time - last_time);
        last_time = now_time;
        fps_cnt = 0;
    }
    //??????PID??????
    yaw = YawPID.updateError(yaw);
    pitch = PitchPID.updateError(pitch);
    //LOG(INFO) << "error:" << error;
    if (error < config->SHOT_THRESHOLD) {
        return sendTarget(serial, yaw, -pitch,
                          dist,runtime->config->SERIAL_OFFSET + 2);
    } else
        return sendTarget(serial, yaw, -pitch,
                          dist,runtime->config->SERIAL_OFFSET + 1);
}

bool ArmorFinder::sendLostBox() {
    PitchPID.clear();
    YawPID.clear();
    return sendTarget(serial, 0, 0, 0, runtime->config->SERIAL_OFFSET);
}