/*!
 * \file TextInputUtil.h
 *
 * \author laizhisheng
 * \date 2017/01/18 13:56
 *
 */

#pragma once

#include "BaseType.h"
#include "Rtti.h"
#include <memory>

NS_BEGIN

typedef void (*INPUT_BOX_CALLBACK)(const char* pszContent, void* param);

class TextInputUtil
{
public:
	/**
	 * \brief The EditBoxInputMode defines the type of text that the user is allowed
	 * to enter. (copy from CCEditBox)
	 */
	enum INPUT_BOX_MODE
	{
		/**
		 * The user is allowed to enter any text, including line breaks.
		 */
		MODE_ANY = 0,
	    
		/**
		 * The user is allowed to enter an e-mail address.
		 */
		MODE_EMAIL_ADDR,

		/**
		 * The user is allowed to enter an integer value.
		 */
		MODE_NUMERIC,

		/**
		 * The user is allowed to enter a phone number.
		 */
		MODE_PHONE_NUMBER,

		/**
		 * The user is allowed to enter a URL.
		 */
		MODE_URL,

		/**
		 * The user is allowed to enter a real number value.
		 * This extends kEditBoxInputModeNumeric by allowing a decimal point.
		 */
		MODE_DECIMAL,

		/**
		 * The user is allowed to enter any text, except for line breaks.
		 */
		MODE_SINGLELINE
	};

	/**
	 * \brief The EditBoxInputFlag defines how the input text is displayed/formatted. (copy from CCEditBox)
	 */
	enum INPUT_BOX_FLAG
	{
		/**
		 * Indicates that the text entered is confidential data that should be
		 * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
		 */
		FLAG_PASSWORD = 0,

		/**
		 * Indicates that the text entered is sensitive data that the
		 * implementation must never store into a dictionary or table for use
		 * in predictive, auto-completing, or other accelerated input schemes.
		 * A credit card number is an example of sensitive data.
		 */
		FLAG_SENSITIVE,

		/**
		 * This flag is a hint to the implementation that during text editing,
		 * the initial letter of each word should be capitalized.
		 */
		FLAG_INITIAL_CAPS_WORD,

		/**
		 * This flag is a hint to the implementation that during text editing,
		 * the initial letter of each sentence should be capitalized.
		 */
		FLAG_INITIAL_CAPS_SENTENCE,

		/**
		 * Capitalize all characters automatically.
		 */
		FLAG_INITIAL_CAPS_ALL_CHARACTERS
	};

public:
	RTTI_ROOT(TextInputUtil);

	static TextInputUtil& getInstance();

	/*!
	 * \brief show platform relate input box
	 * \param strTitle title
	 * \param strContent content
	 * \param mode \see INPUT_BOX_MODE
	 * \param flag \see INPUT_BOX_FLAG
	 * \param maxLength max length of input text
	 * \param callback 
	 * \param param
	 * \return 
	 */
	virtual void showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param) = 0;
	virtual void showIMEKeyboard(bool show) = 0;
	virtual void updateNativeFrame(int left, int top, int maxWidth, int maxHeight) {}		// Coordinates are in device space

};

NS_END
