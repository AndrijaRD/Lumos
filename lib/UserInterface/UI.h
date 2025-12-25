#pragma once
#ifndef Lumos_UI
#define Lumos_UI

#include "../TextureManager/TM.h"
#include "../lib.h"

namespace Lumos {

enum class Align { LEFT, RIGHT, BOTTOM, TOP, CENTER, DEFAULT };

enum class CursorState { OUTSIDE, CLICKED, HOVERING, DRAGGING };

struct PaddingRect {
  int top;
  int right;
  int bottom;
  int left;

  PaddingRect() : top(0), right(0), bottom(0), left(0) {}

  PaddingRect(int padding)
      : top(padding), right(padding), bottom(padding), left(padding) {}

  PaddingRect(int _top, int _right, int _bottom, int _left)
      : top(_top), right(_right), bottom(_bottom), left(_left) {}
};

struct BorderRadiusRect {
  uint top_left;
  uint top_right;
  uint bottom_left;
  uint bottom_right;

  BorderRadiusRect()
      : top_left(0), top_right(0), bottom_left(0), bottom_right(0) {};

  BorderRadiusRect(uint radius)
      : top_left(radius), top_right(radius), bottom_left(radius),
        bottom_right(radius) {};

  BorderRadiusRect(uint tl, // top-left
                   uint tr, // top-right
                   uint bl, // bottom-left
                   uint br  // bottom-right
                   )
      : top_left(tl), top_right(tr), bottom_left(bl), bottom_right(br) {};
};

/**
 * @brief Helper function, used in Rect and Line GUI elements.
 */
void drawThickLineSegment(const SDL_Point &p1, const SDL_Point &p2,
                          const SDL_Color &color, int thickness);

class GUI {
  friend class Sys;

  friend bool isAbsolutePointInContainerRect(SDL_Point point, SDL_Rect rect);
  friend void drawThickLineSegment(const SDL_Point &p1, const SDL_Point &p2,
                                   const SDL_Color &color, int thickness);

private:
  struct LoadedText {
    TextureData td;   // Holds the Compiled Text Texture
    string title;     // The text
    int fontSize = 0; // px
    int weight = 400; // numeric weight (100..900)
    SDL_Color color = {255, 255, 255, 255};
    int lastUsedFrame = 0;

    LoadedText(TextureData _td = TextureData(), string _title = "",
               int _fontSize = 0, int _weight = 400,
               SDL_Color _color = SDL_COLOR_WHITE, int _lastUsedFrame = 0)
        : td(_td), title(_title), fontSize(_fontSize), weight(_weight),
          color(_color), lastUsedFrame(_lastUsedFrame) {};

    string getId() { return getTextId(title, fontSize, weight, color); }

    bool operator<(const LoadedText &other) const {
      return lastUsedFrame < other.lastUsedFrame;
    }
    bool operator>(const LoadedText &other) const {
      return lastUsedFrame > other.lastUsedFrame;
    }
  };

  static inline unordered_map<string, LoadedText> loadedTexts;
  static inline int MAX_LOADED_TEXTS = 0; // 0 means unlimited

  static string getTextId(const string &title, int fontSize, int weight,
                          SDL_Color color);
  static LoadedText *loadNewText(string title, int fontSize, int weight,
                                 SDL_Color color);
  static void removeOldestText();

  struct InputState {
    string id;      // Holds the uniqueId
    string value;   // Holds the current value of the Input Field
    TextureData td; // Holds the texture of the Text

    bool focused;
    bool firstRender = true;
    bool change = true;
    bool deleting = false;
    int removed = 0;

    InputState(const string &_id, const string &_value = "",
               const bool &_focused = false)
        : id(_id), value(_value), focused(_focused) {};

    friend ostream &operator<<(ostream &os, const InputState &state);
  };

  static inline unordered_map<string, InputState> inputStates;

  struct ContainerState {
    string id;
    SDL_Rect dRect;
    int contentHeight = 0;

    int scrollOffset = 0;
    int scrollSpeed = 20;

    // For scrollbar dragging:
    bool scrollbarDragging = false;
    int scrollbarDragStartMouseY = 0;
    int scrollbarDragStartScrollOffset = 0;

