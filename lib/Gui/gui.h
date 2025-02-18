#pragma once
#ifndef MySDL_GUI
#define MySDL_GUI

#include "../lib.h"
#include "../TextureManager/TM.h"

#define GUI_CURSOR_OUTSIDE     0
#define GUI_CURSOR_CLICKED     1
#define GUI_CURSOR_HOVERING    2
#define GUI_CURSOR_DRAGGING    3

#define GUI_ALIGN_LEFT      1
#define GUI_ALIGN_CENTER    2
#define GUI_ALIGN_RIGHT     3
#define GUI_ALIGN_TOP       4
#define GUI_ALIGN_BOTTOM    5

struct BorderRadiusRect {
    uint top_left;
    uint top_right;
    uint bottom_left;
    uint bottom_right;
};


string color2hex(const SDL_Color& color);

class GUI{
private:
    static inline int max_num_of_loaded_textures = 50;

    struct LoadedText {
        int frame;
        TextureData td;
        SDL_Color color;
        string title;
        int fontSize;

        LoadedText(
            int frame = 0,
            TextureData td = TextureData(),
            SDL_Color color = SDL_COLOR_WHITE,
            string title = "",
            int fontSize = 0
        ):  frame(frame), 
            td(td),
            color(color),
            title(title),
            fontSize(fontSize) {}
        
        string getId(){ return getTextID(title, fontSize, color); }

        // Comparator: sort by value
        bool operator<(const LoadedText& other) const { return frame < other.frame; }
        bool operator>(const LoadedText& other) const { return frame > other.frame; }
    };

    static inline unordered_map<string, LoadedText> loadedTexts;
    static string getTextID(string title, int fontSize, SDL_Color color);
    static LoadedText* loadNewText(const string& title, const int& fontSize, const SDL_Color& color);
    static void removeOldest();


    struct InputState {
        string id; // Holds the uniqueId
        string value; // Holds the input value
        bool focused; // Checks if the keyboard is focused on this field
        int removed = 0; // Holds how many characters needs to be removed in order for text to be within the bounds
        bool deleting = false; // Checks if the backspace has been down 
        TextureData td = TextureData(); // The text texture
        bool change = true; // Keeps track of if the value has been changed and texture needs to be reCompiled

        InputState(
            const string& id, 
            const string& value = "",
            const bool& focused = false
        ): id(id), value(value), focused(focused) {}
    };

    static inline unordered_map<string, InputState> inputStates;


    // Pushed styles
    static inline int pFontSize = -1;
    static inline int pTextAlignY = -1;
    static inline int pTextAlignX = -1; 
    static inline bool pAutoFocus = false;
    static inline bool pInputLock = false;
    static inline BorderRadiusRect pBorderRadius = {0, 0, 0, 0};

public:
    static int Button(
        const string& title, 
        const SDL_Rect& dRect,
        const SDL_Color& buttonColor = SDL_COLOR_M_GUN,
        const SDL_Color& textColor = SDL_COLOR_WHITE
    );

    static void TextBox(
        const string& title, 
        SDL_Rect& dRect, 
        const SDL_Color& color = SDL_COLOR_WHITE
    );

    static void TextDynamic(
        const string& title, 
        SDL_Rect& dRect, 
        const SDL_Color& color = SDL_COLOR_WHITE
    );

    static int calcTextWidth(
        const string& text,
        const int& textHeight
    );

    static int calcTextHeight(
        const string& text,
        const int& textWidth
    );

    static void clearLoadedTexts();
    static void setMaxNumOfLoadedTextures(const int& num);
    
    static void Rect(
        const SDL_Rect& dRect,
        const SDL_Color& color = SDL_COLOR_WHITE,
        const int thickness = -1
    );

    static void Circle(
        const SDL_Point& center, 
        const int& radius, 
        const SDL_Color& color = SDL_COLOR_WHITE, 
        const int& thickness = -1
    );

    static void Line(
        const SDL_Point& p1,
        const SDL_Point& p2,
        const SDL_Color& color,
        const uint& thickness = 1
    );

    static string Input(
        const string& uniqeId, 
        const SDL_Rect& dRect,
        const string& placeholder = "Type something...",
        const SDL_Color& background = SDL_COLOR_WHITE,
        const SDL_Color& foreground = SDL_COLOR_BLACK
    );

    static void DestroyInput(const string& uniqueId);

    static void pushFontSize(const uint& fontSize);
    static void pushTextAlignY(const int& direction);
    static void pushTextAlignX(const int& direction);
    static void pushAutoFocus();
    static void pushInputLock();
    static void pushBorderRadius(const uint& radius);
    static void pushBorderRadius(const BorderRadiusRect& radiusRect);
    static void pushBorderRadius(
        const uint& topLeft,
        const uint& topRight,
        const uint& bottomLeft,
        const uint& bottomRight
    );
};

#endif
// Creator: @AndrijaRD