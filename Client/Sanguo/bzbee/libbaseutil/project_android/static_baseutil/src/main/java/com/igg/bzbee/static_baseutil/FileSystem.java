package com.igg.bzbee.static_baseutil;

import android.content.res.AssetManager;

/**
 * Created by zjhlogo on 2016/6/21.
 */
public class FileSystem {
    public static native void initialize(AssetManager assets, String internalStorage, String externalStorage);
    public static native void terminate();

}
