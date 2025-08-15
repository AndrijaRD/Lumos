#include "Sys.h"


void Keyboard::ensureSize() {
    if (!held_.empty()) return;        // already initialized

    int count = SDL_SCANCODE_COUNT;     // this expands to the enum value SDL_SCANCODE_COUNT
    // If that macro is broken, you can also use:
    // int count = static_cast<int>(SDL_SCANCODE_COUNT);

    held_.assign(count, false);
    pressed_.assign(count, false);
    released_.assign(count, false);

    // Only clear inputText_ if we’re focused; else it’s already empty
    if (focused_) inputText_.clear();
}


void Keyboard::clearFrame() {
    // Must be called once per frame (e.g. at frame start)
    ensureSize();
    std::fill(pressed_.begin(),  pressed_.end(),  false);
    std::fill(released_.begin(), released_.end(), false);
    inputText_.clear();
}


void Keyboard::handleEvent(const SDL_Event& e) {
    ensureSize();
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN:
            if (!e.key.repeat) {
                int sc = int(e.key.scancode);
                if (!held_[sc]) pressed_[sc] = true;
                held_[sc] = true;
            }
            break;

        case SDL_EVENT_KEY_UP:
            {
                int sc = int(e.key.scancode);
                released_[sc] = true;
                held_[sc]     = false;
            }
            break;
        
        case SDL_EVENT_TEXT_INPUT:
            if (focused_) {
                inputText_ += e.text.text;
            }
            break;

      default: break;
    }
}



// QUERY FUNCTIONS --------------------------------------------------------------------------------

bool Keyboard::isDown(SDL_Scancode sc) {
    return held_[static_cast<size_t>(sc)];
}

bool Keyboard::wasPressed(SDL_Scancode sc) {
    return pressed_[static_cast<size_t>(sc)];
}

bool Keyboard::wasReleased(SDL_Scancode sc) {
    return released_[static_cast<size_t>(sc)];
}



// QUERY FUNCTIONS --- KEY CODE OVERLOAD ----------------------------------------------------------

bool Keyboard::isDown(SDL_Keycode key) {
    SDL_Scancode sc = SDL_GetScancodeFromKey(key, /*modstate=*/ nullptr);
    return isDown(sc);
}

bool Keyboard::wasPressed(SDL_Keycode key) {
    SDL_Scancode sc = SDL_GetScancodeFromKey(key, /*modstate=*/ nullptr);
    return wasPressed(sc);
}

bool Keyboard::wasReleased(SDL_Keycode key) {
    SDL_Scancode sc = SDL_GetScancodeFromKey(key, /*modstate=*/ nullptr);
    return wasReleased(sc);
}



// MODIFIER HELPERS -------------------------------------------------------------------------------

bool Keyboard::isCtrlDown() {
    return isDown(SDL_SCANCODE_LCTRL) || isDown(SDL_SCANCODE_RCTRL);
}
bool Keyboard::isAltDown() {
    return isDown(SDL_SCANCODE_LALT)  || isDown(SDL_SCANCODE_RALT);
}
bool Keyboard::isShiftDown() {
    return isDown(SDL_SCANCODE_LSHIFT)|| isDown(SDL_SCANCODE_RSHIFT);
}



// INPUT TEXT AND FOCUS CONTROLL ------------------------------------------------------------------

void Keyboard::focus() {
    if (!focused_) {
        focused_ = true;
        SDL_StartTextInput(Sys::window);
    }
}

void Keyboard::unfocus() {
    if (focused_) {
        focused_ = false;
        SDL_StopTextInput(Sys::window);
    }
}

bool Keyboard::isFocused() {
    return focused_;
}

string Keyboard::getText() {
    // Return text only when focused; otherwise empty
    return focused_ ? inputText_ : std::string{};
}