#include "Input.h"
#include <SDL3/SDL.h>

#include "../SystemManager/SM.h"

using namespace Lumos;

// Static member initialization

// --- Update ---
void Input::Keyboard::Update() {
  if (currentKeys)
    SDL_memcpy(previousKeys, currentKeys, keyCount);

  currentKeys = SDL_GetKeyboardState(&keyCount);

  // Clear text buffer every update?
  // Usually text input is per-frame or cumulative until read.
  // old logic cleared it.
  textBuffer.clear();
}

void Input::Keyboard::HandleEvent(const SDL_Event &event) {
  if (event.type == SDL_EVENT_TEXT_INPUT) {
    textBuffer += event.text.text;
  }
}

// --- State queries ---
bool Input::Keyboard::IsPressed(SDL_Scancode key) {
  return focused && currentKeys[key] && !previousKeys[key];
}

bool Input::Keyboard::IsHeld(SDL_Scancode key) {
  return focused && currentKeys[key];
}

bool Input::Keyboard::IsReleased(SDL_Scancode key) {
  return focused && !currentKeys[key] && previousKeys[key];
}

bool Input::Keyboard::AnyKey() {
  if (!focused)
    return false;
  for (int i = 0; i < keyCount; ++i)
    if (currentKeys[i])
      return true;
  return false;
}

// --- Modifiers & Layout ---
SDL_Keymod Input::Keyboard::GetMods() { return SDL_GetModState(); }

const char *Input::Keyboard::GetKeyName(SDL_Scancode key) {
  return SDL_GetScancodeName(key);
}

SDL_Scancode Input::Keyboard::GetScancodeFromName(const char *name) {
  return SDL_GetScancodeFromName(name);
}

SDL_Scancode Input::Keyboard::GetScancodeFromKey(SDL_Keycode keycode) {
  return SDL_GetScancodeFromKey(keycode, nullptr);
}

SDL_Keycode Input::Keyboard::GetKeycodeFromScancode(SDL_Scancode scancode) {
  // You can pass current modifier state, or none
  SDL_Keymod modstate =
      SDL_GetModState(); // current modifiers (Shift, Ctrl, etc.)
  bool key_event = true; // true = key event translation mode
  return SDL_GetKeyFromScancode(scancode, modstate, key_event);
}

const char *Input::Keyboard::GetKeyNameFromKeycode(SDL_Keycode keycode) {
  return SDL_GetKeyName(keycode);
}

// --- Focus control ---
void Input::Keyboard::Focus() { focused = true; }
void Input::Keyboard::Unfocus() { focused = false; }
bool Input::Keyboard::IsFocused() { return focused; }

// --- Text input ---
void Input::Keyboard::StartTextInput() { SDL_StartTextInput(Sys::window); }
void Input::Keyboard::StopTextInput() { SDL_StopTextInput(Sys::window); }
bool Input::Keyboard::IsTextInputActive() {
  return SDL_TextInputActive(Sys::window);
}
string Input::Keyboard::GetText() { return textBuffer; }

// --- Hardware info ---
int Input::Keyboard::GetKeyboardCount() {
  int count = 0;
  SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);

  if (keyboards)
    SDL_free(keyboards); // free immediately since we only need count

  return count;
}

SDL_KeyboardID Input::Keyboard::GetKeyboardID(int index) {
  int count = 0;
  SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);

  if (!keyboards)
    return 0; // failed, return invalid ID

  SDL_KeyboardID id = 0;
  if (index >= 0 && index < count)
    id = keyboards[index];

  SDL_free(keyboards);
  return id;
}

const char *Input::Keyboard::GetKeyboardName(SDL_KeyboardID id) {
  const char *name = SDL_GetKeyboardNameForID(id);
  return name ? name : "";
}

bool Input::Keyboard::HasScreenKeyboard() {
  return SDL_HasScreenKeyboardSupport();
}
