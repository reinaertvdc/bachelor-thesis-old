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

        /*if (contourLength >= MIN_FINGER_TAG_ARC_LENGTH &&
                contourLength <= MAX_FINGER_TAG_ARC_LENGTH) {
            cv::Point2i centroid = getContourCentroid(contours[contourIndex]);

            const cv::Vec4b color = frame.at<cv::Vec4b>(centroid);

            if (color.val[0] > color.val[2] && color.val[1] > color.val[2]) {
                cv::circle(frame, centroid, 16, cv::Scalar(255, 255, 127), 8);
            }
        }*/

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

    transformDest[0] = getClosestPoint(*tags[0]->getPoints(), (*tags[2]->getPoints())[0]);
    transformDest[1] = getClosestPoint(*tags[1]->getPoints(), tagsCenter);
    transformDest[2] = getClosestPoint(*tags[2]->getPoints(), (*tags[0]->getPoints())[0]);

    /*for (int tagIndex = 0; tagIndex < tags.size(); tagIndex++) {
        transformDest[tagIndex] = getClosestPoint(*tags[tagIndex]->getPoints(), tagsCenter);
    }*/

    transform = cv::getAffineTransform(transformSource, transformDest);

    cv::transform(panel, panelProjected, transform);

    int panelPointIndex = 0;
    for (int switchIndex = 0; switchIndex < NUM_SWITCHES; switchIndex++) {
        getCenter(panelProjected[panelPointIndex], panelProjected[panelPointIndex + 2],
                  switchTestPoints[0]);

        for (int i = 0; i < 4; i++) {
            getCenter(panelProjected[panelPointIndex + i], switchTestPoints[0],
                      switchTestPoints[i + 1]);
        }

        bool updated = false;
        for (int i = 0; i < switchTestPoints.size(); i++) {
            const cv::Vec4b &color = frame.at<cv::Vec4b>(switchTestPoints[i]);

            if (color.val[0] > 150 && color.val[1] > 150 && color.val[2] < 100 && !updated) {
                activeSwitchIndex = switchIndex;
                activeSwitchAge = 0;

                switchIsPressed = false;
                updated = true;
            } else if (color.val[0] > 150 && color.val[1] < 100 && color.val[2] < 100) {
                if (!updated) {
                    previousPressedSwitchIndex = activeSwitchIndex;
                    activeSwitchAge = 0;
                }

                activeSwitchIndex = switchIndex;

                if (pressInterval >= MIN_PRESS_INTERVAL ||
                    previousPressedSwitchIndex != activeSwitchIndex) {
                    switchStates[switchIndex] = !switchStates[switchIndex];
                }

                pressInterval = 0;

                switchIsPressed = true;
                switchPressedAge = 0;

                break;
            }
        }

        if (!switchStates[switchIndex]) {
            for (int i = 0; i < 4; i++) {
                int nextPanelPointIndex = panelPointIndex + 1;

                if ((nextPanelPointIndex % 4) == 0) {
                    nextPanelPointIndex -= 4;
                }

                cv::line(frame, panelProjected[panelPointIndex],
                         panelProjected[nextPanelPointIndex],
                         SWITCH_COLOR, 8);

                panelPointIndex++;
            }
        } else {
            panelPointIndex += 4;
        }
    }

    if (pressInterval < MIN_PRESS_INTERVAL) {
        pressInterval++;
    }

    panelPointIndex = 0;
    for (int switchIndex = 0; switchIndex < NUM_SWITCHES; switchIndex++) {
        if (switchStates[switchIndex]) {
            for (int i = 0; i < 4; i++) {
                int nextPanelPointIndex = panelPointIndex + 1;

                if ((nextPanelPointIndex % 4) == 0) {
                    nextPanelPointIndex -= 4;
                }

                cv::line(frame, panelProjected[panelPointIndex],
                         panelProjected[nextPanelPointIndex],
                         SWITCH_COLOR_ON, 8);

                panelPointIndex++;
            }
        } else {
            panelPointIndex += 4;
        }
    }

    if (activeSwitchIndex >= 0) {
        /*const cv::Scalar &drawColor = switchIsPressed ? SWITCH_COLOR_PRESSED : SWITCH_COLOR_ACTIVE;

        for (int i = activeSwitchIndex * 4; i < (activeSwitchIndex * 4) + 4; i++) {
            int nextPanelPointIndex = i + 1;

            if ((nextPanelPointIndex % 4) == 0) {
                nextPanelPointIndex -= 4;
            }

            cv::line(frame, panelProjected[i],
                     panelProjected[nextPanelPointIndex],
                     drawColor, 8);
        }*/

        if (switchIsPressed) {
            switchPressedAge++;

            if (switchPressedAge > MAX_SWITCH_PRESSED_AGE) {
                activeSwitchIndex = -1;
            }
        } else {
            activeSwitchAge++;

            if (activeSwitchAge > MAX_ACTIVE_SWITCH_AGE) {
                activeSwitchIndex = -1;
            }
        }
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

cv::Point2i FrameProcessor::getClosestPoint(const std::vector<cv::Point2i> &points,
                                            const cv::Point2i &point) {
    const cv::Point2i *closestPoint = &points[0];
    double closestPointDistance = cv::norm(points[0] - point);

    for (int pointIndex = 1; pointIndex < points.size(); pointIndex++) {
        double distance = cv::norm(points[pointIndex] - point);

        if (distance < closestPointDistance) {
            closestPoint = &points[pointIndex];
            closestPointDistance = distance;
        }
    }

    return *closestPoint;
}

cv::Point2i FrameProcessor::getFurthestPoint(const std::vector<cv::Point2i> &points,
                                             const cv::Point2i &point) {
    const cv::Point2i *furthestPoint = &points[0];
    double furthestPointDistance = cv::norm(points[0] - point);

    for (int pointIndex = 1; pointIndex < points.size(); pointIndex++) {
        double distance = cv::norm(points[pointIndex] - point);

        if (distance > furthestPointDistance) {
            furthestPoint = &points[pointIndex];
            furthestPointDistance = distance;
        }
    }

    return *furthestPoint;
}

void FrameProcessor::getCenter(const cv::Point2i &point1, const cv::Point2i &point2,
                               cv::Point2i &result) {
    result.x = std::min(point1.x, point2.x) + (std::abs(point1.x - point2.x) / 2);
    result.y = std::min(point1.y, point2.y) + (std::abs(point1.y - point2.y) / 2);
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
