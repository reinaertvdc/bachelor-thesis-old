#include "FrameProcessor.h"

void FrameProcessor::process(cv::Mat &frame, cv::Mat &gray) {
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::threshold(gray, gray, THRESHOLD, THRESHOLD_MAX, cv::THRESH_BINARY);

    findContours(gray, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        double contourLength = cv::arcLength(contours[i], true);

        if (contourLength < MIN_ARC_LENGTH || contourLength > MAX_ARC_LENGTH) {
            continue;
        }

        std::vector<cv::Point> contourApproximation;
        cv::approxPolyDP(contours[i], contourApproximation,
                         APPROX_CURVE_ACCURACY_FACTOR * contourLength, true);

        if (contourApproximation.size() != 4) {
            continue;
        }

        cv::polylines(frame, contourApproximation, true, COLOR);
    }
}
