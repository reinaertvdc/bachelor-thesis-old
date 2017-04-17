#include "FrameProcessor.h"

void FrameProcessor::process(cv::Mat &frame, cv::Mat &gray) {
    this->frame = &frame;

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    getContours(gray, contours, hierarchy);

    for (int contourIndex = 0; contourIndex < contours.size(); contourIndex++) {
        const double contourLength = cv::arcLength(contours[contourIndex], true);

        if (contourLength < MIN_ARC_LENGTH || contourLength > MAX_ARC_LENGTH) {
            continue;
        }

        std::vector<cv::Point> contourApproximation;
        cv::approxPolyDP(contours[contourIndex], contourApproximation,
                         APPROX_CURVE_ACCURACY_FACTOR * contourLength, true);

        if (contourApproximation.size() != 4) {
            continue;
        }

        cv::Point2i centroid = getContourCentroid(contourApproximation);

        const cv::Vec4b color = frame.at<cv::Vec4b>(centroid);

        Tag *tag = getTagByColor(color);

        if (tag == nullptr) {
            continue;
        }

        cv::polylines(frame, contourApproximation, true, tag->getColor(), 8);
    }
}

inline void FrameProcessor::getContours(cv::Mat &gray,
                                        std::vector<std::vector<cv::Point> > &contours,
                                        std::vector<cv::Vec4i> &hierarchy) {
    cv::threshold(gray, gray, THRESHOLD, THRESHOLD_MAX, cv::THRESH_BINARY);
    findContours(gray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
}

inline cv::Point2i
FrameProcessor::getContourCentroid(const std::vector<cv::Point> &contour) {
    cv::Moments moments = cv::moments(contour);

    return cv::Point2i((int) round(moments.m10 / moments.m00),
                       (int) round(moments.m01 / moments.m00));
}

FrameProcessor::Tag *FrameProcessor::getTagByColor(const cv::Vec4b &color) const {
    cv::Vec4b maximizedColor = getMaximizedColor(color);

    for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        Tag &tag = *tags[tagIndex];
        const cv::Vec4b &tagColor = tag.getColor();
        bool isMatch = true;

        for (int primaryColorIndex = 0;
             primaryColorIndex < NUM_PRIMARY_COLORS; primaryColorIndex++) {
            if (abs(tagColor[primaryColorIndex] - maximizedColor[primaryColorIndex])
                > COLOR_THRESHOLD) {
                isMatch = false;
                break;
            }
        }

        if (isMatch) {
            return tags[tagIndex];
        }
    }

    return nullptr;
}

cv::Vec4b FrameProcessor::getMaximizedColor(const cv::Vec4b &color) {
    cv::Vec4b maximizedColor;
    int highestPrimaryColorValue = 0;

    for (int primaryColorIndex = 0; primaryColorIndex < NUM_PRIMARY_COLORS; primaryColorIndex++) {
        highestPrimaryColorValue = highestPrimaryColorValue > color[primaryColorIndex] ?
                                   highestPrimaryColorValue : color[primaryColorIndex];
    }

    highestPrimaryColorValue = std::max(highestPrimaryColorValue, 1);

    double scaleFactor = highestPrimaryColorValue >= MIN_MAX_PRIMARY_COLOR_VALUE ?
                         MAX_PRIMARY_COLOR_VALUE / highestPrimaryColorValue : 0;

    for (int primaryColorIndex = 0; primaryColorIndex < NUM_PRIMARY_COLORS; primaryColorIndex++) {
        maximizedColor[primaryColorIndex] =
                (unsigned char) (color[primaryColorIndex] * scaleFactor);
    }

    return maximizedColor;
}

const cv::Vec4b &FrameProcessor::Tag::getColor() const {
    return color;
}
