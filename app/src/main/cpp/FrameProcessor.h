#ifndef BACHELORTHESIS_FRAMEPROCESSOR_H
#define BACHELORTHESIS_FRAMEPROCESSOR_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

class FrameProcessor {
public:
    FrameProcessor();

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
        static const int MAX_AGE = 8;

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

    static const int THRESHOLD = 64;
    static const int THRESHOLD_MAX = 255;

    static const int NUM_PRIMARY_COLORS = 3;
    static const int MAX_PRIMARY_COLOR_VALUE = 255;

    static const unsigned char COLOR_THRESHOLD = 150;
    static const unsigned char MIN_MAX_PRIMARY_COLOR_VALUE = 32;

    static const int MIN_ARC_LENGTH = 400;
    static const int MAX_ARC_LENGTH = 2000;

    static const int MIN_FINGER_TAG_ARC_LENGTH = 50;
    static const int MAX_FINGER_TAG_ARC_LENGTH = 200;

    static constexpr double APPROX_CURVE_ACCURACY_FACTOR = 0.1;

    static const int NUM_TAG_POINTS = 4;

    cv::Scalar SWITCH_COLOR = cv::Scalar(0, 255, 255);
    cv::Scalar SWITCH_COLOR_ON = cv::Scalar(255, 255, 0);
    cv::Scalar SWITCH_COLOR_ACTIVE = cv::Scalar(255, 0, 0);
    cv::Scalar SWITCH_COLOR_PRESSED = cv::Scalar(255, 0, 255);
    cv::Scalar SWITCH_COLOR_DISABLED = cv::Scalar(0, 255, 255);

    cv::Mat *frame;

    cv::Point2f transformSource[3] = {cv::Point2i(000, -10),
                                      cv::Point2i(100, -10),
                                      cv::Point2i(100, 110)};
    cv::Point2f transformDest[3] = {cv::Point2i(), cv::Point2i(), cv::Point2i()};

    cv::Mat transform;

    std::vector<cv::Point2i> panel = {cv::Point2i(+00, +00), cv::Point2i(+10, +00),
                                      cv::Point2i(+10, +40), cv::Point2i(+00, +40),

                                      cv::Point2i(+10, +00), cv::Point2i(+20, +00),
                                      cv::Point2i(+20, +40), cv::Point2i(+10, +40),

                                      cv::Point2i(+20, +00), cv::Point2i(+30, +00),
                                      cv::Point2i(+30, +40), cv::Point2i(+20, +40),

                                      cv::Point2i(+30, +00), cv::Point2i(+40, +00),
                                      cv::Point2i(+40, +40), cv::Point2i(+30, +40),


                                      cv::Point2i(+50, +00), cv::Point2i(+60, +00),
                                      cv::Point2i(+60, +40), cv::Point2i(+50, +40),

                                      cv::Point2i(+60, +00), cv::Point2i(+70, +00),
                                      cv::Point2i(+70, +40), cv::Point2i(+60, +40),


                                      cv::Point2i(+80, +00), cv::Point2i(100, +00),
                                      cv::Point2i(100, +40), cv::Point2i(+80, +40),


                                      cv::Point2i(+00, +60), cv::Point2i(+20, +60),
                                      cv::Point2i(+20, 100), cv::Point2i(+00, 100),


                                      cv::Point2i(+30, +60), cv::Point2i(+50, +60),
                                      cv::Point2i(+50, 100), cv::Point2i(+30, 100),

                                      cv::Point2i(+50, +60), cv::Point2i(+70, +60),
                                      cv::Point2i(+70, 100), cv::Point2i(+50, 100),


                                      cv::Point2i(+80, +60), cv::Point2i(+90, +60),
                                      cv::Point2i(+90, 100), cv::Point2i(+80, 100),

                                      cv::Point2i(+90, +60), cv::Point2i(100, +60),
                                      cv::Point2i(100, 100), cv::Point2i(+90, 100),
    };
    std::vector<cv::Point2i> panelProjected;

    std::vector<std::vector<cv::Point2i>> FLOOR_PLAN = {
            {
                    // outer walls
                    cv::Point2i(+00, +00),
                    cv::Point2i(100, +00),
                    cv::Point2i(100, 100),
                    cv::Point2i(+00, 100),
                    cv::Point2i(+00, +00),
            },
            {
                    // bedroom
                    cv::Point2i(+20, +60),
                    cv::Point2i(+40, +60),
                    cv::Point2i(+40, 100)
            },
            {
                    // bathroom
                    cv::Point2i(+60, +60),
                    cv::Point2i(100, +60)
            }
    };

    std::vector<cv::Point2i> LIGHT_POINTS = {
            cv::Point2i(+20, +30),
            cv::Point2i(+70, +80),
            cv::Point2i(+80, +30),
            cv::Point2i(+20, +80)
    };

    std::vector<bool> lightStates = {
            false,
            false,
            false,
            false
    };

    std::vector<cv::Point2i> switchTestPoints = {cv::Point2i(),
                                                 cv::Point2i(),
                                                 cv::Point2i(),
                                                 cv::Point2i(),
                                                 cv::Point2i()};

    std::vector<int> switchesToLight = {
            000, 001, 002, 003, /**/ 002, 001, /**/ 00000000,
            00000002, /**/ 00000001, 00000000, /**/ 001, 003
    };

    cv::Point2i FLOOR_TOP_LEFT = cv::Point2i(1570, 680);
    cv::Point2i FLOOR_SIZE = cv::Point2i(300, 300);

    int activeSwitchIndex = -1;
    int activeSwitchAge = 0;
    static const int MAX_ACTIVE_SWITCH_AGE = 2;

    int previousPressedSwitchIndex = -1;
    bool switchIsPressed = false;
    int switchPressedAge = 0;
    static const int MAX_SWITCH_PRESSED_AGE = 2;

    static const int MIN_PRESS_INTERVAL = 10;
    int pressInterval = MIN_PRESS_INTERVAL;

    const int NUM_SWITCHES = (int) (panel.size() / 4);

    bool panelIsSet = false;

    std::string displayedName = "";

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

    static inline cv::Point2i getClosestPoint(const std::vector<cv::Point2i> &points,
                                              const cv::Point2i &point);

    static inline cv::Point2i getFurthestPoint(const std::vector<cv::Point2i> &points,
                                               const cv::Point2i &point);

    static inline void getCenter(const cv::Point2i &point1, const cv::Point2i &point2,
                                 cv::Point2i &result);
};


#endif //BACHELORTHESIS_FRAMEPROCESSOR_H
