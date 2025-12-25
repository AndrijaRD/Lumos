#include "Input.h"
#include <SDL3/SDL.h>

namespace Lumos {
namespace Input {

// ===========================
// Mouse update logic
// ===========================
void Input::Mouse::Update()
{

    // --- 1. Check if window is focused ---
    Uint32 windowFlags = SDL_GetWindowFlags(SDL_GL_GetCurrentWindow());
    bool windowFocused = (windowFlags & SDL_WINDOW_INPUT_FOCUS);
 
    if (!windowFocused) {
        // If window lost focus — reset all input states
        previousButtons = 0;
        currentButtons  = 0;
 
        delta = {0, 0};
        scrollDelta = 0;
 
        for (int i = 0; i < 5; ++i)
            doubleClicked[i] = false;
 
        // Optionally, reset lastClickTime so double-clicks don’t fire right after refocus
        memset(lastClickTime, 0, sizeof(lastClickTime));
 
        // Don't process any further
        return;
    }

    // Save previous state
    previousButtons = currentButtons;
    lastPosition    = position;
    for (int i = 0; i < 5; ++i)
        doubleClicked[i] = false; // reset each frame
    scrollDelta = 0;

    // Get current mouse state and position
    float x, y;
    currentButtons = SDL_GetMouseState(&x, &y);
    position = {int(x), int(y)};

    // Compute delta movement
    delta.x = position.x - lastPosition.x;
    delta.y = position.y - lastPosition.y;

    // 5. Process scroll events
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_MOUSE_WHEEL)
            scrollDelta += e.wheel.y;
    }
}

// ===========================
// Mouse button states
// ===========================
bool Input::Mouse::IsPressed(Button button)
{
    return (currentButtons & SDL_BUTTON_MASK(button)) &&
           !(previousButtons & SDL_BUTTON_MASK(button));
}

bool Input::Mouse::IsHeld(Button button)
{
    return (currentButtons & SDL_BUTTON_MASK(button));
}

bool Input::Mouse::IsReleased(Button button)
{
    return !(currentButtons & SDL_BUTTON_MASK(button)) &&
            (previousButtons & SDL_BUTTON_MASK(button));
}

// ===========================
// Double click detection
// ===========================
bool Input::Mouse::IsDoubleClicked(Button button)
{
    int index = button - 1;
    if (index < 0 || index >= 5) return false;

    Uint32 now = SDL_GetTicks();
    Uint32 diff = now - lastClickTime[index];

    // if pressed this frame and within 250ms = double click
    if (IsPressed(button))
    {
        if (diff < 250)
            doubleClicked[index] = true;

        lastClickTime[index] = now;
    }

    return doubleClicked[index];
}

// ===========================
// Position getters
// ===========================
SDL_Point Input::Mouse::GetAbsolutePos()
{
    return position;
}

SDL_Point Input::Mouse::GetRelativePos()
{
    // Adjust relative to current UI container if needed
    // (replace with your UI system’s variable)
    SDL_Point relative = position;

    if (/* UI::currentDivStartingPosition exists */ false)
    {
        // Example:
        // relative.x -= UI::currentDivStartingPosition.x;
        // relative.y -= UI::currentDivStartingPosition.y;
    }

    return relative;
}

SDL_Point Input::Mouse::GetDelta()
{
    return delta;
}

// ===========================
// Scroll handling
// ===========================
int Input::Mouse::GetScroll()
{
    return scrollDelta;
}


} // namespace Input
} // namespace Lumos
