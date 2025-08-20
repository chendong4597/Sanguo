package com.igg.bzbee.static_app;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.FrameLayout;

import com.igg.bzbee.static_baseutil.FileSystem;

import java.io.File;

public class ActivityBase extends Activity {

	public static final String TAG = "ActivityBase";

	public  interface  Fore_BackgroundListener{
		void onForeground();
		void onBackground();
	}
	private  Fore_BackgroundListener m_forebackgroundListener = null;
	public static int activityActive = 0; //全局变量,表示当前在前台的activity数量
	public static boolean isBackground  = false;
	public RelativeLayout mRelativeLayout = null;
	MainEditText mMainEditText = null;
	OpenGLESView mView = null;
	private RennderThreadCallBack  mCallBack = null;
	// 启动闪屏
	private SplashFragment mSplashFragment = null;
	public Handler mSplashHandler = null;
	
	private static ActivityBase s_instance;
	
	public static ActivityBase getCurActivity() {
		return s_instance;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		//### Log.d(TAG, "onCreate 1");
        // Query device capacity
		ActivityManager.MemoryInfo memoryInfo = getAvailableMemory();
		float memoryCapacity = (float)memoryInfo.totalMem / (1024*1024*1024);	// in gigabyte units
		GameEntry.setMemoryCapacity(memoryCapacity);
		Log.i(TAG, "Memory capacity: " + memoryCapacity);

		Runtime runtime = Runtime.getRuntime();
		int availableProcessors = runtime.availableProcessors();
		GameEntry.setCpuCoreNum(availableProcessors);
		Log.i(TAG, "Available processors: " + availableProcessors);

		int maxFreq = getCPUMaxFreqMHz();
		GameEntry.setCpuMaxFreq(maxFreq);
		Log.i(TAG, "Max frequence: " + maxFreq);

		// initialize file path
		File internalFile = getFilesDir();
		File externalFile = getExternalFilesDir(null);
		FileSystem.initialize(getAssets(), internalFile.getPath(), externalFile == null ? internalFile.getPath() : externalFile.getPath());

		mRelativeLayout = new RelativeLayout(this);
		ViewGroup.LayoutParams vgParam = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
		mRelativeLayout.setId(View.generateViewId());
		mRelativeLayout.setGravity(Gravity.CENTER);
		setContentView(mRelativeLayout, vgParam);

		FrameLayout.LayoutParams edittext_layout_params = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
				FrameLayout.LayoutParams.WRAP_CONTENT);
		mMainEditText = new MainEditText(this);
		mMainEditText.setLayoutParams(edittext_layout_params);
		mRelativeLayout.addView(mMainEditText);

		mView = new OpenGLESView(this, getApplication());
		RelativeLayout.LayoutParams rlParam = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
		mView.setLayoutParams(rlParam);
		mView.initView();
		mView.setMainEditText(mMainEditText);
		mRelativeLayout.addView(mView);
		mView.setVisibility(View.INVISIBLE);
		hideVirtualKeyboard();

		// 启动闪屏
		final FragmentManager fragmentManager = getFragmentManager();
		FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
		mSplashFragment = new SplashFragment();
		fragmentTransaction.add(mRelativeLayout.getId(), mSplashFragment);
		fragmentTransaction.commit();

