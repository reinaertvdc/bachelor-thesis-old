#ifndef BACHELORTHESIS_FRAMEPROCESSOR_H
#define BACHELORTHESIS_FRAMEPROCESSOR_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

class FrameProcessor {
public:
    void process(cv::Mat &frame, cv::Mat &gray);

private:
    const cv::Scalar COLOR = cv::Scalar(255, 0, 0);

    const int MIN_ARC_LENGTH = 400;
    const int MAX_ARC_LENGTH = 2000;

    const double APPROX_CURVE_ACCURACY_FACTOR = 0.1;
};


#endif //BACHELORTHESIS_FRAMEPROCESSOR_H