    int lastActiveFrame = 0;

    // Default Constructor
    ContainerState(string _id) { id = _id; }
  };

  static inline unordered_map<string, ContainerState> containerStates;
  static inline string activeContainer;

  static void renderVerticalScrollbar(
      const SDL_Rect &container, // x,y,w,h of the visible area
      int contentHeight,         // total height of scrollable content
      int &scrollOffset          // current scroll offset—in/out
  );

  static void renderRect(const SDL_Rect &dRect, const SDL_Color &color,
                         const int thickness,
                         const BorderRadiusRect &borderRadius);

  // Pushable States ----------------------------------------
  static inline int pFontSize = -1;

  static inline Align pTextAlignY = Align::DEFAULT;
  static inline Align pTextAlignX = Align::DEFAULT;

  static inline bool pAutoFocus = false;
  static inline bool pInputLock = false;
  static inline string pDefaultValue = "";
  static inline int pOutlineThickness = -1;
  static inline SDL_Color pOutlineColor = {0, 0, 0, 0};

  static inline BorderRadiusRect pBorderRadius = {0, 0, 0, 0};
  static inline PaddingRect pPaddingRect = {-1, -1, -1, -1};

  static inline bool pDashLine = false;
  static inline int pDashSize = 0;
  static inline int pDashGapSize = 0;

public:
  static void Text(const string &text, SDL_Rect &dRect, const SDL_Color &color,
                   int weight = 400);

  static void TextDynamic(const string &title, SDL_Rect &dRect,
                          const SDL_Color &color);

  static int calcTextWidth(const string &text, int textHeight);
  static int calcTextHeight(const string &text, int textWidth);
  static void setMaxNumOfLoadedTextures(int number = 0);

  static CursorState Button(const string &title, const SDL_Rect &dRect,
                            const SDL_Color &buttonColor = THEME_COLOR_2,
                            const SDL_Color &textColor = SDL_COLOR_WHITE);

  static void Image(SDL_Texture *texture, SDL_Rect &rect);
  static void Image(TextureData &td, SDL_Rect &rect);
  static void Image(SVGIcon &icon, SDL_Rect &rect);

  static string Input(const std::string &uniqueId, const SDL_Rect &dRect,
                      const std::string &placeholder = "Type text here...",
                      const SDL_Color &background = SDL_COLOR_WHITE,
                      const SDL_Color &textColor = SDL_COLOR_BLACK);

  static void DestroyInput(const string &inputId);
  static InputState *getInputState(const string &inputId);

  static void beginContainer(const string &uniqueId, const SDL_Rect &dRect);
  static void endContainer();
  static ContainerState *getContainerState(const string &containerId);

  static bool isRectVisible(SDL_Rect rect);

  static void Rect(const SDL_Rect &dRect,
                   const SDL_Color &color = SDL_COLOR_WHITE,
                   const int thickness = -1);

  static void Line(const SDL_Point &p1, const SDL_Point &p2,
                   const SDL_Color &color, const uint &thickness = 1);

  static void Circle(const SDL_Point &center, const int &radius,
                     const SDL_Color &color = SDL_COLOR_WHITE,
                     const int &thickness = -1);

  // Pushing the styles
  // -------------------------------------------------------------------------

  static void pushFontSize(uint fontSize);

  static void pushTextAlignY(Align direction);
  static void pushTextAlignX(Align direction);

  static void pushAutoFocus();
  static void pushInputLock();

  static void pushBorderRadius(uint radius);
  static void pushBorderRadius(BorderRadiusRect radiusRect);
  static void pushBorderRadius(uint topLeft, uint topRight, uint bottomLeft,
                               uint bottomRight);

  static void pushDefaultValue(string value);
  static void setInputValue(string inputId,
                            string newValue); // Override the current value

  static void pushOutlineStyle(int thickness = -1,
                               SDL_Color color = {0, 0, 0, 0});

  static void pushPadding(int padding);
  static void pushPadding(PaddingRect paddingRect);

  static void pushDashLineStyle(int dashLineSize, int dashGapSize);
};

} // namespace Lumos

#endif