		mSplashHandler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				switch (msg.what) {
					case 1:
						Log.e(TAG, "Splash prepare to be removed .... ");
						fragmentManager.beginTransaction().remove(mSplashFragment).commitAllowingStateLoss();
						break;
				}
			}
		};

		// OpenGLESView show delay 500ms
		mSplashHandler.postDelayed(new Runnable() {
			@Override
			public void run() {
				mView.setVisibility(View.VISIBLE);
			}
		}, 500);
		
		s_instance = this;
	}

	@Override
	protected void onStart() {
		super.onStart();
	}
	@Override
	protected void onPause() {
		activityActive--;
		if (activityActive == 0) {
			//app 进入后台
			isBackground = true;//记录当前已经进入后台
			if (null != m_forebackgroundListener) {
				m_forebackgroundListener.onBackground();
			}
		}
		mView.onPause();
		super.onPause();
		//### Log.d(TAG, "onPause 1, activityActive=" + activityActive + ",isBackground = " + isBackground);
	}

	@Override
	protected void onResume() {
		super.onResume();

		//app 从后台唤醒，进入前台
		activityActive++;
		isBackground = false;
		//### Log.d(TAG, "onResume 1, activityActive=" + activityActive + ",isBackground = " + isBackground);

		if (null != m_forebackgroundListener) m_forebackgroundListener.onForeground();
		mView.onResume();
		hideVirtualKeyboard();
	}

	@Override
	protected void onStop() {
		super.onStop();
		//### Log.d(TAG, "onStop 1");
	}
	@SuppressLint("NewApi")
	public void hideVirtualKeyboard()
	{
		if (Build.VERSION.SDK_INT < 11) return;

		// The UI options currently enabled are represented by a bitfield.
		// getSystemUiVisibility() gives us that bitfield.
		int uiOptions = mRelativeLayout.getSystemUiVisibility();
		int newUiOptions = uiOptions;
		// Navigation bar hiding:  Backwards compatible to ICS.
		if (Build.VERSION.SDK_INT >= 14) {newUiOptions |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;}

		// Status bar hiding: Backwards compatible to Jellybean
		if (Build.VERSION.SDK_INT >= 16) {
			newUiOptions |= View.SYSTEM_UI_FLAG_FULLSCREEN;
			newUiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
			newUiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
		}

		// Immersive mode: Backward compatible to KitKat.
		// Note that this flag doesn't do anything by itself, it only augments the behavior
		// of HIDE_NAVIGATION and FLAG_FULLSCREEN.  For the purposes of this sample
		// all three flags are being toggled together.
		// Note that there are two immersive mode UI flags, one of which is referred to as "sticky".
		// Sticky immersive mode differs in that it makes the navigation and status bars
		// semi-transparent, and the UI flag does not get cleared when the user interacts with
		// the screen.
		if (Build.VERSION.SDK_INT >= 18) {newUiOptions |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;}

		mRelativeLayout.setSystemUiVisibility(newUiOptions);
		mView.requestFocus();
	}

	public RennderThreadCallBack getmCallBack() {
		return mCallBack;
	}

	public void setmCallBack(RennderThreadCallBack mCallBack) {
		this.mCallBack = mCallBack;
	}
	public Fore_BackgroundListener getForebackgroundListener() {
		return m_forebackgroundListener;
	}

	public void setForebackgroundListener(Fore_BackgroundListener m_forebackgroundListener) {
		this.m_forebackgroundListener = m_forebackgroundListener;
	}
	
	// Support getting writable path as dump path.
	public String getDumpFilePath() {		
		String dumpFilePath = null;
		boolean IsExtStroageAvailable = IsExternalStorageAvailable();
		if (IsExtStroageAvailable) {
			boolean bIsExtStroageReadOnly = IsExternalStorageReadOnly();
			if (!bIsExtStroageReadOnly) {
				dumpFilePath = getExternalCacheDirectory();
			}
		}
        
		if (dumpFilePath == null)
			dumpFilePath = getInternalCacheDirectory();
		
		if (dumpFilePath != null)
			mkdir(dumpFilePath);
		
		return dumpFilePath;
	}
	
	protected boolean IsExternalStorageAvailable() {
		boolean state = false;
		String extStorageState = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(extStorageState)) {
			state = true;
		}
		return state;
	}
	
	protected boolean IsExternalStorageReadOnly() {
		boolean state = false;
		String extStorageState = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(extStorageState)) {
			state = true;
		}
		return state;
	}
	
	protected String getInternalCacheDirectory() {
		String cacheDirPath = null;
		File cacheDir = getCacheDir();
		if (cacheDir != null) {
			cacheDirPath = cacheDir.getPath();
		}
		return cacheDirPath;
	}
	
	protected String getExternalCacheDirectory() {
		String extCacheDirPath = null;
		File cacheDir = getExternalCacheDir();
		if (cacheDir != null) {
			extCacheDirPath = cacheDir.getPath();
		}
		return extCacheDirPath;
	}
	
	public static void mkdir(String path)
    {
    	java.io.File fd = null;
        
        try {
            fd = new java.io.File(path);
            if (!fd.exists())
            {
                fd.mkdirs();
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        finally {
            fd = null;
        }
    }

	// Get a MemoryInfo object for the device's current memory status.
	private ActivityManager.MemoryInfo getAvailableMemory() {
		ActivityManager activityManager = (ActivityManager) this.getSystemService(ACTIVITY_SERVICE);
		ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
		activityManager.getMemoryInfo(memoryInfo);
		return memoryInfo;
	}

	//get cpu max freq
	private int getCPUMaxFreqMHz(){
		int maxFreq = 0;
		File[] cpuFiles = getCPUs();
		String strFileList = "";
		try{
			for(int i =0; i<cpuFiles.length; i++){
				String strCpuFreqPath = cpuFiles[i].getAbsolutePath()+"/cpufreq/cpuinfo_max_freq";
				String strCpuFreq = cmdCat(strCpuFreqPath);
				float freqBound = Float.parseFloat(strCpuFreq) / 1000;
				if(freqBound > maxFreq)	{
					maxFreq = (int)freqBound;
				}
			}
			return maxFreq;
		} catch (Exception e) {
			e.printStackTrace();
			return 2100;
		}

	}

	// //get cpu cur freq
	// private void readCpuFreqNow(){
	// 	File[] cpuFiles = getCPUs();
	// 	String strFileList = "";
	// 	for(int i=0; i<cpuFiles.length; i++){

	// 		String path_scaling_cur_freq =
	// 				cpuFiles[i].getAbsolutePath()+"/cpufreq/scaling_cur_freq";

	// 		String scaling_cur_freq = cmdCat(path_scaling_cur_freq);

	// 		strFileList +=
	// 				i + ": "
	// 						+ path_scaling_cur_freq + "\n"
	// 						+ scaling_cur_freq
	// 						+ "\n";
	// 	}

	// 	Log.i(TAG, strFileList);
	// }

	//run Linux command
	//$ cat f
	private String cmdCat(String f){

		String[] command = {"cat", f};
		StringBuilder cmdReturn = new StringBuilder();

		try {
			ProcessBuilder processBuilder = new ProcessBuilder(command);
			Process process = processBuilder.start();

			java.io.InputStream inputStream = process.getInputStream();
			int c;

			while ((c = inputStream.read()) != -1) {
				cmdReturn.append((char) c);
			}

			return cmdReturn.toString();

		} catch (java.io.IOException e) {
			e.printStackTrace();
			return "Something Wrong";
		}

	}

	/*
	 * Get file list of the pattern
	 * /sys/devices/system/cpu/cpu[0..9]
	 */
	private File[] getCPUs(){

		class CpuFilter implements java.io.FileFilter {
			@Override
			public boolean accept(File pathname) {
				if(java.util.regex.Pattern.matches("cpu[0-9]+", pathname.getName())) {
					return true;
				}
				return false;
			}
		}

		File dir = new File("/sys/devices/system/cpu/");
		File[] files = dir.listFiles(new CpuFilter());
		return files;
	}

	/**
	 * 渲染线程发送事件
	 * @param runnable
	 */
	public void sendMsgToCpp(Runnable runnable) {
		if (mView != null) {
			mView.queueEvent(runnable);
		}
	}

}
