#include "../Input/Input.h"
#include "../SystemManager/SM.h"
#include "UI.h"

namespace Lumos {

string GUI::Input(const string &uniqueId, const SDL_Rect &dRect,
                  const string &placeholder, const SDL_Color &background,
                  const SDL_Color &foreground) {
  // COPY STYLES -------------------------------------------------------
  // First we copy the pushed styles
  int fontSize = GUI::pFontSize;
  GUI::pFontSize = -1;

  Align textAlignX = GUI::pTextAlignX;
  if (textAlignX == Align::DEFAULT)
    textAlignX = Align::LEFT; // Default
  GUI::pTextAlignX = Align::DEFAULT;

  Align textAlignY = GUI::pTextAlignY;
  if (textAlignY == Align::DEFAULT)
    textAlignY = Align::CENTER; // Default
  GUI::pTextAlignY = Align::DEFAULT;

  bool autoFocus = GUI::pAutoFocus;
  GUI::pAutoFocus = false;

  bool inputLock = GUI::pInputLock;
  GUI::pInputLock = false;

  string defaultValue = GUI::pDefaultValue;
  GUI::pDefaultValue = "";

  BorderRadiusRect borderRadius = pBorderRadius;
  pBorderRadius = {0, 0, 0, 0};

  int outlineThickness = pOutlineThickness;
  pOutlineThickness = -1;

  // if outlineThickness is -1 then that means outline thickness
  // was not set, not pushed, so Input should use default thickness
  if (outlineThickness == -1)
    outlineThickness = 1;

  SDL_Color outlineColor = pOutlineColor;
  pOutlineColor = {0, 0, 0, 0};

  // if color.a is 0, then that means that outline color was not pushed
  // its default value, so default for input outline is black
  if (outlineColor.a == 0)
    outlineColor = SDL_COLOR_BLACK;

  PaddingRect paddingRect = pPaddingRect;
  pPaddingRect = {-1, -1, -1, -1};

  // If paddingRect is filled with -1 then those values should be
  // replaced by default values for input element
  if (paddingRect.top == -1)
    paddingRect.top = 3;
  if (paddingRect.right == -1)
    paddingRect.right = 5;
  if (paddingRect.bottom == -1)
    paddingRect.bottom = 3;
  if (paddingRect.left == -1)
    paddingRect.left = 5;

  // FIND STATE --------------------------------------------------------
  // Get the pointer to the inputs state using uniqueId
  InputState *state = nullptr;
  auto it = inputStates.find(uniqueId);
  if (it != inputStates.end()) {
    state = &it->second;
  } else {
    // If the state doesnt exist, create it
    inputStates.insert({uniqueId, InputState(uniqueId)});
    state = &inputStates.at(uniqueId);
  }

  if (inputLock) {
    autoFocus = false;
    // if(state->focused) Input::Keyboard::Unfocus(); // Unfocus logic?
    // The Keyboard class has Focus, Unfocus.
    if (state->focused)
      Input::Keyboard::Unfocus();
    state->focused = false;
  }

  if (autoFocus && state->firstRender) {
    Input::Keyboard::Focus();
    state->focused = true;
  }

  if (state->firstRender && defaultValue != "") {
    state->value = defaultValue;
  }

  // DRAW BACKGROUND ---------------------------------------------------
  GUI::pushBorderRadius(borderRadius);
  GUI::Rect(dRect, background); // Background field

  if (outlineThickness != 0) {
    GUI::pushBorderRadius(borderRadius);
    GUI::Rect(dRect, outlineColor, outlineThickness); // 1px black Outline
  }

  // TEXT RECT CALCULATION ---------------------------------------------
  SDL_Rect textRect; // Calculate the position and size of the text

  // TEXT RECT HEIGHT --------------------------------------------------
  // If there is no fontSize speicified make the font max height
  if (fontSize == -1) {
    textRect.h = dRect.h - paddingRect.top - paddingRect.bottom;
  } else {
    textRect.h = fontSize;
  }

  // TEXT RECT WIDTH ---------------------------------------------------
  textRect.w = textRect.h * (float)state->td.getWidth() / state->td.getHeight();

  // TEXT RECT X POS ---------------------------------------------------
  // Depending on the placement, left, center, right. Default: left
  if (textAlignX == Align::LEFT) {
    textRect.x = dRect.x + paddingRect.left;
  } else if (textAlignX == Align::RIGHT) {
    textRect.x = dRect.x + dRect.w - paddingRect.right - textRect.w;
  } else if (textAlignX == Align::CENTER) {
    textRect.x = dRect.x + dRect.w / 2 - textRect.w / 2;
  }

  // TEXT RECT Y POS ---------------------------------------------------
  if (textAlignY == Align::BOTTOM) {
    textRect.y = dRect.y + dRect.h - paddingRect.bottom - textRect.h;
  } else if (textAlignY == Align::CENTER) {
    textRect.y = dRect.y + dRect.h / 2 - textRect.h / 2;
  } else if (textAlignY == Align::TOP) {
    textRect.y = dRect.y + paddingRect.top;
  }

  // Render the text
  GUI::Image(state->td, textRect);

  if (inputLock)
    GUI::Rect(dRect, {120, 120, 120, 120});

  // Check if placeholder was rendered
  bool placeholderActive = false;
  if (state->value == "")
    placeholderActive = true;

  // If the rendered text is larger then the allowed width
  // and we are also not talking about the placeholder THEN
  // make the removed variable larger by one, signaling to the
  // next frame that it should cut out one more frame from the left
  if (textRect.w > (dRect.w - paddingRect.left - paddingRect.right) &&
      !placeholderActive) {
    state->removed++;
    state->change = true;
  }

  // If the keyboard is focused on the field draw the caret, text cursor
  if (state->focused) {
    // The cursor would be drawn for half a second and then be not drawn for
    // another half a second, this is acheved trough clock which changes value
    // every (FPS / 2)frames so it can be devieded using %2 operator
    int blinkClock = static_cast<int>(Sys::getCurrentFrame() / (Sys::FPS / 2));
    // Sys::FPS is 60? It is defined in SM.h.

    if (blinkClock % 2) {
      // Top point of the line, default the
      // x is equal to the starting position of the text
      SDL_Point p1 = {0, dRect.y + paddingRect.top};

      if (placeholderActive) {
        if (textAlignX == Align::LEFT) {
          p1.x = dRect.x + paddingRect.left;
        } else if (textAlignX == Align::CENTER) {
          p1.x = dRect.x + dRect.w / 2;
        } else if (textAlignX == Align::RIGHT) {
          p1.x = dRect.x + dRect.w - paddingRect.right;
        }
      } else {
        p1.x = textRect.x + textRect.w + 1;
      }

      // Bottom point of the line
      SDL_Point p2 = {p1.x,
                      p1.y + (dRect.h - paddingRect.top - paddingRect.bottom)};

      // Draw the line
      GUI::Line(p1, p2, foreground);
    }
  }

  // EVENTS ---------------------------------

  // Use Input::Mouse::GetAbsolutePos and manual check?
  // Or isHovering if I added it to Mouse or somewhere.
  // I did not add isHovering to Input::Mouse.
  // I'll use the helper `Lumos::isRectVisible`? No, `isPointInRect` is in
  // `lib.h`? Wait, where did I see isPointInRect? In previous `Button.cpp` I
  // used `SDL_Point mPos = Input::Mouse::GetAbsolutePos(); bool hovering =
  // isPointInRect(mPos, dRect);`. I will do the same here.

  SDL_Point mPos = Input::Mouse::GetAbsolutePos();
  bool hovering = isPointInRect(mPos, dRect);

  if (hovering && Input::Mouse::IsReleased(Input::Mouse::LEFT)) {
    Input::Keyboard::Focus();
    Input::Keyboard::StartTextInput(); // Start SDL Text Input
    state->focused = true;
  }

  // If user is not hovering the button, cursor is outside
  // and there is click, that means that there has been unfocus
  if (!hovering && Input::Mouse::IsReleased(Input::Mouse::LEFT) &&
      !state->firstRender) {
    Input::Keyboard::Unfocus();
    Input::Keyboard::StopTextInput();
    state->focused = false;
  }

  // If Sys::Keyboard::unfocus() was runned but the input still wants the focus
  // Note: New Keyboard doesn't have a concept of "who has focus", just "is
  // focused". Use state->focused to track if THIS input thinks it has focus.
  if (state->focused && !Input::Keyboard::IsFocused()) {
    // Re-acquire focus? Or lose focus?
    // If global focus is lost, we should probably lose focus too?
    // Or re-assert it?
    // Original: `if(state->focused && !Keyboard::isFocused())
    // Keyboard::focus();` This suggests aggressive re-focusing?
    Input::Keyboard::Focus();
    // And StartTextInput?
    Input::Keyboard::StartTextInput();
  }

  // A small lambda funtion for handling deletion
  auto deleteLastChar = [&]() {
    // If value is already empty skip
    if (state->value.empty())
      return;

    // Pop last char and mark change has been made
    state->value.pop_back();
    state->change = true;

    // If there was any chars to be removed, decrease the amount
    if (state->removed != 0)
      state->removed--;
  };

  // If the input is focused handle key presses
  if (state->focused) {
    // Add the text inputed into the value
    // Keyboard::getText() returns buffer of text input events
    string txt = Input::Keyboard::GetText();
    if (!txt.empty()) {
      state->change = true;
      state->value += txt;
    }

    // If the backspace has been pressed handle deletion
    if (Input::Keyboard::IsHeld(SDL_SCANCODE_BACKSPACE)) {
      // Original used IsDown (which probably means Held in old lib or Pressed?)
      // lib_old/GUI/Input.cpp used `Keyboard::isDown(SDL_SCANCODE_BACKSPACE)`
      // and differentiated simple press vs hold with `state->deleting`.

      // If the input is not in the state of deletion
      // that means validity check logic.
      // Let's replicate logic.
      if (!state->deleting) {
        deleteLastChar();
        state->deleting = true;
      } else {
        // If the deletion has been going, and the
        // backspace is still down, holding, every
        // 200ms delete one last char from the field
        if (Sys::getCurrentFrame() % (Sys::FPS / 5) == 0) { // Modulo 0?
          // Original: `if(Sys::getCurrentFrame() % (Sys::FPS / 5)){`
          // Wait, if result is non-zero (true), then it deletes?
          // That means it deletes on 4 out of 5 frames? That's very fast.
          // Or did it mean `== 0`?
          // "every 200ms delete one last char" -> 5 times a sec.
          // FPS=60. FPS/5 = 12.
          // if frame % 12 ...
          // if it returns non-zero, it deletes?
          // Standard logic is `== 0` for periodic.
          // Maybe the original code had a bug or I misread it.
          // "if(Sys::getCurrentFrame() % (Sys::FPS / 5))" evaluates to true
          // most of the time. So it deletes almost every frame except when
          // modulo is 0? That would be super fast. I'll assume it meant `== 0`.
          deleteLastChar();
        }
      }
    } else {
      // If the backspace is not pressed end the deletion process
      state->deleting = false;
    }
  }

  // Now handle the value changes and updating the texture
  if (state->change) {
    state->td.setTexture(nullptr);

    string textToCompile;
    SDL_Color colorOfText = foreground;
    if (state->value == "") {
      colorOfText.a *= 0.75;
      textToCompile = placeholder;
    } else {
      // So bc text can get longer then it can fit in the field
      // program also keeps track of how many characters it needs
      // to hide from the left side in order for text from the right
      // side to be vissible and with in the bounds
      textToCompile = state->value;
      if (state->removed < static_cast<int>(textToCompile.size()))
        textToCompile.erase(0, state->removed);
      else
        textToCompile = "";
    }

    int err = TM::createTextTexture(state->td, textToCompile, textRect.h, 400,
                                    colorOfText);
    CHECK_ERROR(err);

    state->change = false;
  }

  if (state->firstRender)
    state->firstRender = false;

  // Return the value
  return state->value;
}

void GUI::DestroyInput(const string &uniqueId) {
  InputState *state = nullptr;
  auto it = inputStates.find(uniqueId);
  if (it != inputStates.end()) {
    state = &it->second;
  } else {
    return;
  }

  state->td.setTexture(nullptr);
  Input::Keyboard::Unfocus();
  state->focused = false;
  state->value.clear();
  inputStates.erase(it);
  return;
}
} // namespace Lumos
