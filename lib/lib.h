#pragma once
#ifndef MySDL
#define MySDL

#include <iostream>             // std
#include <filesystem>           // std::filesystem
#include <SDL2/SDL.h>           // Global SDL
#include <SDL2/SDL_ttf.h>       // Fonts
#include <SDL2/SDL_image.h>     // IMG_Load
#include <vector>               // vector<>
#include <algorithm>            // For std::remove
#include <libpq-fe.h>           // For PosgreSQL DB
#include <unordered_map>        // For unordered_map<type, type>
#include <set>                  // For sets (gui.h)
#include <future>               // For promises, async (TM)
#include <thread>               // For handling threads
#include <any>                  // std::any
#include <opencv2/opencv.hpp>   // Opencv, Mat
#include <opencv2/core.hpp>     // Opencv Core
#include <opencv2/imgproc.hpp>  // Opencv Color

#include "LumosForbidSDL.h"


using namespace std;
namespace fs = std::filesystem;

#define VERSION 2.5



// OS ---------------------------------------------------------------------------
#define OS_LINUX    0
#define OS_WINDOWS  1


// COLORS -----------------------------------------------------------------------
#define SDL_COLOR_WHITE         {255,   255,    255,    255}
#define SDL_COLOR_BLACK         {0,     0,      0,      255}
#define SDL_COLOR_BLUE          {0,     0,      255,    255}
#define SDL_COLOR_GREEN         {0,     255,    0,      255}
#define SDL_COLOR_RED           {255,   0,      0,      255}
#define SDL_COLOR_PURPLE        {128,   0,      128,    255}
#define SDL_COLOR_YELLOW        {255,   255,    0,      255}
#define SDL_COLOR_CYAN          {0,     255,    255,    255}
#define SDL_COLOR_PINK          {255,   192,    203,    255}
#define SDL_COLOR_SEMI_TRANSP   {120,   120,    120,    127}
#define SDL_COLOR_TRANSP        {255,   255,    255,    0  }
#define SDL_COLOR_GRAY          {120,   120,    120,    255}
#define SDL_COLOR_M_GUN         {32,    32,     45,     255}

#define WIN_COLOR SDL_Color{31, 31, 31, 255}
#define BG_COLOR  SDL_Color{46, 46, 46, 255}
#define FG_COLOR  SDL_Color{62, 62, 62, 255}
#define ACT_COLOR SDL_Color{88, 88, 88, 255}
#define TXT_COLOR SDL_Color{224, 224, 224, 225}

#define MODERN_BLACK    SDL_Color{20, 19, 28, 255}
#define MODERN_BLUE     SDL_Color{42, 67, 161, 255}
#define MODERN_GREEN    SDL_Color{31, 83, 28, 255}
#define MODERN_RED      SDL_Color{174, 41, 63, 255}

inline Uint32 toUint32(const SDL_Color& color) {
    return (static_cast<Uint32>(color.r) << 24) |
           (static_cast<Uint32>(color.g) << 16) |
           (static_cast<Uint32>(color.b) << 8)  |
            static_cast<Uint32>(color.a);
}


// ERRORS ----------------------------------------------------------------------
#define NO_ERROR                        0x00

#define SYS_SDL_INIT_ERROR              0x01
#define SYS_FONT_INIT_ERROR             0x02
#define SYS_FONT_PATH_ERROR             0x03
#define SYS_WINDOW_INIT_ERROR           0x04
#define SYS_RENDERER_INIT_ERROR         0x05
#define SYS_FPS_TOO_LOW                 0x06
#define SYS_FPS_TOO_HIGH                0x07
#define SYS_FONT_NOT_INITED             0x08
//  SYS RESERVED                        0x1f

#define TM_SURFACE_CREATE_ERROR         0x20
#define TM_SURFACE_CONVERT_ERROR        0x21
#define TM_TEXTURE_CREATE_ERROR         0x22
#define TM_TEXTURE_SET_BLENDMODE_ERROR  0x23
#define TM_TEXTURE_UPDATE_ERROR         0x24
#define TM_GOT_NULLPTR_TEX              0x25
#define TM_INVALID_DRECT                0x26        // SDL_Rect dRect           - Invalid
#define TM_RCPY_FAILED                  0x27        // SDL_RenderCopy           - Failed
#define TM_SRT_FAILED                   0x28        // SDL_SetRenderTarget      - Failed
#define TM_SRDC_FAILED                  0x29        // SDL_SetRenderDrawColor   - Failed
#define TM_RCLR_FAILED                  0x2a        // SDL_RenderClear          - Failed
#define TM_STSM_FAILED                  0x2b        // SDL_SetTextureScaleMode  - Failed
#define TM_FILL_RECT_ERROR              0x2c        // SDL_RenderFillRect       - Failed
#define TM_INVALID_LINE_LENGTH          0x2d
#define TM_MAT_INVALID_FORMAT           0x2e
//  TM RESERVED                         0x3f

