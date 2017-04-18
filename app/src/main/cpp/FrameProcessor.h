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
        Tag(FrameProcessor &frameProcessor, const cv::Vec4b color, bool isTopTag, bool isLeftTag) :
                frameProcessor(frameProcessor), color(color),
                isTopTag(isTopTag), isLeftTag(isLeftTag) {};

        const cv::Vec4b &getColor() const;

        void notifyNewFrame();

        void draw() const;

        void setPoints(const std::vector<cv::Point2i> &points);

        const std::vector<cv::Point2i> *getPoints() const;

    private:
        static const int MAX_AGE = 4;

        static const int DRAWING_THICKNESS = 8;

        int age = 0;
        bool isSet = false;
        const bool isTopTag;
        const bool isLeftTag;
        const FrameProcessor &frameProcessor;
        const cv::Vec4b color;
        std::vector<cv::Point2i> points = {cv::Point2i(0, 0), cv::Point2i(0, 0),
                                           cv::Point2i(0, 0), cv::Point2i(0, 0)};
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

    static const int NUM_TAG_POINTS = 4;

    cv::Mat *frame;

    std::vector<cv::Point2i> panel = {cv::Point2i(0, 0), cv::Point2i(0, 0),
                                      cv::Point2i(0, 0), cv::Point2i(0, 0)};

    bool panelIsSet = false;

    Tag tagBlue = Tag(*this, cv::Vec4b(255, 0, 0), true, true);
    Tag tagGreen = Tag(*this, cv::Vec4b(0, 255, 0), true, false);
    Tag tagRed = Tag(*this, cv::Vec4b(0, 0, 255), false, true);

    const std::vector<Tag *> tags = {&tagBlue, &tagGreen, &tagRed};
    std::vector<cv::Point2i> tagsContour = {cv::Point2i(0, 0),
                                            cv::Point2i(0, 0),
                                            cv::Point2i(0, 0)};
    cv::Point2i tagsCenter;

    static inline void getContours(cv::Mat &gray,
                                   std::vector<std::vector<cv::Point> > &contours,
                                   std::vector<cv::Vec4i> &hierarchy);

    static inline cv::Point2i getContourCentroid(const std::vector<cv::Point> &contour);

    inline Tag *getTagByColor(const cv::Vec4b &color) const;

    static inline cv::Vec4b getMaximizedColor(const cv::Vec4b &color);

    static inline cv::Point2i getFurthestPoint(const std::vector<cv::Point2i> &points,
                                               const cv::Point2i &point);
};


#endif //BACHELORTHESIS_FRAMEPROCESSOR_H
