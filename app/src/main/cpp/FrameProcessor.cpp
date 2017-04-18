#include "FrameProcessor.h"

void FrameProcessor::process(cv::Mat &frame, cv::Mat &gray) {
    this->frame = &frame;

    for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        tags[tagIndex]->notifyNewFrame();
    }

    std::vector<std::vector<cv::Point2i> > contours;
    std::vector<cv::Vec4i> hierarchy;
    getContours(gray, contours, hierarchy);

    for (int contourIndex = 0; contourIndex < contours.size(); contourIndex++) {
        const double contourLength = cv::arcLength(contours[contourIndex], true);

        if (contourLength < MIN_ARC_LENGTH || contourLength > MAX_ARC_LENGTH) {
            continue;
        }

        std::vector<cv::Point2i> contourApproximation;
        cv::approxPolyDP(contours[contourIndex], contourApproximation,
                         APPROX_CURVE_ACCURACY_FACTOR * contourLength, true);

        if (contourApproximation.size() != NUM_TAG_POINTS) {
            continue;
        }

        cv::Point2i centroid = getContourCentroid(contourApproximation);

        const cv::Vec4b color = frame.at<cv::Vec4b>(centroid);

        Tag *tag = getTagByColor(color);

        if (tag == nullptr) {
            continue;
        }

        tag->setPoints(contourApproximation);
    }

    for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        tags[tagIndex]->draw();
    }

    bool tagsMissing = false;
    for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        const std::vector<cv::Point2i> *tagContour = tags[tagIndex]->getPoints();

        if (tagContour == nullptr) {
            tagsMissing = true;
            break;
        }

        tagsContour[tagIndex] = getContourCentroid(*tagContour);

        tags[tagIndex]->draw();
    }

    if (tagsMissing) {
        return;
    }

    tagsCenter = (tagsContour[0] + tagsContour[2]) * 0.5;

    cv::circle(frame, tagsCenter, 16, cv::Scalar(255, 255, 0), 8);

    for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        panel[tagIndex] = getFurthestPoint(*tags[tagIndex]->getPoints(), tagsCenter);
    }

    panel[3].x = panel[1].x - (panel[1].x - (std::min(panel[0].x, panel[2].x) +
                                             (abs(panel[0].x - panel[2].x) / 2))) * 2;
    panel[3].y = panel[1].y - (panel[1].y - (std::min(panel[0].y, panel[2].y) +
                                             (abs(panel[0].y - panel[2].y) / 2))) * 2;

    for (int panelPointIndex = 0; panelPointIndex < panel.size(); panelPointIndex++) {
        cv::line(frame, panel[panelPointIndex], panel[(panelPointIndex + 1) % panel.size()],
                 cv::Scalar(0, 255, 255), 8);
    }
}

inline void FrameProcessor::getContours(cv::Mat &gray,
                                        std::vector<std::vector<cv::Point2i> > &contours,
                                        std::vector<cv::Vec4i> &hierarchy) {
    cv::threshold(gray, gray, THRESHOLD, THRESHOLD_MAX, cv::THRESH_BINARY);
    findContours(gray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
}

inline cv::Point2i
FrameProcessor::getContourCentroid(const std::vector<cv::Point2i> &contour) {
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

cv::Point2i FrameProcessor::getFurthestPoint(const std::vector<cv::Point2i> &points,
                                             const cv::Point2i &point) {
    const cv::Point2i *closestPoint = &points[0];
    double closestPointDistance = cv::norm(points[0] - point);

    for (int pointIndex = 1; pointIndex < points.size(); pointIndex++) {
        double distance = cv::norm(points[pointIndex] - point);

        if (distance > closestPointDistance) {
            closestPoint = &points[pointIndex];
            closestPointDistance = distance;
        }
    }

    return *closestPoint;
}

const cv::Vec4b &FrameProcessor::Tag::getColor() const {
    return color;
}

void FrameProcessor::Tag::notifyNewFrame() {
    if (isSet) {
        if (age == MAX_AGE) {
            isSet = false;
        } else {
            age++;
        }
    }
}

void FrameProcessor::Tag::draw() const {
    if (isSet) {
        cv::polylines(*(frameProcessor.frame), points, true, color, DRAWING_THICKNESS);
    }
}

void FrameProcessor::Tag::setPoints(const std::vector<cv::Point2i> &points) {
    for (int pointIndex = 0; pointIndex < NUM_TAG_POINTS; pointIndex++) {
        this->points[pointIndex] = points[pointIndex];
    }

    isSet = true;
    age = 0;
}

const std::vector<cv::Point2i> *FrameProcessor::Tag::getPoints() const {
    if (isSet) {
        return &points;
    } else {
        return nullptr;
    }
}
