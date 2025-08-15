#include "Sys.h"

// ─── Per-frame ─────────────────────────────────────────────────────
void Mouse::clearFrame() {
    pressedL_  = false;
    releasedL_ = false;
    pressedR_  = false;
    releasedR_ = false;
    // framesX_ will be updated in handleEvent or here if still held
    // we defer updating framesX_ until after handleEvent / at getX if you prefer

    if(!wasCursorChangedLastFrame){
        setCursor(CursorType::DEFAULT);
    }
    wasCursorChangedLastFrame = false;
}

void Mouse::handleEvent(const SDL_Event& e) {
    switch (e.type) {
      case SDL_EVENT_MOUSE_MOTION:
        mouseX_ = e.motion.x;
        mouseY_ = e.motion.y;
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
            if (!heldRawL_) {
                pressedL_ = true;
                framesL_ = 0;
            }
            heldRawL_ = true;
        }
        else if (e.button.button == SDL_BUTTON_RIGHT) {
            if (!heldRawR_) {
                pressedR_ = true;
                framesR_ = 0;
            }
            heldRawR_ = true;
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (e.button.button == SDL_BUTTON_LEFT) {
            releasedL_ = true;
            heldRawL_  = false;
            framesL_   = 0;
        }
        else if (e.button.button == SDL_BUTTON_RIGHT) {
            releasedR_ = true;
            heldRawR_  = false;
            framesR_   = 0;
        }
        break;

      default:
        break;
    }

    // After processing the event, bump the frame counters if still held
    if (heldRawL_) {
        ++framesL_;
    }
    if (heldRawR_) {
        ++framesR_;
    }
}

// ─── Position ─────────────────────────────────────────────────────
SDL_Point Mouse::getAbsolutePos() {
    // always up-to-date from last MOUSE_MOTION or SDL_GetMouseState fallback
    return SDL_Point{mouseX_, mouseY_};
}

// ─── Left button ──────────────────────────────────────────────────
bool Mouse::wasPressedLeft() {
    return pressedL_;
}
bool Mouse::wasReleasedLeft() {
    return releasedL_;
}
int Mouse::framesDownLeft() {
    return heldRawL_ ? framesL_ : 0;
}
bool Mouse::isDownLeft() {
    return heldRawL_ && (framesL_ >= holdThreshold_);
}

// ─── Right button ─────────────────────────────────────────────────
bool Mouse::wasPressedRight() {
    return pressedR_;
}
bool Mouse::wasReleasedRight() {
    return releasedR_;
}
int Mouse::framesDownRight() {
    return heldRawR_ ? framesR_ : 0;
}
bool Mouse::isDownRight() {
    return heldRawR_ && (framesR_ >= holdThreshold_);
}

// ─── Hold threshold ───────────────────────────────────────────────
void Mouse::setHoldThreshold(int f) {
    holdThreshold_ = f < 1 ? 1 : f;
}
int Mouse::getHoldThreshold() {
    return holdThreshold_;
}

bool Mouse::isHovering(SDL_Rect rect){
    SDL_Point Position = getAbsolutePos();

    return (
        Position.x >= rect.x && 
        Position.x <= rect.x + rect.w &&
        Position.y >= rect.y && 
        Position.y <= rect.y + rect.h
    );
}

void Mouse::setCursor(CursorType new_ct){
    wasCursorChangedLastFrame = true;

    if(new_ct == currentType) return;
    currentType = new_ct;

    switch (new_ct)
    {
    case CursorType::DEFAULT:
        SDL_SetCursor(defaultCursor);
        break;
    
    case CursorType::POINTER:
        SDL_SetCursor(pointerCursor);
        break;

    case CursorType::TEXT:
        SDL_SetCursor(textCursor);
        break;
    
    case CursorType::NOT_ALLOWED:
        SDL_SetCursor(notAllowedCursor);
        break;

    case CursorType::WAIT:
        SDL_SetCursor(waitCursor);
        break;
    
    default:
        break;
    }

    // Ensure cursor is visible:
    SDL_ShowCursor();
}


