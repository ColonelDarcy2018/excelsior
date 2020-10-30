package com.itexcelsior.as.x.rtmp.camera;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.security.Policy;

public class CameraHandler {

    private static String TAG = "ZXW@CameraHandler";

    private Camera mCamera = null;
    private boolean isPreview = false;
    private SurfaceView sv;
    Camera.PreviewCallback callback;

    private CameraHandler(SurfaceView sv, Camera.PreviewCallback callback) {
        this.sv = sv;
        this.callback = callback;
    }

    public Camera getCamera() {
        return mCamera;
    }

    public static CameraHandler build(@NonNull SurfaceView sv, Camera.PreviewCallback callback,@NonNull SurfaceHolder.Callback surfaceHolderCallback) {
        //添加预览回调函数
        sv.getHolder().addCallback(surfaceHolderCallback);
        return new CameraHandler(sv, callback);
    }


    private void openCamera() {
        //防止重复打开
        if (mCamera!=null)return;
        //0前置 1后置
        mCamera = Camera.open(1);

        if (mCamera != null && !isPreview) {
            Camera.Parameters parameters = mCamera.getParameters();
            //TODO 测试使用，生产删除
            for (Camera.Size size : parameters.getSupportedPictureSizes()) {
                Log.w(TAG, size.width + "           " + size.height);
            }
            Log.d(TAG, "==============================================");
            for (Camera.Size size : parameters.getSupportedPreviewSizes()) {
                Log.w(TAG, size.width + "           " + size.height);
            }


            Log.d(TAG, "==============================================");
            for (int[] fpss : parameters.getSupportedPreviewFpsRange()) {
                for (int i=0;i<fpss.length;i++){
                    Log.w(TAG,"fps取值范围："+fpss[i]);
                }
                Log.d(TAG, "----------------");
            }
            //TODO 结束

            parameters.setPictureFormat(ImageFormat.NV21); //设置图片格式
//            parameters.setPictureFormat(ImageFormat.YUV_420_888);
            //@zxw
            parameters.setPreviewSize(720,480);
//            parameters.setPreviewFpsRange(15000,15000);
            mCamera.setParameters(parameters);

            try {
                mCamera.setPreviewDisplay(sv.getHolder());
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera.setPreviewCallback(callback);
            mCamera.startPreview();


        }
        isPreview = true;
    }


    public void releaseCamera() {
        if (mCamera != null){
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera=null;
        }
    }

    /**
     * 将预览数据展示到surfaceView上
     */
    public void setPreviewDisplay() {
        if (mCamera==null){
            openCamera();
        }
        try {
            mCamera.setPreviewDisplay(sv.getHolder());
            mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}
