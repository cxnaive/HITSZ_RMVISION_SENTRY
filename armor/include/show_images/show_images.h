//
// Created by xinyang on 19-3-27.
//

#ifndef _SHOW_IMAGES_H_
#define _SHOW_IMAGES_H_

#include <armor_finder/armor_finder.h>

#include <opencv2/core.hpp>

//
void showNetBoxes(std::string windows_name, const cv::Mat &src,
                  const std::vector<ArmorInfo> &armors);
void showArmorBox(std::string windows_name, const cv::Mat &src,
                  const ArmorBox &box, RmRunTime *runtime);
void showLightBlobs(std::string windows_name, const cv::Mat &src,
                    const LightBlobs &light_blobs);
void showTrackSearchingPos(std::string window_names, const cv::Mat &src,
                           const cv::Rect2d pos);

#endif /* _SHOW_IMAGES_H_ */
