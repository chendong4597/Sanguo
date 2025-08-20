package com.igg.bzbee.static_app;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.text.InputType;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.inputmethod.InputMethodManager;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


public class OpenGLESView extends GLSurfaceView implements SurfaceTexture.OnFrameAvailableListener
{
    private final static int HANDLER_OPEN_IME_KEYBOARD = 2;
    private final static int HANDLER_CLOSE_IME_KEYBOARD = 3;

    private static OpenGLESView msOpenGLESSurfaceView;
    private static TextInputWraper msTextInputWraper;
    private static Handler msHandler;
    private static ActivityBase mActivity = null;
    private static MainEditText mMainEditText = null;
    private static MediaPlayer mMediaPlayer = null;
    private static SurfaceTexture mVideoTexture = null;
    private static int mVideoTextureName;
    private static boolean mPlaying = false;
    private static int mVideoPosition = 0;
    private static String mVideoFilePath;
    private static long mVideoFileOffset;
    private static long mVideoFileLength;
    private static boolean mLoop = false;

	private static String obbAssetsDirStr;

    Renderer mRenderer = null;

    MultipleTouchProcessor mMultipleTouchProcessor = new MultipleTouchProcessor();

    private class Renderer implements GLSurfaceView.Renderer {

        boolean mRequestInitialize = false;
        boolean mInitialized = false;

        int mFrameCount = 0;

        @Override
        public void onDrawFrame(GL10 gl) {
            if (mRequestInitialize) return;

            long startTime = System.currentTimeMillis();
            // step
            GameEntry.step();
            long endTime = System.currentTimeMillis();
            if (mFrameCount <= 3) {
                Log.i("OpenGLESView", String.format("on step time: %d ms", endTime - startTime));
            }

            // after third frame, remove splash image
            mFrameCount++;
            if (mFrameCount == 3) {
                OpenGLESView.this.mActivity.mSplashHandler.sendEmptyMessage(1);
            }
        }

        /**
         * @param gl
         * @param width
         * @param height
         */
        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            if (mRequestInitialize) {
                mRequestInitialize = false;

                if(null != mActivity && null != mActivity.getmCallBack())
                {
                    mActivity.getmCallBack().init();
                }

                long startTime = System.currentTimeMillis();
                GameEntry.initialize(width, height);
                long endTime = System.currentTimeMillis();
                Log.i("OpenGLESView", String.format("initialize time: %d ms", endTime - startTime));

                // Reload video texture
                GameEntry.reloadVideoTexture();
                if (mMediaPlayer != null)
                {
                    mVideoTextureName = GameEntry.getVideoTextureName();
                    resetTexture();
                }

                mInitialized = true;
            }

            // resize
            GameEntry.resize(width, height);
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // initialize
            if (mInitialized){
                GameEntry.reload();

                // Reload video texture
                GameEntry.reloadVideoTexture();
                if (mMediaPlayer != null)
                {
                    mVideoTextureName = GameEntry.getVideoTextureName();
                    resetTexture();
                }
            }
            else {
                mRequestInitialize = true;
            }
        }

        public boolean isInitialized() {
            return mInitialized;
        }

        public void handleInsertText(String pText) {
            String strText = pText;
            Log.i("Input", pText);
            GameEntry.insertText(strText);
        }

        public void handleDeleteBackward() {
            GameEntry.deleteBackward();
        }
        public void setVideoTransformMatrix(float[] transformMatrix)
        {
            GameEntry.setVideoTransformMatrix(transformMatrix);
        }

        public String getContentText() {
            return GameEntry.getContentText();
        }

        public int getEditBoxHeight() {
            return GameEntry.getEditBoxHeight();
        }

