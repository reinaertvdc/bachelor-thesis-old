#ifndef BACHELORTHESIS_FRAMEPROCESSOR_H
#define BACHELORTHESIS_FRAMEPROCESSOR_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

class FrameProcessor {
public:
    void process(cv::Mat &frame, cv::Mat &gray);

private:
    class Tag {
    public:
        Tag(const cv::Vec4b color) : color(color) {};

        const cv::Vec4b &getColor() const;

    private:
        const cv::Vec4b color;
    };

    static const int THRESHOLD = 40;
    static const int THRESHOLD_MAX = 255;

    static const int NUM_PRIMARY_COLORS = 3;
    static const int MAX_PRIMARY_COLOR_VALUE = 255;

    static const unsigned char COLOR_THRESHOLD = 150;
    static const unsigned char MIN_MAX_PRIMARY_COLOR_VALUE = 32;

    static const int MIN_ARC_LENGTH = 400;
    static const int MAX_ARC_LENGTH = 2000;

    static constexpr double APPROX_CURVE_ACCURACY_FACTOR = 0.1;

    cv::Mat *frame;

    Tag tagBlue = Tag(cv::Vec4b(255, 0, 0));
    Tag tagGreen = Tag(cv::Vec4b(0, 255, 0));
    Tag tagRed = Tag(cv::Vec4b(0, 0, 255));

    const std::vector<Tag *> tags = {&tagBlue, &tagGreen, &tagRed};

    static inline void getContours(cv::Mat &gray,
                                   std::vector<std::vector<cv::Point> > &contours,
                                   std::vector<cv::Vec4i> &hierarchy);

    static inline cv::Point2i getContourCentroid(const std::vector<cv::Point> &contour);

    inline Tag *getTagByColor(const cv::Vec4b &color) const;

    static inline cv::Vec4b getMaximizedColor(const cv::Vec4b &color);
};


#endif //BACHELORTHESIS_FRAMEPROCESSOR_H
