package com.igg.bzbee.static_baseutil;

import android.os.Build;

import java.util.Locale;




/*!
 * \file Helper.cpp
 *
 * \author ChenBK
 * \date 11/12/2016
 *
 */

public class SystemUtil {
    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }

    public static String getCurrentCountry() {
        return Locale.getDefault().getCountry();
    }

    public static String getAPILevel() {
        return Build.VERSION.SDK_INT + "";
    }
}
