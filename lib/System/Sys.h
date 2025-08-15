#pragma once
#ifndef MySDL_SYSTEM
#define MySDL_SYSTEM

#include "../lib.h"

// A macro for easyer checking of the errors, if there is something working print the error
#define CHECK_ERROR(error) \
    if ((error) != NO_ERROR) \
        std::cout << "[ERROR] " << Sys::checkError((error)) << std::endl;




/**
 * @brief This functions just checks if a given point
 * is within the bounds of some rect.
 * 
 * This should not be used to check if mouse is hovering some rect
 * as it doesnt take into account the containers offset.
 * 
 * @param point SDL_Point, relative point
 * @param rect SDL_Rect, relative rect
 * @return bool
 */
bool isPointInRect(SDL_Point point, SDL_Rect rect);


/**
 * @brief Checks if some point, which has absolute coordinates
 * ,relative to the window (0, 0), is inside some rect
 * that is a part of the container so the coordinates of the 
 * rect have to be calibrated and turned into absolute coordinates too.
 * 
 * The most reasonable usecase of this would be if user needs to
 * check if the mouse position falls onto some rect that is inside the container.
 * 
 * @return true 
 * @return false 
 */
bool isAbsolutePointInContainerRect(SDL_Point point, SDL_Rect rect);


enum class CursorType {
    DEFAULT,
    TEXT,
    POINTER,
    WAIT,
    NOT_ALLOWED
};


class Sys {
    friend class Mouse;
    friend class TM;
    friend class GUI;
    friend class TextureData;

private:
    static inline int backendIndex = 0;
    static inline int OS;
    static inline string windowTitle;
    static inline bool isFullscreen;
    static inline SDL_Color clearColor = {21, 20, 21, 255};

    static inline int FPS = 60;
    static inline uint frameCounter             = 0;
    static inline Uint64 frameStart             = 0;
    static inline Uint64 previousFrameStart     = 0;
    static inline Uint64 deltaTime              = 0;

    static inline SDL_Window* win = nullptr;
    static inline SDL_Renderer* r = nullptr;

    static inline string fontPath = "";
    static inline unordered_map<int, TTF_Font*> fontMap;

    static inline int wWidth = 0;
    static inline int wHeight = 0;

    static unordered_map<int, string> errorMap;

    static inline DebugLevels debugLevel = All;
    static void printf_info(string msg);
    static void printf_warn(string msg);
    static void printf_err(string msg);
    static void printf_err(int error_code);

public:
    static int initWindow(
        const string& windowTitle = "MySDL Window",
        const bool& fullscreen = false,
        const int& windowWidth = 1920*0.75,             // 3/4 of the screen
        const int& windowHeight = 1080*0.75             // 3/4 of the screen
    );

    static int initFont(const string& fontPath = "/home/data/DATA/ASSETS/Poppins/Poppins-Regular.ttf");
    static TTF_Font* getFont(const int& fontSize);

    static int handleEvents();
    static int presentFrame();
    static int cleanup();

    static int getOS();
    static void setClearColor(const SDL_Color& color);

    static inline thread::id mainThreadId;
    static bool isMainThread();

    static int getFPS();
    static void setFPS(const int& newFPS);
    static int getCurrentFrame();

    static string checkError(int error);

    static inline bool isRunning = true;

    static inline SDL_Window* const& window = win;      // win pointer (SDL_Window*) exposed to the global code, but as read-only
    static inline SDL_Renderer* const& renderer = r;    // r pointer (SDL_Renderer*) exposed to the global code, but as read-only

    static inline int const& winWidth = wWidth;         // wWidth (Window Width int) exposed to the global code, but as read-only
    static inline int const& winHeight = wHeight;       // wHeight (Window Height int) exposed to the global code, but as read-only
    
};



class Mouse {
    friend class Sys;
    public:
        // ─── Per-frame lifecycle ─────────────────────────────────────────
        // Call once at the start of each frame
        static void clearFrame();
    
        // Call for every SDL_Event in your Poll loop
        static void handleEvent(const SDL_Event& e);
    
        // ─── Position ────────────────────────────────────────────────────
        // Window-relative mouse coords
        static SDL_Point getAbsolutePos();

        // “Container”-relative:
        static SDL_Point getRelativePos();
    
        // ─── Left button ────────────────────────────────────────────────
        static bool isDownLeft();          // held (and beyond threshold if set)
        static bool wasPressedLeft();      // first frame down
        static bool wasReleasedLeft();     // first frame up
        static int  framesDownLeft();      // consecutive frames it’s been held
    
        // ─── Right button ───────────────────────────────────────────────
        static bool isDownRight();
        static bool wasPressedRight();
        static bool wasReleasedRight();
        static int  framesDownRight();
    
        // ─── Hold threshold ─────────────────────────────────────────────
        // how many frames “down” before isDownX() actually returns true
        static void setHoldThreshold(int frames);
        static int  getHoldThreshold();

        // ─── Helper ───────────────────────────────────────────────
        static bool isHovering(SDL_Rect rect);

        // ─── Helper ───────────────────────────────────────────────
        static void setCursor(CursorType ct);
        
    
    private:
        // Internal state
        static inline int  mouseX_      = 0;
        static inline int  mouseY_      = 0;
        static inline bool pressedL_    = false;
        static inline bool releasedL_   = false;
        static inline bool heldRawL_    = false;  // raw down
        static inline int  framesL_     = 0;
    
        static inline bool pressedR_    = false;
        static inline bool releasedR_   = false;
        static inline bool heldRawR_    = false;
        static inline int  framesR_     = 0;
    
        static inline int  holdThreshold_ = 20;    // default

        static inline SDL_Cursor* defaultCursor;
        static inline SDL_Cursor* pointerCursor;
        static inline SDL_Cursor* textCursor;
        static inline SDL_Cursor* notAllowedCursor;
        static inline SDL_Cursor* waitCursor;

        static inline CursorType currentType = CursorType::DEFAULT;
        static inline bool wasCursorChangedLastFrame = false;
    };


class Keyboard {
public:
    // ─── Per-frame methods ────────────────────────────────────────
    // Call once at the start of each frame
    static void clearFrame();

    // Call for every SDL_Event in your Poll loop
    static void handleEvent(const SDL_Event& e);

    // ─── Focus/Text-input API ─────────────────────────────────────
    // Give text-input widgets focus; starts collecting TEXT_INPUT
    static void focus();

    // Remove focus; stops TEXT_INPUT events
    static void unfocus();

    // Query whether text-input mode is active
    static bool isFocused();

    // If focused(), returns the UTF-8 characters typed *this frame*.
    // Otherwise returns an empty string.
    static std::string getText();

    // ─── Key state queries (same as before) ───────────────────────
    static bool isDown(SDL_Scancode sc);
    static bool wasPressed(SDL_Scancode sc);
    static bool wasReleased(SDL_Scancode sc);

    static bool isDown(SDL_Keycode key);
    static bool wasPressed(SDL_Keycode key);
    static bool wasReleased(SDL_Keycode key);

    // Modifiers
    static bool isCtrlDown();
    static bool isAltDown();
    static bool isShiftDown();
    
private:
    // Internal state per scancode
    static inline vector<bool> held_;
    static inline vector<bool> pressed_;
    static inline vector<bool> released_;

    // Text buffer for this frame (only valid if focused_)
    static inline std::string inputText_;

    // Are we in text-input (focused) mode?
    static inline bool focused_ = false;

    // Lazy init for vectors
    static void ensureSize();
};


#endif