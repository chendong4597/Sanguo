//HTML转义相关
#ifndef HTML_ESCAPE
#define HTML_ESCAPE

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <iostream>  
#include <assert.h>  
 
using namespace std;
 
int IsLittleEndian()
{
	int x = 1;
	if (*(char*)&x == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
 
void HtmEscapeEntityCodeToUnicode(const char* entity_code, char* html_char)
{
	int decimal_value = atoi(entity_code);
	char* uchari = (char*)&decimal_value;
	if (IsLittleEndian())
	{
		html_char[0] = uchari[0] & 0xFF;
		html_char[1] = uchari[1] & 0xFF;
	}
	else
	{
		html_char[0] = uchari[1] & 0xFF;
		html_char[1] = uchari[0] & 0xFF;
	}
}
 
void OneUnicode2UTF8(const char* unicode_char, size_t unicode_char_length,
	char* utf_char)
{
	//unicode: 0x192->110010010 ,utf8:0xC692->1100011010010010  
	int value = 0;
	memcpy(&value, unicode_char, unicode_char_length);
 
	if (value >= 0x0000 && value <= 0x007F)
	{
		utf_char[0] = unicode_char[0];
	}
	else if (value >= 0x0080 && value <= 0x07FF)
	{
		utf_char[0] = ((value >> 6) | 0xC0);
		utf_char[1] = ((value & 0x3F) | 0x80);
	}
	else if (value >= 0x0800 && value <= 0xFFFF)
	{
		utf_char[0] = ((value >> 12) | 0xE0);
		utf_char[1] = ((value >> 6 & 0x3F) | 0x80);
		utf_char[2] = ((value & 0x3F) | 0x80);
	}
	else if (value >= 0x10000 && value <= 0x10FFFF)
	{
		utf_char[0] = (value >> 18 | 0xF0);
		utf_char[1] = ((value >> 12 & 0x3F) | 0x80);
		utf_char[2] = ((value >> 6 & 0x3F) | 0x80);
		utf_char[3] = ((value & 0x3F) | 0x80);
	}
	else
	{
		cerr << "value too big." << endl;
		assert(0);
	}
}
 
static const char* kEntityNameToEntityCodeMap[] =
{
	"161", "iexcl",
	"162", "cent",
	"163", "pound",
	"164", "curren",
	"165", "yen",
	"166", "brvbar",
	"167", "sect",
	"168", "uml",
	"169", "copy",
	"170", "ordf",
	"171", "laquo",
	"173", "shy",
	"174", "reg",
	"175", "macr",
	"176", "deg",
	"177", "plusmn",
	"178", "sup2",
	"179", "sup3",
	"180", "acute",
	"181", "micro",
	"182", "para",
	"183", "middot",
	"184", "cedil",
	"185", "sup1",
	"186", "ordm",
	"187", "raquo",
	"188", "frac14",
	"189", "frac12",
	"190", "frac34",
	"191", "iquest",
	"192", "Agrave",
	"193", "Aacute",
	"194", "Acirc",
	"195", "Atilde",
	"196", "Auml",
	"197", "Aring",
	"198", "AElig",
	"199", "Ccedil",
	"200", "Egrave",
	"201", "Eacute",
	"202", "Ecirc",
	"203", "Euml",
	"204", "Igrave",
	"205", "Iacute",
	"206", "Icirc",
	"207", "Iuml",
	"208", "ETH",
	"209", "Ntilde",
	"210", "Ograve",
	"211", "Oacute",
	"212", "Ocirc",
	"213", "Otilde",
	"214", "Ouml",
	"215", "times",
	"216", "Oslash",
	"217", "Ugrave",
	"218", "Uacute",
	"219", "Ucirc",
	"220", "Uuml",
	"221", "Yacute",
	"222", "THORN",
	"223", "szlig",
	"224", "agrave",
	"225", "aacute",
	"226", "acirc",
	"227", "atilde",
	"228", "auml",
	"229", "aring",
	"230", "aelig",
	"231", "ccedil",
	"232", "egrave",
	"233", "eacute",
	"234", "ecirc",
	"235", "euml",
	"236", "igrave",
	"237", "iacute",
	"238", "icirc",
	"239", "iuml",
	"240", "eth",
	"241", "ntilde",
	"242", "ograve",
	"243", "oacute",
	"244", "ocirc",
	"245", "otilde",
	"246", "ouml",
	"247", "divide",
	"248", "oslash",
	"249", "ugrave",
	"250", "uacute",
	"251", "ucirc",
	"252", "uuml",
	"253", "yacute",
	"254", "thorn",
	"255", "yuml"
};
//1.这里可以优化的余地很大.  
const char* HtmEscapeEntityNameToEntityCode(const char* entity_name)
{
	static size_t length = sizeof(kEntityNameToEntityCodeMap) / sizeof(char*);
	for (size_t i = 0; i < length; i += 2)
	{
		if (!strcmp(entity_name, kEntityNameToEntityCodeMap[i + 1]))
		{
			return kEntityNameToEntityCodeMap[i];
		}
	}
	return NULL;
}
 
string UnescapeUTFHTMLContent(const char* str)
{
	string temp;
 
	char* pos_amp = NULL;
	char* pos_semicolon = (char*)str;
	const char* start_amp = str;
 
	int entity_length = 0;
	char entity_code[5];
	const int kMaxEntityLength = 4;
 
	char entity_name[20];
	const int kMaxEntityNameLength = 18;
 
	char unicode[3];
	char utf8[4];
 
	while (true)
	{
		if (!start_amp || !(*start_amp))
		{
			break;
		}
		pos_amp = (char *)strchr(start_amp, '&');
		if (!pos_amp)
		{
			temp.append(start_amp);
			break;
		}
		int pos_no = pos_amp - pos_semicolon;
		if (pos_no > 0)
		{
			temp.append(start_amp, pos_no);
			start_amp = pos_amp;
		}
 
		char* pos_amp1 = pos_amp + 1;
		if (!pos_amp1 || !(*pos_amp1))
		{
			string t2(start_amp);
			temp.append(start_amp);
			break;
		}
 
		if (isalpha(*pos_amp1))
		{
			pos_semicolon = strchr(pos_amp1, ';');
			if (pos_semicolon)
			{
				//调用 HtmEscapeEntityNameToEntityCode  
				memset(entity_name, 0, sizeof(entity_name));
				entity_length = ((pos_semicolon - pos_amp1) >
					kMaxEntityNameLength) ? kMaxEntityNameLength :
					(pos_semicolon - pos_amp1);
				strncpy(entity_name, pos_amp1, entity_length);
				const char* entity_code_c =
					HtmEscapeEntityNameToEntityCode(entity_name);
				if (entity_code_c)
				{
					memset(unicode, 0, sizeof(unicode));
					memset(utf8, 0, sizeof(utf8));
					HtmEscapeEntityCodeToUnicode(entity_code_c, unicode);
					OneUnicode2UTF8(unicode, 2, utf8);
					temp.append(utf8);
				}
				else
				{
					temp.append(entity_name);
				}
				//1.entity_name转换为entity_code之后再转换为utf8字符.  
				start_amp = pos_semicolon + 1;
				pos_semicolon += 1;
			}
			else
			{
				start_amp = pos_amp1;
			}
		}
		else if (*pos_amp1 == '#')
		{
			char* pos_digit = (pos_amp1 + 1);
			if (!pos_digit)
			{
				break;
			}
 
			if (isdigit(*pos_digit))
			{
				//1.需要判断数值小于10000.  
				pos_semicolon = (char *)strchr(start_amp, ';');
				if (pos_semicolon)
				{
					memset(entity_code, 0, sizeof(entity_code));
					entity_length = ((pos_semicolon - pos_digit) >
						kMaxEntityLength) ? kMaxEntityLength :
						(pos_semicolon - pos_digit);
					strncpy(entity_code, pos_digit, entity_length);
					memset(unicode, 0, sizeof(unicode));
					memset(utf8, 0, sizeof(utf8));
					HtmEscapeEntityCodeToUnicode(entity_code, unicode);
					OneUnicode2UTF8(unicode, 2, utf8);
					temp.append(utf8);
 
					start_amp = pos_semicolon + 1;
					pos_semicolon += 1;
				}
				else
				{
					start_amp = pos_digit;
				}
			}
		}
		else
		{
			string sa(start_amp, pos_amp1 - start_amp);
			temp.append(sa);
			start_amp = pos_amp1;
		}
	}
	return temp;
}

#endif


