package com.igg.bzbee.static_app;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipDescription;
import android.content.ClipboardManager;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import com.igg.bzbee.static_app.ActivityBase;

public class GameEntry {
    public static final int TOUCH_BEGIN = 1;
    public static final int TOUCH_END = 2;
    public static final int TOUCH_MOVE = 3;

    public static native boolean initialize(int width, int height);

    public static native void terminate();

    public static native void resize(int width, int height);

    public static native void reload();

    public static native void step();

    public static native void touchEvent(int length, int pointerId[], int actionType[], float x[], float y[]);

    public static native void setDeviceId(String deviceId);

    public static native void setNativeActivity(Object activity);

    public static native void setMainObbPath(String path);
    public static native void setPatchObbPath(String path);
    public static native void setObbVersionCode(int obbVersionCode);
    public static native void onGetObbUrl(String obbUrl, int step, int code);

	public static native void setDumpFilePath(String path);
	
    public static native void insertText(String pText);
    public static native void deleteBackward();
    public static native void setVideoTransformMatrix(float[] transformMatrix);
    public static native int getVideoTextureName();
    public static native int reloadVideoTexture();
    public static native String getContentText();
    public static native int getEditBoxHeight();
    public static native int getEditBoxState();
    public static native void setMemoryCapacity(float capacity);
    public static native void setCpuMaxFreq(int freq);
    public static native void setCpuCoreNum(int num);
	public static native void setAndroidVersion(String pText);
    public static native void mediaPlayStart(String filePath, int videoWidth, int videoHeight);
    public static native void mediaPlayEnd(String filePath);
	public static native void mediaPlayError(String filePath);

	public static native void onPause();
	public static native void onResume();

    public static native void setDeviceModel(String devModel);
    public static native void setVersionName(String verName);
    public static native String getGameId();
    public static native String getScretKey();
    public static native boolean getIsChinaMainland();
    public static native String getPaymentKey();

    public static native void setApplicationName(String appName);

	public static String getNetworkType()
	{
		Activity activity = ActivityBase.getCurActivity();
		if (activity == null)
		{ 
			return "";
		}

		ConnectivityManager cm = (ConnectivityManager)activity.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info = cm.getActiveNetworkInfo();
		if (info == null) 
		{
			return "";
		}
		
		int type = info.getType();
		String s= String.valueOf(type);
		return s;
	}
}

