#pragma once
#ifndef Lumos_InputManager
#define Lumos_InputManager

#include "../lib.h"

#define CM Lumos::Input::Mouse
#define KM Lumos::Input::Keyboard

namespace Lumos {
namespace Input {

class Mouse {
public:
  // Button identifiers (from SDL)
  enum Button {
    LEFT = SDL_BUTTON_LEFT,
    RIGHT = SDL_BUTTON_RIGHT,
    MIDDLE = SDL_BUTTON_MIDDLE,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2
  };

  // --- Public Interface ---

  // Called once per frame to update mouse state
  static void Update();

  // True only on the frame the button was first pressed
  static bool IsPressed(Button button);

  // True while the button is held down
  static bool IsHeld(Button button);

  // True only on the frame the button was released
  static bool IsReleased(Button button);

  // True if double-clicked within short time interval
  static bool IsDoubleClicked(Button button);

  // Current absolute mouse position (relative to window)
  static SDL_Point GetAbsolutePos();

  // Position relative to the currently active UI container
  static SDL_Point GetRelativePos();

  // Change in position since last frame
  static SDL_Point GetDelta();

  // Scroll wheel movement since last frame
  static int GetScroll();

private:
  // --- Internal State (persistent between frames) ---

  // Bitmask of currently pressed mouse buttons (from SDL_GetMouseState)
  static inline Uint32 currentButtons = 0;

  // Bitmask from previous frame (used to detect press/release)
  static inline Uint32 previousButtons = 0;

  // Current mouse position (in window space)
  static inline SDL_Point position = {0, 0};

  // Mouse position from previous frame (used to compute delta)
  static inline SDL_Point lastPosition = {0, 0};

  // Change in position since last frame (x and y movement)
  static inline SDL_Point delta = {0, 0};

  // Scroll change since last frame (positive = up, negative = down)
  static inline int scrollDelta = 0;

  // Last time (in milliseconds) a click occurred for each button
  // Used to detect double-clicks.
  // Index 0 = left, 1 = right, 2 = middle, etc.
  static inline Uint32 lastClickTime[5] = {0};

  // Flags indicating whether a double-click occurred this frame for each button
  static inline bool doubleClicked[5] = {false};
};

class Keyboard {
public:
  // === FRAME UPDATE ===
  static void Update();
  static void HandleEvent(const SDL_Event &event);

  // === STATE QUERIES ===
  static bool IsPressed(SDL_Scancode key);
  static bool IsHeld(SDL_Scancode key);
  static bool IsReleased(SDL_Scancode key);
  static bool AnyKey();

  // === MODIFIERS & LAYOUT ===
  static SDL_Keymod GetMods();
  static const char *GetKeyName(SDL_Scancode key);
  static SDL_Scancode GetScancodeFromName(const char *name);
  static SDL_Scancode GetScancodeFromKey(SDL_Keycode keycode);
  static SDL_Keycode GetKeycodeFromScancode(SDL_Scancode scancode);
  static const char *GetKeyNameFromKeycode(SDL_Keycode keycode);

  // === FOCUS CONTROL ===
  static void Focus();
  static void Unfocus();
  static bool IsFocused();

  // === TEXT INPUT ===
  static void StartTextInput();
  static void StopTextInput();
  static bool IsTextInputActive();
  static std::string GetText();

  // === HARDWARE INFO ===
  static int GetKeyboardCount();
  static SDL_KeyboardID GetKeyboardID(int index);
  static const char *GetKeyboardName(SDL_KeyboardID id);
  static bool HasScreenKeyboard();

private:
  static inline const bool *currentKeys = nullptr;
  static inline Uint8 previousKeys[SDL_SCANCODE_COUNT] = {0};
  static inline int keyCount = 0;
  static inline bool focused = false;
  static inline std::string textBuffer = "";
};

}; // namespace Input
}; // namespace Lumos

#endif