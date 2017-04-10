package me.reinaertvandecruys.bachelorthesis;

import android.content.Context;
import android.hardware.Camera;
import android.support.annotation.NonNull;
import android.util.AttributeSet;

import org.opencv.android.JavaCameraView;

import java.util.Arrays;
import java.util.List;

@SuppressWarnings("deprecation")
public class CustomJavaCameraView extends JavaCameraView {
    public CustomJavaCameraView(Context context, int cameraId) {
        super(context, cameraId);
    }

    public CustomJavaCameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @NonNull
    public int[] getFpsRange() throws IllegalStateException {
        try {
            int[] fpsRange = {0, 0};

            mCamera.getParameters().getPreviewFpsRange(fpsRange);

            return fpsRange;
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    public void setFpsRange(@NonNull int[] range) throws IllegalStateException {
        try {
            if (!supportsFpsRange(range)) {
                throw new RuntimeException();
            }

            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFpsRange(range[0], range[1]);
            mCamera.setParameters(parameters);
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    @NonNull
    public int[] getHighestSupportedFpsRange() throws IllegalStateException {
        try {
            List<int[]> supportedFpsRanges = getSupportedFpsRanges();
            int[] result = supportedFpsRanges.get(supportedFpsRanges.size() - 1);

            if (result == null) {
                throw new RuntimeException();
            }

            return result;
        } catch (RuntimeException e) {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    @NonNull
    public Camera.Size getResolution() throws IllegalStateException {
        try {
            Camera.Size result = mCamera.getParameters().getPreviewSize();

            if (result == null) {
                throw new RuntimeException();
            }

            return result;
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    public void setResolution(@NonNull Camera.Size resolution) throws IllegalStateException {
        try {
            if (!supportsResolution(resolution)) {
                throw new RuntimeException();
            }

            disconnectCamera();
            connectCamera(resolution.width, resolution.height);
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    @NonNull
    public List<int[]> getSupportedFpsRanges() {
        try {
            List<int[]> result = mCamera.getParameters().getSupportedPreviewFpsRange();

            if (result == null) {
                throw new RuntimeException();
            }

            return result;
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    @NonNull
    public List<Camera.Size> getSupportedResolutions() throws IllegalStateException {
        try {
            List<Camera.Size> result = mCamera.getParameters().getSupportedPreviewSizes();

            if (result == null) {
                throw new RuntimeException();
            }

            return result;
        } catch (RuntimeException e) {
            throw new IllegalStateException();
        }
    }

    public void setFpsRangeToHighestSupported() throws IllegalStateException {
        setFpsRange(getHighestSupportedFpsRange());
    }

    public void setResolutionToHighestSupported() throws IllegalStateException {
        setResolution(getHighestSupportedResolution());
    }

    public boolean supportsFpsRange(@NonNull int[] fpsRange) throws IllegalStateException {
        for (int[] supportedFpsRange : getSupportedFpsRanges()) {
            if (Arrays.equals(supportedFpsRange, fpsRange)) {
                return true;
            }
        }

        return false;
    }

    public boolean supportsResolution(@NonNull Camera.Size resolution)
            throws IllegalStateException {
        for (Camera.Size supportedResolution : getSupportedResolutions()) {
            if (supportedResolution.equals(resolution)) {
                return true;
            }
        }

        return false;
    }

    @NonNull
    Camera.Size getHighestSupportedResolution() throws IllegalStateException {
        try {
            Camera.Size highestResolution = getResolution();
            int highestSize = highestResolution.width * highestResolution.height;

            for (Camera.Size resolution : getSupportedResolutions()) {
                int size = highestResolution.width * highestResolution.height;

                if (size > highestSize) {
                    highestResolution = resolution;
                    highestSize = size;
                }
            }

            return highestResolution;
        } catch (RuntimeException e) {
            throw new ArrayIndexOutOfBoundsException();
        }
    }
}
