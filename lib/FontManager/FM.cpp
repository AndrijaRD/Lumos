#include "FM.h"
#include "../SystemManager/SM.h" // For logging if needed, or use std::cout

namespace Lumos {

int FM::init(const string &_fontPath) {
  defaultFontPath = _fontPath;

  // Initialize SDL_ttf
  if (!TTF_WasInit()) {
    if (TTF_Init() < 0) {
      Sys::logError(SYS_FONT_INIT_ERROR);
      SDL_Log("TTF_Init failed: %s", SDL_GetError());
      return SYS_FONT_INIT_ERROR;
    }
  }

  cout << "[INIT] Fonts Initialized (SDL_ttf)..." << endl;
  return NO_ERROR;
}

TTF_Font *FM::get(int fontSize, const string &specificPath) {
  // Use default path if no specific one provided
  string path = specificPath.empty() ? defaultFontPath : specificPath;

  // Check cache: fontCache[path][fontSize]
  if (fontCache.count(path) && fontCache[path].count(fontSize)) {
    return fontCache[path][fontSize];
  }

  // Load new font
  TTF_Font *font = TTF_OpenFont(path.c_str(), fontSize);
  if (!font) {
    SDL_Log("[ERROR] Failed to load font: %s (%dpt)", path.c_str(), fontSize);
    SDL_Log("SDL_ttf Error: %s", SDL_GetError());
    return nullptr;
  }

  // Store in cache
  fontCache[path][fontSize] = font;
  return font;
}

void FM::cleanup() {
  for (auto &fileEntry : fontCache) {
    for (auto &sizeEntry : fileEntry.second) {
      if (sizeEntry.second) {
        TTF_CloseFont(sizeEntry.second);
      }
    }
  }
  fontCache.clear();

  if (TTF_WasInit()) {
    TTF_Quit();
  }
}
} // namespace Lumos
