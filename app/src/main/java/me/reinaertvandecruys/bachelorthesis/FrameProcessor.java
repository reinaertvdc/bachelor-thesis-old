package me.reinaertvandecruys.bachelorthesis;

import android.support.annotation.NonNull;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.Mat;

class FrameProcessor {
    static {
        System.loadLibrary("lib_frameprocessor");
    }

    private long mFrameProcessorAddress;

    FrameProcessor() {
        mFrameProcessorAddress = create();
    }

    @NonNull
    Mat process(@NonNull CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat frame = inputFrame.rgba();

        process(mFrameProcessorAddress,
                frame.getNativeObjAddr(),
                inputFrame.gray().getNativeObjAddr());

        return frame;
    }

    void release() {
        delete(mFrameProcessorAddress);
    }

    private native long create();

    private native void delete(long frameProcessorAddress);

    private native void process(long frameProcessorAddress, long frameAddress, long grayAddress);
}
