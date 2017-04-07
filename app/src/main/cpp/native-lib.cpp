#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

extern "C"
JNIEXPORT void JNICALL
Java_me_reinaertvandecruys_bachelorthesis_MainActivity_process(
        JNIEnv *env,
        jobject thiz,
        jlong frameAddr,
        jlong grayAddr) {
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Scalar color = cv::Scalar(255, 0, 0);

    cv::Mat &frame = *(cv::Mat *) frameAddr;
    cv::Mat &gray = *(cv::Mat *) grayAddr;

    cv::threshold(gray, gray, 40, 255, cv::THRESH_BINARY);

    findContours(gray, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        drawContours(frame, contours, i, color, 2, 8, hierarchy, 0);
    }
}
