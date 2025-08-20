/***************************************************************************/
/*                                                                         */
/*  afscript.h                                                             */
/*                                                                         */
/*    Auto-fitter scripts (specification only).                            */
/*                                                                         */
/*  Copyright 2013-2016 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /* The following part can be included multiple times. */
  /* Define `SCRIPT' as needed.                         */


  /* Add new scripts here.  The first and second arguments are the    */
  /* script name in lowercase and uppercase, respectively, followed   */
  /* by a description string.  Then comes the corresponding HarfBuzz  */
  /* script name tag, followed by a string of standard characters (to */
  /* derive the standard width and height of stems).                  */
  /*                                                                  */
  /* Note that fallback scripts only have a default style, thus we    */
  /* use `HB_SCRIPT_INVALID' as the HarfBuzz script name tag for      */
  /* them.                                                            */

  SCRIPT( arab, ARAB,
          "Arabic",
          HB_SCRIPT_ARABIC,
          HINTING_BOTTOM_TO_TOP,
          "\xD9\x84 \xD8\xAD \xD9\x80" ) /* 賱 丨 賭 */

  SCRIPT( armn, ARMN,
          "Armenian",
          HB_SCRIPT_ARMENIAN,
          HINTING_BOTTOM_TO_TOP,
          "\xD5\xBD \xD5\x8D" ) /* 战 諐 */

  /* there are no simple forms for letters; we thus use two digit shapes */
  SCRIPT( beng, BENG,
          "Bengali",
          HB_SCRIPT_BENGALI,
          HINTING_TOP_TO_BOTTOM,
          "\xE0\xA7\xA6 \xE0\xA7\xAA" ) /* 唰?唰?*/

  SCRIPT( cher, CHER,
          "Cherokee",
          HB_SCRIPT_CHEROKEE,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x8E\xA4 \xE1\x8F\x85 \xEA\xAE\x95" ) /* 釒?釓?戤?*/

  SCRIPT( cyrl, CYRL,
          "Cyrillic",
          HB_SCRIPT_CYRILLIC,
          HINTING_BOTTOM_TO_TOP,
          "\xD0\xBE \xD0\x9E" ) /* 芯 袨 */

  SCRIPT( deva, DEVA,
          "Devanagari",
          HB_SCRIPT_DEVANAGARI,
          HINTING_TOP_TO_BOTTOM,
          "\xE0\xA4\xA0 \xE0\xA4\xB5 \xE0\xA4\x9F" ) /* 啶?啶?啶?*/

  SCRIPT( ethi, ETHI,
          "Ethiopic",
          HB_SCRIPT_ETHIOPIC,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x8B\x90" ) /* 釈?*/

  SCRIPT( geor, GEOR,
          "Georgian (Mkhedruli)",
          HB_SCRIPT_GEORGIAN,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x83\x98 \xE1\x83\x94 \xE1\x83\x90" ) /* 醿?醿?醿?*/

  SCRIPT( geok, GEOK,
          "Georgian (Khutsuri)",
          HB_SCRIPT_INVALID,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x82\xB6 \xE1\x82\xB1 \xE2\xB4\x99" ) /* 醾?醾?獯?*/

  SCRIPT( grek, GREK,
          "Greek",
          HB_SCRIPT_GREEK,
          HINTING_BOTTOM_TO_TOP,
          "\xCE\xBF \xCE\x9F" ) /* 慰 螣 */

  SCRIPT( gujr, GUJR,
          "Gujarati",
          HB_SCRIPT_GUJARATI,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xAA\x9F \xE0\xAB\xA6" ) /* 嗒?喃?*/

  SCRIPT( guru, GURU,
          "Gurmukhi",
          HB_SCRIPT_GURMUKHI,
          HINTING_TOP_TO_BOTTOM,
          "\xE0\xA8\xA0 \xE0\xA8\xB0 \xE0\xA9\xA6" ) /* 啜?啜?喋?*/

  SCRIPT( hebr, HEBR,
          "Hebrew",
          HB_SCRIPT_HEBREW,
          HINTING_BOTTOM_TO_TOP,
          "\xD7\x9D" ) /* 诐 */

  SCRIPT( knda, KNDA,
          "Kannada",
          HB_SCRIPT_KANNADA,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xB3\xA6 \xE0\xB2\xAC" ) /* 喑?嗖?*/

  /* only digit zero has a simple shape in the Khmer script */
  SCRIPT( khmr, KHMR,
          "Khmer",
          HB_SCRIPT_KHMER,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x9F\xA0" ) /* 釤?*/

  SCRIPT( khms, KHMS,
          "Khmer Symbols",
          HB_SCRIPT_INVALID,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\xA7\xA1 \xE1\xA7\xAA" ) /* 岌?岌?*/

  /* only digit zero has a simple shape in the Lao script */
  SCRIPT( lao, LAO,
          "Lao",
          HB_SCRIPT_LAO,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xBB\x90" ) /* 嗷?*/

  SCRIPT( latn, LATN,
          "Latin",
          HB_SCRIPT_LATIN,
          HINTING_BOTTOM_TO_TOP,
          "o O 0" )

  SCRIPT( latb, LATB,
          "Latin Subscript Fallback",
          HB_SCRIPT_INVALID,
          HINTING_BOTTOM_TO_TOP,
          "\xE2\x82\x92 \xE2\x82\x80" ) /* 鈧?鈧€ */

  SCRIPT( latp, LATP,
          "Latin Superscript Fallback",
          HB_SCRIPT_INVALID,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\xB5\x92 \xE1\xB4\xBC \xE2\x81\xB0" ) /* 岬?岽?鈦?*/

  SCRIPT( mlym, MLYM,
          "Malayalam",
          HB_SCRIPT_MALAYALAM,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xB4\xA0 \xE0\xB4\xB1" ) /* 啻?啻?*/

  SCRIPT( mymr, MYMR,
          "Myanmar",
          HB_SCRIPT_MYANMAR,
          HINTING_BOTTOM_TO_TOP,
          "\xE1\x80\x9D \xE1\x80\x84 \xE1\x80\x82" ) /* 醼?醼?醼?*/

  SCRIPT( none, NONE,
          "no script",
          HB_SCRIPT_INVALID,
          HINTING_BOTTOM_TO_TOP,
          "" )

  SCRIPT( sinh, SINH,
          "Sinhala",
          HB_SCRIPT_SINHALA,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xB6\xA7" ) /* 喽?*/

  /* only digit zero has a simple (round) shape in the Tamil script */
  SCRIPT( taml, TAML,
          "Tamil",
          HB_SCRIPT_TAMIL,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xAF\xA6" ) /* 喁?*/

  /* there are no simple forms for letters; we thus use two digit shapes */
  SCRIPT( telu, TELU,
          "Telugu",
          HB_SCRIPT_TELUGU,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xB1\xA6 \xE0\xB1\xA7" ) /* 啾?啾?*/

  SCRIPT( thai, THAI,
          "Thai",
          HB_SCRIPT_THAI,
          HINTING_BOTTOM_TO_TOP,
          "\xE0\xB8\xB2 \xE0\xB9\x85 \xE0\xB9\x90" ) /* 喔?喙?喙?*/

#ifdef AF_CONFIG_OPTION_INDIC

  SCRIPT( limb, LIMB,
          "Limbu",
          HB_SCRIPT_LIMBU,
          HINTING_BOTTOM_TO_TOP,
          "o" ) /* XXX */

  SCRIPT( orya, ORYA,
          "Oriya",
          HB_SCRIPT_ORIYA,
          HINTING_BOTTOM_TO_TOP,
          "o" ) /* XXX */

  SCRIPT( sund, SUND,
          "Sundanese",
          HB_SCRIPT_SUNDANESE,
          HINTING_BOTTOM_TO_TOP,
          "o" ) /* XXX */

  SCRIPT( sylo, SYLO,
          "Syloti Nagri",
          HB_SCRIPT_SYLOTI_NAGRI,
          HINTING_BOTTOM_TO_TOP,
          "o" ) /* XXX */

  SCRIPT( tibt, TIBT,
          "Tibetan",
          HB_SCRIPT_TIBETAN,
          HINTING_BOTTOM_TO_TOP,
          "o" ) /* XXX */

#endif /* AF_CONFIG_OPTION_INDIC */

#ifdef AF_CONFIG_OPTION_CJK

  SCRIPT( hani, HANI,
          "CJKV ideographs",
          HB_SCRIPT_HAN,
          HINTING_BOTTOM_TO_TOP,
          "\xE7\x94\xB0 \xE5\x9B\x97" ) /* 鐢?鍥?*/

#endif /* AF_CONFIG_OPTION_CJK */


/* END */
