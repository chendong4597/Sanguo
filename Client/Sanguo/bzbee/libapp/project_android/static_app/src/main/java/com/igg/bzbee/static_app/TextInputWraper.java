/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.igg.bzbee.static_app;

import android.text.Editable;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

public class TextInputWraper implements TextWatcher, OnEditorActionListener {
	// ===========================================================
	// Constants
	// ===========================================================

	@SuppressWarnings("unused")
	private static final String TAG = TextInputWraper.class.getSimpleName();

	// ===========================================================
	// Fields
	// ===========================================================

	private final OpenGLESView mOpenGLESurfaceView;
	private String mText;
	private String mOriginText;

	// ===========================================================
	// Constructors
	// ===========================================================

	public TextInputWraper(final OpenGLESView pOpenGLESSurfaceView) {
		this.mOpenGLESurfaceView = pOpenGLESSurfaceView;
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	private boolean isFullScreenEdit() {
		return false;
//		final TextView textField = this.mCocos2dxGLSurfaceView.getCocos2dxEditText();
//		final InputMethodManager imm = (InputMethodManager) textField.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
//		return imm.isFullscreenMode();
	}

	public void setOriginText(final String pOriginText) {
		this.mOriginText = pOriginText;
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void afterTextChanged(final Editable s) {
		if (this.isFullScreenEdit()) {
			return;
		}

		//if (BuildConfig.DEBUG) {
			//Log.d(TAG, "afterTextChanged: " + s);
		//}
		// compare text before and after input
		int lengthBefore = this.mText.length();
		int lengthAfter = s.length();
		int lengthMin = Math.min(lengthBefore, lengthAfter);
		int lengthEqual = lengthMin;
		for (int i=0; i<lengthMin; ++i)
		{
			if (s.charAt(i) != this.mText.charAt(i))
			{
				lengthEqual = i;
				break;
			}
		}

		// delete
		if (lengthBefore > lengthEqual)
		{
			int nModified = lengthBefore - lengthEqual;
			for (; nModified > 0; --nModified)
			{
				this.mOpenGLESurfaceView.deleteBackward();
				/*
				if (BuildConfig.DEBUG) {
					Log.d(TAG, "deleteBackward");
				}
				*/
			}
		}

		// insert
		if (lengthAfter > lengthEqual)
		{
			final String insertText = s.subSequence(lengthEqual, lengthAfter).toString();
			this.mOpenGLESurfaceView.insertText(insertText);
			/*
			if (BuildConfig.DEBUG) {
				Log.d(TAG, "insertText(" + insertText + ")");
			}
			*/
		}

		this.mText = s.toString();
	}

	@Override
	public void beforeTextChanged(final CharSequence pCharSequence, final int start, final int count, final int after) {
		/*
		if (BuildConfig.DEBUG) {
			Log.d(TAG, "beforeTextChanged(" + pCharSequence + ")start: " + start + ",count: " + count + ",after: " + after);
		}
		*/
		this.mText = pCharSequence.toString();
	}

	@Override
	public void onTextChanged(final CharSequence pCharSequence, final int start, final int before, final int count) {

	}

	@Override
	public boolean onEditorAction(final TextView pTextView, final int pActionID, final KeyEvent pKeyEvent) {
		if (pActionID == EditorInfo.IME_ACTION_DONE)
		{
			this.mOpenGLESurfaceView.insertText("\n");
			return false;
		}
		
		return false;
	}

	// ===========================================================
	// Methods
	// ===========================================================

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}