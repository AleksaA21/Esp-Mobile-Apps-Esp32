/* 
 * ***********
 * Project   : util - Utilities to esp-idf
 * Programmer: Joao Lopes
 * Module    : Esp_Util - General utilities to esp-idf
 * Comments  :
 * Versions  :
 * ------- 	-------- 	-------------------------
 * 0.1.0 	01/08/18 	First version
 */

///// Includes

#define _GLIBCXX_USE_C99 // Needed for std::string -> to_string inclusion.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_system.h"
#include "nvs_flash.h"
//#include "esp_pm.h"
//#include "esp_clk.h"
//#include "esp_timer.h"

#include "log.h"

// Util

#include "esp_util.h"

////// Variables

// Log

static const char* TAG = "util";

////// Routines

void Esp_Util::esp32Initialize() {

	// Initialize the Esp32

	esp_err_t ret;

	logI("Initalizing Esp32 ...");

	// Initialize NVS

	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		logE("nvs_flash_erase ...");
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK( ret );

	// Debug

	logI("Esp32 initialized");

}

string Esp_Util::strExpand(const string& str) {

	// Expand escape chars for debug

	string ret = "";

	for(unsigned int i = 0; i<str.length(); i++) {
	    char c = str[i];
	    switch (c) {
			case '\r':
				ret.append("\\r");
				break;
			case '\n':
				ret.append("\\n");
				break;
			case '\t':
				ret.append("\\t");
				break;
			default:
				ret.append(1u, c);
				break;
		}
	}

	// logI("expand -> %s", ret.c_str());

	return ret;
}

void Esp_Util::strReplace(string& str, char c1, char c2) {

	// Replace one char to another

	for (int i = 0; i < str.length(); ++i) {
		if (str[i] == c1)
			str[i] = c2;
  }

  return;

}


bool Esp_Util::strIsNum(const string& str) {

	// String is numeric

    bool isNum = false;

    for (uint8_t i = 0; i < str.length(); i++) {
        isNum = isdigit(str[i]) || str[i] == '+' ||
                        str[i] == '.' || str[i] == '-';
        if (!isNum)
            return false;
    }

    return isNum;
}

int Esp_Util::strToInt(const string& str) {

	// Convert string para integer

    if (strIsNum(str)) {

    	return atoi(str.c_str());
    }

    return 0; // Numero invalido

}

float Esp_Util::strToFloat(const string& str) {

	// Converte string para float

	if (strIsNum(str)) {

		return atof(str.c_str());

	}

	return 0.0f;
}

float Esp_Util::roundFloat(float value, uint8_t decimals) {

	// Round float

    float factor = ROUNDF(pow(10, decimals));

    int aux = ROUNDF(value * factor);

    float ret = ((float) aux / factor);

    return ret;
}

string Esp_Util::floatToStr(float value, uint8_t decimals, bool comma) {

	// Convert float to string (dtostrf is not good)

	char str[10];
	snprintf(str, 10, "%.*f", decimals, value);
    string ret = str;

    // Comma ?

    if (comma) {
        strReplace(ret, '.', ',');
    }

    // Return

    return ret;
}

string Esp_Util::intToStr(uint32_t value) {

	// Convert int to string

	char str[10];
	snprintf(str, 10, "%d", value);
	string ret = str;
	return ret;
}

string Esp_Util::formatFloat(float value, uint8_t intPlaces, uint8_t decPlaces, bool comma)
{

	// Format float

    string ret = "";

    // Signal

    if (value < 0.0f) {
        value*=-1.0f;
        ret = '-';
    }
    // Integer

    float factor = ROUNDF(pow(10, decPlaces));

    uint32_t aux = (uint32_t) roundFloat(value * factor, 0);

    uint32_t factorInt = (uint32_t) factor;

    uint32_t valInt = (uint32_t) (aux / factorInt);

    // Decimal

    uint32_t valDec = (aux - (valInt * factorInt));

    // Return

    if (intPlaces == 0) {
        ret.append(intToStr(valInt));
    } else {
        ret.append(formatNumber(valInt, intPlaces));
    }

    ret.append(1u, (comma)?',':'.');

    ret.append(formatNumber(valDec, decPlaces));

    // Return

    return ret;

}

string Esp_Util::formatNumber(uint32_t value, uint8_t size, char insert) {

	// Format numbers

    // Putting zeroes in left

    string ret = "";

    for (uint8_t i=1; i<=size; i++) {
        uint32_t max = pow(10, i);
        if (value < max) {
            for (uint8_t j=(size - i); j>0; j--) {
                ret.append(1u, insert);
            }
            break;
        }
    }

    ret.append(intToStr(value));

    return ret;
}

string Esp_Util::formatMinutes(uint16_t minutes) {

	// Format minutes

	uint8_t hours = (minutes / (60 * 60));

	minutes = (minutes - (hours * 60));

	string ret = "";

	if (hours < 10) {
		ret = '0';
	}
	ret.append(intToStr(hours));
	ret.append(1u, ':');

	if (minutes < 10) {
	   ret.append(1u, '0');
	}
	ret.append(intToStr(minutes));

	return ret;
}

void Esp_Util::strSplit(vector<string>& tokens, const string& str, const string& delimiter) {

	// Split string with delimiters into a vector - seen it in: https://github.com/KjellKod/StringFix/blob/master/StringFix.cpp

	string::size_type start = str.find_first_not_of(delimiter, 0);
	string::size_type stop = str.find_first_of(delimiter, start);

	while (string::npos != stop || string::npos != start) {
		tokens.push_back(str.substr(start, stop - start));
		start = str.find_first_not_of(delimiter, stop);
		stop = str.find_first_of(delimiter, start);
	}
}

//////// End