#define DB_CONNECTION_ERROR             0x40
#define DB_PREPARE_ERROR                0x41
#define DB_EXEC_RESULT_ERROR            0x42
#define DB_EXEC_NOT_PREPARED_ERROR      0x43
#define DB_INVALID_RESULT               0x44
#define DB_INVALID_ROW_COLUMN           0x45
#define DB_INVALID_RES_VALUE            0x46
#define DB_EMPTY_STATEMENT_PARAM        0x47
#define DB_REPREPARATION                0x48
//  DB RESERVED                         0x5f

// GENERAL PURPOUSE
#define INVALID_ARGUMENTS_PASSED        0x60

#define UNKNOWN_ERROR                   -1



// DATE ------------------------------------------------------------------------
struct dateT {
    uint year;
    uint month;
    uint day;

    dateT(): year(2024), month(12), day(24) {};
    dateT(uint y, uint m, uint d): year(y), month(m), day(d) {};
};

/**
 * Handles << operator overload for dateT type.
 */
inline std::ostream& operator<<(std::ostream& os, const dateT& date){
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << date.year << "-"
        << std::setw(2) << std::setfill('0') << date.month << "-"
        << std::setw(2) << std::setfill('0') << date.day;
    os << oss.str();
    return os;
}

/** String to dateT
 * 
 * Takes "yyyy-mm-dd" format and returns dateT.
 */
inline dateT string2date(const string& dateStr){
    dateT date;
    sscanf(dateStr.c_str(), "%4u-%2u-%2u", &date.year, &date.month, &date.day);
    return date;
}

/** dateT to string
 * 
 * Takes dateT and returns string in "yyyy-mm-dd" format.
 */
inline string date2string(const dateT& date){
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << date.year << "-"
        << std::setw(2) << std::setfill('0') << date.month << "-"
        << std::setw(2) << std::setfill('0') << date.day;
    return oss.str();
}

// TIME ------------------------------------------------------------------------
struct timeT {
    uint hour;
    uint minute;
    uint second;

    timeT(): hour(0), minute(0), second(0) {};
    timeT(uint h, uint m, uint s): hour(h), minute(m), second(s) {};
};

/**
 * Handles << operator overload for timeT type.
 */
inline std::ostream& operator<<(std::ostream& os, const timeT time){
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << time.hour << ":"
        << std::setw(2) << std::setfill('0') << time.minute << ":"
        << std::setw(2) << std::setfill('0') << time.second;
    os << oss.str();
    return os;
}

/** String to timeT
 * 
 * Takes "hh:mm:ss" format and returns timeT.
 */
inline timeT string2time(const string& timeStr){
    timeT time;
    sscanf(timeStr.c_str(), "%2u:%2u:%2u", &time.hour, &time.minute, &time.second);
    return time;
}

/** timeT to string
 * 
 * Takes timeT and returns string in "hh:mm:ss" format.
 */
inline string time2string(const timeT& time){
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << time.hour << ":"
        << std::setw(2) << std::setfill('0') << time.minute << ":"
        << std::setw(2) << std::setfill('0') << time.second;
    return oss.str();
}




// Get Timestamp --------------------------------------------------------------
inline int getTimestamp(const dateT& date, const timeT& time){
    std::tm t = {};
    // mktime expects years since 1900, months 0–11
    t.tm_year = static_cast<int>(date.year) - 1900;
    t.tm_mon  = static_cast<int>(date.month) - 1;
    t.tm_mday = static_cast<int>(date.day);
    t.tm_hour = static_cast<int>(time.hour);
    t.tm_min  = static_cast<int>(time.minute);
    t.tm_sec  = static_cast<int>(time.second);
    
    // std::mktime converts local time to time_t (seconds since epoch)
    // Note: mktime uses the current locale’s timezone.
    time_t timestamp = std::mktime(&t);
    return static_cast<int>(timestamp);
}

// Size -----------------------------------------------------------------------
struct Size {
    int width;
    int height;

    Size(): width(0), height(0) {};
    Size(int w, int h): width(w), height(h) {};
};

inline ostream& operator<<(ostream& os, const Size& size){
    os << "Size(w: " << size.width << ", h: " << size.height << ")";
    return os;
}


// SDL_RECT -------------------------------------------------------------------
inline ostream& operator<<(ostream& os, const SDL_Rect& dRect){
    os << "Rect(" << dRect.x;
    os << ", " << dRect.y;
    os << ", " << dRect.w;
    os << ", " << dRect.h << ")";
    return os;
}

// SDL_POINT ------------------------------------------------------------------
inline ostream& operator<<(ostream& os, const SDL_Point& point){
    os << "Point(" << point.x;
    os << ", " << point.y << ")";
    return os;
}


