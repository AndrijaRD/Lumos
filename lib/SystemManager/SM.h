#pragma once
#ifndef Lumos_SystemManager
#define Lumos_SystemManager

#include "../lib.h"

// A macro for easyer checking of the errors, if there is something working
// print the error
#define CHECK_ERROR(error)                                                     \
  if ((error) != NO_ERROR)                                                     \
    std::cout << "[ERROR] " << Lumos::Sys::checkError((error)) << std::endl;

enum class CursorType { DEFAULT, TEXT, POINTER, WAIT, NOT_ALLOWED };

namespace Lumos {
class Sys {
  friend class Mouse;
  friend class TM;
  friend class GUI;
  friend class TextureData;

private:
  static inline string windowTitle;
  static inline int wWidth = 0;
  static inline int wHeight = 0;
  static inline bool isFullscreen = false;

  static inline SDL_Color clearColor = {21, 20, 21, 255};

  static inline int detectedOS = -1;
  static inline int backendIndex = 0;

  static inline int FPS = 60; // Frames Per Second
  static inline uint64_t frameCounter =
      0; // Counts number of frames since the start of the App
  static inline uint64_t frameStart = 0;         // Time of current Frame Start
  static inline uint64_t previousFrameStart = 0; // Time of previous Frame Start
  static inline uint64_t deltaTime = 0; // Time it took to compleate the Frame

  static inline SDL_Window *win = nullptr;
  static inline SDL_Renderer *r = nullptr;

  static inline thread::id _mainThreadId;

  static unordered_map<int, string> errorMap;
  static inline DebugLevels debugLevel = All;

  // Private helpers
  static void printf_info(string msg);
  static void printf_warn(string msg);
  static void printf_err(string msg);
  static void printf_err(int error_code);

public:
  // INIT FUNCTIONS
  // =============================================================================
  static int
  initWindow(const string &windowTitle = "MySDL App",
             const bool &fullscreen = false,
             const int &windowWidth = 1920 * 0.75, // 3/4 of the screen
             const int &windowHeight = 1080 * 0.75 // 3/4 of the screen
  );

  // LOOP FUNCTIONS
  // =============================================================================
  static int handleEvents(); // Called at the start of each cycle, frame
  static int presentFrame(); // Called at the end of each cycle, frame
  static int
  cleanup(); // Called after exiting the Main Loop, at the end of the program

  // INTERNAL SETTINGS
  // ==========================================================================
  static int getFPS();
  static void setFPS(int newFPS = 60);
  static int getCurrentFrame();

  static inline bool isRunning = true; // This the main App State boolean

  static int getOS();
  static void setClearColor(const SDL_Color &color);

  static string checkError(int error);
  static void logError(int error) {
    printf_err(error);
  } // Public wrapper maybe?

  // Font
  static TTF_Font *getFont(int fontSize);

  // EXPOSED VARIABLES
  // ==========================================================================
  static inline SDL_Window *const &window = win;   // win pointer (SDL_Window*)
  static inline SDL_Renderer *const &renderer = r; // r pointer (SDL_Renderer*)

  static inline int const &winWidth = wWidth;   // wWidth (Window Width int)
  static inline int const &winHeight = wHeight; // wHeight (Window Height int)

  static inline thread::id const &mainThreadId =
      _mainThreadId; // Main Thread ID
  static bool isMainThread();
};

// Utility functions moved to namespace scope or kept global?
// Keeping global for now to match macros, but put in namespace if preferred.
// Assuming global scope usage in other files based on previous `isPointInRect`
// location.
} // namespace Lumos

// Global helpers if needed, or put them in Lumos namespace
bool isPointInRect(SDL_Point point, SDL_Rect rect);

#endif