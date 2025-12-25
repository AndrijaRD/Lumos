#pragma once
#ifndef Lumos_FontManager
#define Lumos_FontManager

#include "../lib.h"

namespace Lumos {

class FM {
  // Default fallback font path
  static inline string defaultFontPath = "";

  // Cache structure: Path -> Size -> Font
  static inline unordered_map<string, unordered_map<int, TTF_Font *>> fontCache;

public:
  static int init(const string &fontPath =
                      "/home/data/DATA/ASSETS/Poppins/Poppins-Regular.ttf");

  // Get font by size. Optional: provide specific path to load a different font.
  static TTF_Font *get(int fontSize, const string &specificPath = "");

  static void cleanup();
};

} // namespace Lumos

#endif