        public int getEditBoxState() {
            return GameEntry.getEditBoxState();
        }
    }

    /**
     * ***************************************************************************
     */

    public OpenGLESView(ActivityBase activity, Context context) {
        super(context);
        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        if (Build.VERSION.SDK_INT >= 11)
        {
            setPreserveEGLContextOnPause(true);
        }
        mRenderer = new Renderer();
        setRenderer(mRenderer);
        mActivity = activity;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mRenderer == null || !mRenderer.isInitialized()) return true;

        return mMultipleTouchProcessor.process(this, event);
    }

    @Override
    public void onPause() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (msOpenGLESSurfaceView.mMediaPlayer != null && msOpenGLESSurfaceView.mMediaPlayer.isPlaying()) {
                        msOpenGLESSurfaceView.mVideoPosition = msOpenGLESSurfaceView.mMediaPlayer.getCurrentPosition();
                        msOpenGLESSurfaceView.mMediaPlayer.pause();
                    }
                } catch (IllegalStateException e) {
                    Log.e("Video", "Pause media player failed, illegal state exception.", e);
                    msOpenGLESSurfaceView.onError();
                }
            }
        });
        super.onPause();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (msOpenGLESSurfaceView.mMediaPlayer != null && !msOpenGLESSurfaceView.mMediaPlayer.isPlaying() && msOpenGLESSurfaceView.mPlaying)
                    {
                        resetTexture();
                        msOpenGLESSurfaceView.mMediaPlayer.start();
                        if (!msOpenGLESSurfaceView.mMediaPlayer.isPlaying())
                        {
                            playVideo(mVideoFilePath, mVideoFileOffset, mVideoFileLength, mLoop);
                            if (msOpenGLESSurfaceView.mMediaPlayer.isPlaying())
                            {
                                msOpenGLESSurfaceView.mMediaPlayer.seekTo(msOpenGLESSurfaceView.mVideoPosition);
                            }
                        }
                    }
                } catch (IllegalStateException e) {
                    Log.e("Video", "Start media player failed, illegal state exception.", e);
                    msOpenGLESSurfaceView.onError();
                }
            }
        });
    }

    public void initView() {
        this.setFocusableInTouchMode(true);

        OpenGLESView.msOpenGLESSurfaceView = this;
        OpenGLESView.msTextInputWraper = new TextInputWraper(this);

        msHandler = new Handler() {
            @Override
            public void handleMessage(final Message msg) {
                switch (msg.what) {
                    case HANDLER_OPEN_IME_KEYBOARD:
                        if (null != OpenGLESView.this.mMainEditText && OpenGLESView.this.mMainEditText.requestFocus()) {
                            OpenGLESView.this.mMainEditText.removeTextChangedListener(OpenGLESView.msTextInputWraper);
                            OpenGLESView.this.mMainEditText.setText("");

                            final String text = (String) msg.obj;
                            int state = OpenGLESView.this.getEditBoxState();
                            int inputType = 0;
                            if ((state & InputDefine.EDITBOX_STATE_NUMBER) != 0){
                                inputType |= InputType.TYPE_CLASS_NUMBER;
                            } else {
                                inputType |= InputType.TYPE_CLASS_TEXT;
                            }
                            if ((state & InputDefine.EDITBOX_STATE_MULTI_LINE)!=0) {
                                inputType |= InputType.TYPE_TEXT_FLAG_MULTI_LINE;
                            }
                            OpenGLESView.this.mMainEditText.setInputType(inputType);

                            OpenGLESView.this.mMainEditText.append(text);
                            OpenGLESView.msTextInputWraper.setOriginText(text);
                            OpenGLESView.this.mMainEditText.setHeight(getEditBoxHeight());
                            OpenGLESView.this.mMainEditText.addTextChangedListener(OpenGLESView.msTextInputWraper);
                            final InputMethodManager imm = (InputMethodManager) OpenGLESView.msOpenGLESSurfaceView.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                            imm.showSoftInput(OpenGLESView.this.mMainEditText, 0);
                            Log.d("GLSurfaceView", "showSoftInput");
                        }
                        break;

                    case HANDLER_CLOSE_IME_KEYBOARD:
                        if (null != OpenGLESView.this.mMainEditText) {
                            OpenGLESView.this.mMainEditText.setHeight(0);
                            OpenGLESView.this.mMainEditText.removeTextChangedListener(OpenGLESView.msTextInputWraper);
                            final InputMethodManager imm = (InputMethodManager) OpenGLESView.msOpenGLESSurfaceView.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                            imm.hideSoftInputFromWindow(OpenGLESView.this.mMainEditText.getWindowToken(), 0);
                            Log.d("GLSurfaceView", "HideSoftInput");
                            mActivity.hideVirtualKeyboard();
                        }
                        break;
                }
            }
        };
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================
    private String getContentText() {
        return this.mRenderer.getContentText();
    }

    public int getEditBoxHeight() {
        return this.mRenderer.getEditBoxHeight();
    }

    public int getEditBoxState() {
        return this.mRenderer.getEditBoxState();
    }

    private void resetTexture()
    {
        if (mVideoTexture != null)
        {
            mVideoTexture.setOnFrameAvailableListener(null);
            mVideoTexture.release();
        }

        try {
            mVideoTexture = new SurfaceTexture(mVideoTextureName);
            mVideoTexture.setOnFrameAvailableListener(OpenGLESView.msOpenGLESSurfaceView);
            mMediaPlayer.setSurface(new Surface(mVideoTexture));
        }
        catch (Exception e)
        {
            Log.e("Video", "Reset texture failed.", e);
        }
    }

    private void onError() {
        mPlaying = false;
        if (mMediaPlayer != null) {
            try {
                mMediaPlayer.reset();
            } catch (IllegalStateException e) {
                Log.e("Video", "onError", e);
            }
        }
        GameEntry.mediaPlayError(mVideoFilePath);
    }

    public MainEditText getMainEditText() {
        return this.mMainEditText;
    }

    public void setMainEditText(final MainEditText pMainEditText) {
        this.mMainEditText = pMainEditText;
        if (this.mMainEditText != null)
        {
            this.mMainEditText.setImeOptions(301989894); // EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_FLAG_NO_FULLSCREEN
            this.mMainEditText.setGravity(Gravity.BOTTOM);
        }

        if (null != this.mMainEditText && null != OpenGLESView.msTextInputWraper) {
            this.mMainEditText.setOnEditorActionListener(OpenGLESView.msTextInputWraper);
            this.mMainEditText.setOpenGLES2SurfaceView(this);
            this.requestFocus();
        }
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture)
    {
        if (mRenderer == null || !mRenderer.isInitialized()) { return; }

        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mMediaPlayer == null || mVideoTexture == null || !mMediaPlayer.isPlaying() || !mPlaying) { return; }
                try {
                    mVideoTexture.updateTexImage();
                    float[] transformMatrix = new float[16];
                    mVideoTexture.getTransformMatrix(transformMatrix);
                    OpenGLESView.this.mRenderer.setVideoTransformMatrix(transformMatrix);
                }
                catch (IllegalStateException e)
                {
                    Log.e("Video", "Update texture image failed, illegal state exception.", e);
                    resetTexture();
                    onError();
                }
                catch (Exception e)
                {
                    Log.e("Video", "Update texture image failed.", e);
                    resetTexture();
                    onError();
                }
            }
        });
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static void openIMEKeyboard() {
        final Message msg = new Message();
        msg.what = OpenGLESView.HANDLER_OPEN_IME_KEYBOARD;
        msg.obj = OpenGLESView.msOpenGLESSurfaceView.getContentText();
        OpenGLESView.msHandler.sendMessage(msg);
        Log.d("openIMEKeyboard", "openIMEKeyboard");
    }

    public static void closeIMEKeyboard() {
        final Message msg = new Message();
        msg.what = msOpenGLESSurfaceView.HANDLER_CLOSE_IME_KEYBOARD;
        msOpenGLESSurfaceView.msHandler.sendMessage(msg);
    }

    public static void setObbAssetsDir(String dirStr)
    {
        obbAssetsDirStr = dirStr;
    }

    public static boolean playVideo(final String filePath, final long offset, final long length, boolean bLoop) {
        mVideoTextureName = GameEntry.getVideoTextureName();
        if (msOpenGLESSurfaceView.mRenderer == null || !msOpenGLESSurfaceView.mRenderer.isInitialized()) {
            return false;
        }
        if (mMediaPlayer == null) {
            mMediaPlayer = new MediaPlayer();
        } else {
            try {
                mMediaPlayer.reset();
            } catch (IllegalStateException e) {
                Log.e("Video", "playVideo", e);
            }
        }
        try {
            if (filePath.startsWith("/")) {
                // Absolute path
                FileInputStream fileInputStream = new FileInputStream(new File(filePath));
                if (length > 0) {
                    mMediaPlayer.setDataSource(fileInputStream.getFD(), offset, length);
                } else {
                    mMediaPlayer.setDataSource(fileInputStream.getFD());
                }
            } else {
                // File from apk
                AssetFileDescriptor afd = mActivity.getAssets().openFd(filePath);
                mMediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            }

            mMediaPlayer.setLooping(bLoop);
            mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                @Override
                public void onPrepared(final MediaPlayer mp) {
                    msOpenGLESSurfaceView.queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                mp.start();
                                mPlaying = mp.isPlaying();
                                mVideoFilePath = filePath;
                                mVideoFileOffset = offset;
                                mVideoFileLength = length;
                                if (mPlaying) {
                                    GameEntry.mediaPlayStart(mVideoFilePath, mp.getVideoWidth(), mp.getVideoHeight());
                                }
                            } catch (IllegalStateException e) {
                                e.printStackTrace();
                                msOpenGLESSurfaceView.onError();
                            }
                        }
                    });
                }
            });
            mMediaPlayer.prepareAsync();

            mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener(){
                @Override
                public void onCompletion(final MediaPlayer mp) {
                    msOpenGLESSurfaceView.queueEvent(new Runnable() {
                                                         @Override
                                                         public void run() {
															 Log.d("Video", "setOnCompletionListener");
                                                             OpenGLESView.stop();
                                                             GameEntry.mediaPlayEnd(mVideoFilePath);
                                                         }
                                                     }
                    );
                }
            });

            mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
                @Override
                public boolean onError(final MediaPlayer mediaPlayer, int what, int extra) {
                    msOpenGLESSurfaceView.queueEvent(new Runnable() {
                                                         @Override
                                                         public void run() {
															 Log.e("Video", "setOnErrorListener");
                                                             OpenGLESView.stop();
                                                             GameEntry.mediaPlayError(mVideoFilePath);
                                                         }
                                                     }
                    );
                    return false;
                }
            });

            mLoop = bLoop;
            msOpenGLESSurfaceView.resetTexture();

            Log.d("Video", "playVideo");
            return true;
        }
        catch (IOException e)
        {
			GameEntry.mediaPlayEnd(mVideoFilePath);
            Log.e("Video", "Could not open video.", e);
            return false;
        }
        catch(Exception e)
        {
			GameEntry.mediaPlayEnd(mVideoFilePath);
            Log.e("Video", "Prepare media player failed.", e);
            return false;
        }
    }

    public static void seekTo(final int msec) {
        if (msOpenGLESSurfaceView.mMediaPlayer != null && msOpenGLESSurfaceView.mPlaying) {
            try {
                msOpenGLESSurfaceView.mMediaPlayer.seekTo(msec);
            } catch (IllegalStateException e) {
                Log.e("Video", "seek to media player failed, illegal state exception.", e);
                msOpenGLESSurfaceView.onError();
            }
        }
    }

    public static void stop() {
        if (msOpenGLESSurfaceView.mMediaPlayer != null && msOpenGLESSurfaceView.mPlaying) {
            try {
                if (mVideoTexture != null) {
                    mVideoTexture.setOnFrameAvailableListener(null);
                    mVideoTexture.release();
                }
                mMediaPlayer.setOnCompletionListener(null);
                msOpenGLESSurfaceView.mMediaPlayer.stop();
                msOpenGLESSurfaceView.mMediaPlayer.release();
                msOpenGLESSurfaceView.mMediaPlayer = null;
                msOpenGLESSurfaceView.mPlaying = false;
            } catch (IllegalStateException e) {
                Log.e("Video", "stop media player failed, illegal state exception.", e);
                msOpenGLESSurfaceView.onError();
            }
        }
    }

    public static void setEditBoxViewRect(final int left, final int top, final int maxWidth, final int maxHeight) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mMainEditText != null) {
                    mMainEditText.setEditBoxViewRect(left, top, maxWidth, maxHeight);
                }
            }
        });
    }

    public void insertText(final String pText) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                OpenGLESView.this.mRenderer.handleInsertText(pText);
            }
        });
    }

    public void deleteBackward() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                OpenGLESView.this.mRenderer.handleDeleteBackward();
            }
        });
    }
}
