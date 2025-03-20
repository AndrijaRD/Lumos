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

    BorderRadiusRect(): 
        top_left(0), 
        top_right(0), 
        bottom_left(0), 
        bottom_right(0) {};

    BorderRadiusRect(uint radius): 
        top_left(radius), 
        top_right(radius), 
        bottom_left(radius), 
        bottom_right(radius) {};
    
    BorderRadiusRect(
        uint tl, // top-left
        uint tr, // top-right
        uint bl, // bottom-left
        uint br // bottom-right
    ): 
        top_left(tl), 
        top_right(tr), 
        bottom_left(bl), 
        bottom_right(br) {};
};

struct paddingRect {
    int top;
    int right;
    int bottom;
    int left;

    paddingRect() : top(0), right(0), bottom(0), left(0) {}

    paddingRect(int padding)
        : top(padding), right(padding), bottom(padding), left(padding) {}

    paddingRect(int _top, int _right, int _bottom, int _left)
        : top(_top), right(_right), bottom(_bottom), left(_left) {} 
};





string color2hex(const SDL_Color& color);

class GUI{
    friend class Sys;
private:
    static inline int max_num_of_loaded_textures = 50;

    struct LoadedText {
        int frame;
        TextureData td;
        SDL_Color color;
        string title;
        int fontSize;

        LoadedText(
            int _frame = 0, 
            TextureData _td = TextureData(), 
            SDL_Color _color = SDL_COLOR_WHITE, 
            string _title = "", 
            int _fontSize = 0
        ) {
            this->frame = _frame;
            this->td = _td;
            this->color = _color;
            this->title = _title;
            this->fontSize = _fontSize;
        }
        
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
        bool firstRender = true;

        InputState(
            const string& _id, 
            const string& _value = "",
            const bool& _focused = false
        ) {
            this->id = _id;
            this->value = _value;
            this->focused = _focused;
        }

        friend ostream& operator<<(ostream& os, const InputState& state){
            os << "InputState(";
            os << "id: " << state.id;
            os << ", value: " << state.value;
            os << ", focused: " << state.focused;
            os << ", firstRender: " << state.firstRender;
            os << ")";
            return os;
        }
    };

    static inline unordered_map<string, InputState> inputStates;


    struct ContainerState {
        string id; // Holds the uniqueId
        int scrollOffset = 0;
        int scrollSpeed = 20;
        SDL_Rect dRect;
        SDL_Texture* containerTex;
        SDL_Texture* previousRenderTarget;
        int lastActiveFrame = 0;
        int contentHeight = 0;

        // For scrollbar dragging:
        bool scrollbarDragging = false;
        int scrollbarDragStartMouseY = 0;
        int scrollbarDragStartScrollOffset = 0;


        ContainerState(string _id){
            id = _id;
        }
        ContainerState(string _id, int _scrollOffset, int _scrollSpeed){
            id = _id;
            scrollOffset = _scrollOffset;
            scrollSpeed = _scrollSpeed;
        }

        friend ostream& operator<<(ostream& os, const ContainerState& state){
            os << "ContainerState(";
            os << "id: " << state.id;
            os << ", scrollOffset: " << state.scrollOffset;
            os << ", dRect: " << state.dRect;
            os << ", lastActiveFrame: " << state.lastActiveFrame;
            os << ", contentHeight: " << state.contentHeight;
            os << ")";
            return os;
        }
    };

    static inline unordered_map<string, ContainerState> containerStates;
    static void renderScrollbar(ContainerState* state);


    // Pushed styles
    static inline int pFontSize = -1;
    static inline int pTextAlignY = -1;
    static inline int pTextAlignX = -1; 
    static inline bool pAutoFocus = false;
    static inline bool pInputLock = false;
    static inline BorderRadiusRect pBorderRadius = {0, 0, 0, 0};
    static inline string pDefaultValue = "";
    static inline int pOutlineThickness = -1;
    static inline SDL_Color pOutlineColor = {0, 0, 0, 0};
    static inline paddingRect pPaddingRect = {-1, -1, -1, -1};
    static inline bool pDashLine = false;
    static inline int pDashSize = 0;
    static inline int pDashGapSize = 0;

public:
    static void drawThickLineSegment(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color, int thickness);

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
    static void setMaxNumOfLoadedTextures(const int& num = 50);
    
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

    static void startContainer(string uniqueId, SDL_Rect rect, int contentMaxHeight);
    static void endContainer(string uniqueId);
    static SDL_Point getMousePosInContainer(const string& containerId);
    static bool isRectVisibleInContainer(const std::string& containerId, const SDL_Rect& rect);
    static ContainerState* getContainerState(const string& containerId);

    static void DestroyInput(const string& inputId);
    static InputState* getInputState(const string& inputId);

    static void pushFontSize(uint fontSize);

    static void pushTextAlignY(int direction);
    static void pushTextAlignX(int direction);

    static void pushAutoFocus();
    static void pushInputLock();

    static void pushBorderRadius(uint radius);
    static void pushBorderRadius(BorderRadiusRect radiusRect);
    static void pushBorderRadius(
        uint topLeft,
        uint topRight,
        uint bottomLeft,
        uint bottomRight
    );

    static void pushDefaultValue(string value);
    static void setInputValue(string inputId, string newValue); // Override the current value

    static void pushOutlineStyle(int thickness = -1, SDL_Color color = {0, 0, 0, 0});

    static void pushPadding(int padding);
    static void pushPadding(paddingRect paddingRect);

    static void pushDashLineStyle(int dashLineSize, int dashGapSize);
};

#endif
// Creator: @AndrijaRD