// SDL_COLOR ------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os, const SDL_Color& color){
    os << "SDL_Color(" << static_cast<int>(color.r);
    os << ", " << static_cast<int>(color.g);
    os << ", " << static_cast<int>(color.b);
    os << ", " << static_cast<int>(color.a) << ")";
    return os;
}

/** 
 * @brief Converts SDL_Color into hex representation of the color "#rrggbbaa"
 * 
 * @param color SDL_Color
 * @return string formated "#rrggbbaa"
 */
inline string sdlColor2hex(SDL_Color color){
    std::ostringstream oss;
    oss << '#'
        << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(color.r)
        << std::setw(2) << static_cast<int>(color.g)
        << std::setw(2) << static_cast<int>(color.b)
        << std::setw(2) << static_cast<int>(color.a);
    // reset format flags if you care about subsequent ostream use…
    return oss.str();
}

/** 
 * @brief Converts Hex Color String into SDL_Color object.
 * 
 * Valid formats are "#rgb", "#rgba", "#rrggbb" and "#rrggbbaa"
 * 
 * @param hex Hex code of the color
 * @return SDL_Color object
 */
inline SDL_Color hex2sdlColor(string hex){
    SDL_Color out{0,0,0,255};

    // Must start with '#'
    if (hex.size() < 4 || hex[0] != '#')
        return out;

    // drop the '#'
    hex.erase(hex.begin());

    auto hexToByte = [&](char c) -> int {
        c = static_cast<char>(std::tolower(c));
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        return 0;
    };

    auto parsePair = [&](int idx) -> Uint8 {
        return static_cast<Uint8>((hexToByte(hex[idx]) << 4) | hexToByte(hex[idx+1]));
    };

    switch (hex.size()) {
      case 3:
        // rgb -> rr,gg,bb and alpha=FF
        out.r = static_cast<Uint8>(hexToByte(hex[0]) * 0x11);
        out.g = static_cast<Uint8>(hexToByte(hex[1]) * 0x11);
        out.b = static_cast<Uint8>(hexToByte(hex[2]) * 0x11);
        out.a = 255;
        break;
      case 4:
        // rgba -> rr,gg,bb,aa
        out.r = static_cast<Uint8>(hexToByte(hex[0]) * 0x11);
        out.g = static_cast<Uint8>(hexToByte(hex[1]) * 0x11);
        out.b = static_cast<Uint8>(hexToByte(hex[2]) * 0x11);
        out.a = static_cast<Uint8>(hexToByte(hex[3]) * 0x11);
        break;
      case 6:
        // rrggbb -> rr,gg,bb and alpha=FF
        out.r = parsePair(0);
        out.g = parsePair(2);
        out.b = parsePair(4);
        out.a = 255;
        break;
      case 8:
        // rrggbbaa
        out.r = parsePair(0);
        out.g = parsePair(2);
        out.b = parsePair(4);
        out.a = parsePair(6);
        break;
      default:
        // leave default black
        break;
    }
    return out;
}



// DEBUG ENUM --------------------------------------------------------------------
enum DebugLevels {
    All,
    Warnings,
    Errors,
    Silent
};


// TM ENUM --------------------------------------------------------------------
enum TM_Functions {
    LOAD_TEXTURE,
    CREATE_TEXT_TEXTURE,
    COPY_TEXTURE,
    RESIZE_TEXTURE,
    EXPORT_TEXTURE,
    TO_TEXTURE,
    CONVERT_TO_TEX,
    CONVERT_TEX_TO
};

// ASYNC ----------------------------------------------------------------------
struct asyncData {
    bool                     done = false;
    TM_Functions             pendingFunction;
    std::vector<std::any>    params;  // the arguments
    std::any                 result;  // the return value
};
using AsyncDataPtr = std::shared_ptr<asyncData>;


// OVERLOAD

inline SDL_Point operator+(const SDL_Point& p1, const SDL_Point& p2) {
    return SDL_Point{ p1.x + p2.x, p1.y + p2.y };
}

inline SDL_Rect operator+(const SDL_Rect& rect, const SDL_Point& point) {
    return SDL_Rect{ 
        rect.x + point.x, 
        rect.y + point.y,
        rect.w,
        rect.h
    };
}

inline SDL_Rect operator*(const SDL_Rect& rect, float scale) {
    return SDL_Rect{ 
        static_cast<int>(rect.x * scale), 
        static_cast<int>(rect.y * scale),
        static_cast<int>(rect.w * scale),
        static_cast<int>(rect.h * scale)
    };
}

#endif


/*
TODO:

Input controll using arrows
Input Ender button controll
Input mouse controll
Input shortcuts, ctrl+c, ctrl+v, ctrl+x, ctrl+a

Sys Keyboard Keypress controll

*/

// Creator: @AndrijaRD