package com.itexcelsior.as.x.rtmp.ffmpeg;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.itexcelsior.as.x.rtmp.camera.CameraActivity;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    TextView tv;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native String getAvcodecConfiguartion();

    public native String getDumpLog(String filePath);

    public native void printDeviceData();

//    public native int initVideo(String url,int width,int height);

    public native int pushRtmpFile(String path);

    public native void test();

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case  R.id.btn_get_configure:
//                tv.setText(getAvcodecConfiguartion());
                test();

                break;
            case R.id.btn_print_device:
                final String path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "u21word.flv";
//                printDeviceData();
//                initVideo("rtmp://49.232.93.219/live/test",480,320);

                if (ContextCompat.checkSelfPermission(this,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE)
                        != PackageManager.PERMISSION_GRANTED) {
                    //没有授权，编写申请权限代码
                    ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
                } else {
                    Log.d("zxw", "requestMyPermissions: 有写SD权限");
                }
                pushRtmpFile(path);

                break;
            case R.id.btn_to_camera:
                //跳转到摄像头预览页面
                startActivity(new Intent(this, CameraActivity.class));

        }
    }
}
