#include <jni.h>
#include "FrameProcessor.h"

extern "C" {
JNIEXPORT jlong JNICALL
Java_me_reinaertvandecruys_bachelorthesis_FrameProcessor_create(
        JNIEnv *env,
        jobject thiz) {
    return (jlong) new FrameProcessor();
}

JNIEXPORT void JNICALL
Java_me_reinaertvandecruys_bachelorthesis_FrameProcessor_delete(
        JNIEnv *env,
        jobject thiz,
        jlong frameProcessorAddress) {
    delete (FrameProcessor *) frameProcessorAddress;
}

JNIEXPORT void JNICALL
Java_me_reinaertvandecruys_bachelorthesis_FrameProcessor_process(
        JNIEnv *env,
        jobject thiz,
        jlong frameProcessorAddress,
        jlong frameAddress,
        jlong grayAddress) {

    FrameProcessor &frameProcessor = *(FrameProcessor *) frameProcessorAddress;
    cv::Mat &frame = *(cv::Mat *) frameAddress;
    cv::Mat &gray = *(cv::Mat *) grayAddress;

    frameProcessor.process(frame, gray);
}
}
