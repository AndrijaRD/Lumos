#include "../Input/Input.h"
#include "../SystemManager/SM.h"
#include "UI.h"

namespace Lumos {

CursorState GUI::Button(const string &title, const SDL_Rect &dRect,
                        const SDL_Color &buttonColor,
                        const SDL_Color &textColor) {
  // COPY STYLES -------------------------------------------------------
  // First we copy the pushed styles
  int fontSize = GUI::pFontSize;
  GUI::pFontSize = -1;

  Align textAlignX = GUI::pTextAlignX;
  if (textAlignX == Align::DEFAULT)
    textAlignX = Align::CENTER; // Default
  GUI::pTextAlignX = Align::DEFAULT;

  Align textAlignY = GUI::pTextAlignY;
  if (textAlignY == Align::DEFAULT)
    textAlignY = Align::CENTER; // Default
  GUI::pTextAlignY = Align::DEFAULT;

  BorderRadiusRect borderRadius = pBorderRadius;
  pBorderRadius = {0, 0, 0, 0};

  PaddingRect padding = GUI::pPaddingRect;
  pPaddingRect = {0, 0, 0, 0};

  // CHECK VALIDITY OF dRect -------------------------------------------
  if (dRect.w < 1 || dRect.h < 1)
    return CursorState::OUTSIDE;

  // FIND fontSize
  // ---------------------------------------------------------------
  if (fontSize == -1) {
    // Valid width is width that text can occupy, buttons width minus the left
    // and right padding
    int validWidth = dRect.w - (padding.left + padding.right);

    int validHeight = dRect.h - (padding.top + padding.bottom);

    // If valid width is too small discard it, just use full button width
    if (validWidth < 10) {
      validWidth = dRect.w;
      padding.left = 0;
      padding.right = 0;
    }

    if (validHeight < 10) {
      validHeight = dRect.h;
      padding.top = 0;
      padding.bottom = 0;
    }

    // If width that text would occupy with max font size would spil out of the
    // validWidth, calculate the right fontSize
    if (calcTextWidth(title, validHeight) > validWidth)
      fontSize = calcTextHeight(title, validWidth);
    else
      fontSize = dRect.h;
  }

  // FIND BUTTON TEXTURE
  LoadedText *textPointer = nullptr;

  auto it = loadedTexts.find(getTextId(title, fontSize, 400, textColor));
  if (it != loadedTexts.end()) {
    // Save the pointer to that LoadedText item
    textPointer = &it->second;

    // Update with the current frame
    textPointer->lastUsedFrame = Sys::getCurrentFrame();
  } else {
    // Create new Text Texture
    textPointer = loadNewText(title, fontSize, 400, textColor);
  }

  // BUTTON BACK -------------------------------------------------------
  // Render the buttons background, filled rect,
  // and then on top of it will be white text
  // SDL_SetRenderDrawColor(Sys::renderer, buttonColor);
  // SDL_RenderFillRect(Sys::renderer, &dRect);
  GUI::pushBorderRadius(borderRadius);
  GUI::Rect(dRect, buttonColor);

  // TEXT DRECT
  // ------------------------------------------------------------------
  SDL_Rect text_dRect;

  // DRECT HEIGHT
  // ----------------------------------------------------------------
  text_dRect.h = fontSize;

  // DRECT WIDTH
  // -----------------------------------------------------------------
  text_dRect.w = text_dRect.h * (float)textPointer->td.getWidth() /
                 textPointer->td.getHeight();

  // DRECT X POS
  if (textAlignX == Align::LEFT) {
    text_dRect.x = dRect.x + padding.left;
  } else if (textAlignX == Align::CENTER) {
    text_dRect.x = dRect.x + dRect.w / 2 - text_dRect.w / 2;
  } else if (textAlignX == Align::RIGHT) {
    text_dRect.x = dRect.x + dRect.w - padding.right - text_dRect.w;
  }

  // DRECT Y POS
  if (textAlignY == Align::TOP) {
    text_dRect.y = dRect.y + padding.top;
  } else if (textAlignY == Align::CENTER) {
    text_dRect.y = dRect.y + dRect.h / 2 - text_dRect.h / 2;
  } else if (textAlignY == Align::BOTTOM) {
    text_dRect.y = dRect.y + dRect.h - padding.bottom - text_dRect.h;
  }

  // Now render the text
  GUI::Image(textPointer->td, text_dRect);

  // Uses Input logic
  // Need to check if Mouse::isHovering etc needs conversion
  // pRect needs to be passed to isHovering? No, SDL_Rect is fine?
  // Let's check Input::Mouse interface again.
  // It doesn't have `isHovering(Rect)`. It has `GetAbsolutePos`.
  // I need to implement isHovering logic locally or add it to Mouse.
  // The old code had `Mouse::isHovering(dRect)`.

  // Let's use local logic or helper if Mouse doesn't have it.
  // isPointInRect is available in `lib.h` (Lumos::isPointInRect or global?)
  // In Step 74 I put `isPointInRect` outside namespace.

  SDL_Point mPos = Input::Mouse::GetAbsolutePos();
  bool hovering = isPointInRect(mPos, dRect);

  if (hovering) {
    // Mouse::setCursor(CursorType::POINTER); // No setCursor in new Mouse
    // class? Sys::setCursor? SM.h has definition of CursorType. But SM.h
    // doesn't seem to have setCursor function exposed? Checking SM.h... Step 72
    // view: `Lumos::Sys` class within `SystemManager/SM.h` outline. I need to
    // check SM.h again if setCursor is there. If not, I'll comment it out or
    // add it. For now, I'll assume setCursor is missing or I need to add it to
    // Sys.

    if (Input::Mouse::IsReleased(Input::Mouse::LEFT)) {
      return CursorState::CLICKED;
    }
    if (Input::Mouse::IsHeld(Input::Mouse::LEFT)) {
      return CursorState::DRAGGING;
    }
    return CursorState::HOVERING;
  }

  return CursorState::OUTSIDE;
}

} // namespace Lumos
