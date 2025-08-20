package com.igg.bzbee.static_app;

import android.util.SparseArray;
import android.view.MotionEvent;
import android.opengl.GLSurfaceView;

import java.util.ArrayList;

public class MultipleTouchProcessor {

    public static ActivityBase activityBase = null;

    private class PointInfo {
        public int mPointId = 0;
        public int mActionType = MotionEvent.ACTION_CANCEL;
        public float mX = 0.0f;
        public float mY = 0.0f;
    }

    private SparseArray<PointInfo> mActivePointers = new SparseArray<>();

    private class TouchRunnable implements Runnable {
        private static final int MAX_ARRAY = 32;
        private int mNumberOfArrayElement = 0;
        private int mPointIdArray[] = null;
        private int mActionTypeArray[] = null;
        private float mXArray[] = null;
        private float mYArray[] = null;

        public TouchRunnable(SparseArray<PointInfo> touchPoints) {
            mNumberOfArrayElement = touchPoints.size() < MAX_ARRAY ? touchPoints.size() : MAX_ARRAY;
            mPointIdArray = new int[mNumberOfArrayElement];
            mActionTypeArray = new int[mNumberOfArrayElement];
            mXArray = new float[mNumberOfArrayElement];
            mYArray = new float[mNumberOfArrayElement];

            for (int i = 0; i < mNumberOfArrayElement; ++i) {
                PointInfo point = touchPoints.valueAt(i);
                mPointIdArray[i] = point.mPointId;
                mActionTypeArray[i] = point.mActionType;
                mXArray[i] = point.mX;
                mYArray[i] = point.mY;
            }
        }

        public void run() {
            GameEntry.touchEvent(mNumberOfArrayElement, mPointIdArray, mActionTypeArray, mXArray, mYArray);
        }
    }

    public MultipleTouchProcessor() {

    }

    public boolean process(GLSurfaceView surfaceView, MotionEvent event) {
        // get pointer index from the event object
        int pointerIndex = event.getActionIndex();

        // get pointer ID
        int pointerId = event.getPointerId(pointerIndex);
        ArrayList<Integer> removePointerIds = new ArrayList<Integer>();

        // get masked (not specific to a pointer) action
        int maskedAction = event.getActionMasked();

        switch (maskedAction) {
            case MotionEvent.ACTION_POINTER_DOWN:
            case MotionEvent.ACTION_DOWN: {
                // We have a new pointer. Lets add it to the list of pointers
                PointInfo point = new PointInfo();
                point.mPointId = pointerId;
                point.mActionType = GameEntry.TOUCH_BEGIN;
                point.mX = event.getX(pointerIndex);
                point.mY = event.getY(pointerIndex);
                mActivePointers.put(pointerId, point);

                if (activityBase != null)
                {
                    activityBase.hideVirtualKeyboard();
                }
            }
            break;
            case MotionEvent.ACTION_MOVE: {
                // a pointer was moved
                for (int size = event.getPointerCount(), i = 0; i < size; i++) {
                    PointInfo point = mActivePointers.get(event.getPointerId(i));
                    if (point != null) {
                        point.mActionType = GameEntry.TOUCH_MOVE;
                        point.mX = event.getX(i);
                        point.mY = event.getY(i);
                    }
                }
            }
            break;
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_UP: {
                PointInfo point = mActivePointers.get(pointerId);
                if (point != null) {
                    point.mActionType = GameEntry.TOUCH_END;
                    point.mX = event.getX(pointerIndex);
                    point.mY = event.getY(pointerIndex);
                    removePointerIds.add(pointerId);
                }
            }
            break;
            case MotionEvent.ACTION_CANCEL:
            {
                for (int i = 0; i < mActivePointers.size(); ++i) {
                    PointInfo point = mActivePointers.valueAt(i);
                    int idx = event.findPointerIndex(point.mPointId);

                    if (point != null && idx != -1) {
                        point.mActionType = GameEntry.TOUCH_END;
                        point.mX = event.getX(idx);
                        point.mY = event.getY(idx);
                        removePointerIds.add(point.mPointId);
                    }
                }
            }

        }
        if (mActivePointers.size() > 0) {
            surfaceView.queueEvent(new TouchRunnable(mActivePointers));
        }

        for (int i = 0; i < removePointerIds.size(); i++) {
            mActivePointers.remove(removePointerIds.get(i));
        }

        return true;
    }
}
