package com.igg.bzbee.static_app;


import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;

/*
 * Override EditText to get hide soft keyboard event!
 */
public class MainEditText extends EditText
{
    
	private OpenGLESView mOpenGLES2SurfaceView;
	
	
	public MainEditText(Context context) {
		super(context);
	}
	
	public MainEditText(Context context, AttributeSet attrs) {
		super(context, attrs);
	}
	
	public MainEditText(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public void setEditBoxViewRect(int left, int top, int maxWidth, int maxHeight) {
		setHeight(top + maxHeight);
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	public void setOpenGLES2SurfaceView(final OpenGLESView pOpenGLES2SurfaceView) {
		this.mOpenGLES2SurfaceView = pOpenGLES2SurfaceView;
	}
	
	
	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public boolean onKeyDown(final int KeyCode, final KeyEvent event) {
		/* Let GlSurfaceView get focus if back key is input. */
		if (KeyCode==KeyEvent.KEYCODE_BACK){
			this.mOpenGLES2SurfaceView.requestFocus();
		}

		return super.onKeyDown(KeyCode, event);
	}
	@Override  
    public void onEditorAction(int actionCode) {  
		super.onEditorAction(actionCode);
		if (actionCode==EditorInfo.IME_ACTION_DONE ){
			this.mOpenGLES2SurfaceView.requestFocus();
		}
    }  
	
	/*
	@Override
	public boolean dispatchKeyEventPreIme(KeyEvent event) 
	{
        if(event.getKeyCode() == KeyEvent.KEYCODE_BACK)
        {
        	if(getVisibility()==View.VISIBLE){
        		setVisibility(View.INVISIBLE);
        		Casino.closeIME();
        	}
        }
		return super.dispatchKeyEventPreIme(event);
	}
	*/
}
