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

class Sys{
    friend class Mouse;
    friend class TM;
    friend class GUI;
    friend class TextureData;

    private:
    static inline int OS;
    static inline string windowTitle;
    static inline bool isFullscreen;
    static inline SDL_Color clearColor = {21, 20, 21, 255};

    static inline int maxFPS = 120;
    static inline int minFPS = 30;
    // static inline bool dynamicFPS = false;   // If this is true, the game will constantly change FPS as
                                                // the game is unable to too well able to catch up
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

    static inline std::mutex pendingJobsMutex;
    static inline std::condition_variable pendingJobsCV;
    static inline vector<weak_ptr<asyncData>> pendingJobs;

    static void processPendingJobs();

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

    static int initFont(const string& fontPath);
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
    // static void setDynamicFPS(bool dynamicFPS = false, int maxFPS = 120, int minFPS = 30);
    static int getCurrentFrame();

    static string checkError(int error);

    static inline bool isRunning = true;

    static inline SDL_Window* const& window = win;      // win pointer (SDL_Window*) exposed to the global code, but as read-only
    static inline SDL_Renderer* const& renderer = r;    // r pointer (SDL_Renderer*) exposed to the global code, but as read-only

    static inline int const& winWidth = wWidth;         // wWidth (Window Width int) exposed to the global code, but as read-only
    static inline int const& winHeight = wHeight;       // wHeight (Window Height int) exposed to the global code, but as read-only

    class Mouse {
        friend class Sys;

        protected:
            static inline SDL_Point pos = {0, 0};
            static inline bool clicked = false;
            static inline bool down = false;
            static inline uint mouseDownStartFrame = 0; // Stores the frame that the mouse started being down

            static inline const int clickDuration = 20; // Max number of frames that mouse can be down 
                                                        // and still be considered a click

        public: 
            /**
             * @brief Get the current mouse position
             * If there is some currently active container the position
             * will be relative to the container start, if
             * containerRelative is set to true, which by default is.
             * 
             * @param containerRelative If this is set to false, you will get coordinates relative to the window's (0, 0)
             * @return SDL_Point
             */
            static SDL_Point getPos(bool containerRelative = true);

            /**
             * @brief Returns boolean, was mouse clicked this frame
             * Click is considered true if the last frame the mouse was
             * down and this frame its not
             * 
             * @return bool
             */
            static bool isClicked();

            /**
             * @brief Checks if the mouse left click is pressed down
             * 
             * @return bool
             */
            static bool isDown();

            /**
             * @brief Checks if mouse is hovering some area, some rectangle
             * If containerRelative is set to true then function first checks
             * if there are any active containers, if so the coordinates are
             * calibrated as such.
             * 
             * @param area SDL_Rect of the rectangle in question
             * @param containerRelative Should the mouse be calibrated to active container
             * @return bool
             */
            static bool isHovering(const SDL_Rect& area, bool containerRelative = true);
    };

    class Keyboard{
        friend class Sys;

        protected:
            static inline SDL_Keycode keyUp;
            static inline SDL_Keycode keyDown;
            static inline string text;

            static inline bool focused = false;
            static inline bool pendingFocus = false;
            static inline bool pendingUnFocus = false;

        public:
            static SDL_Keycode getKeyUp();
            static SDL_Keycode getKeyDown();
            static string getText();
            static bool isFocused();
            static void focus();
            static void unfocus();
    };

};

#endif
// Creator: @AndrijaRD