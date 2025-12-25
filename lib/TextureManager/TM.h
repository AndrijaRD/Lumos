#pragma once
#ifndef Lumos_TextureManager
#define Lumos_TextureManager

#include "../SystemManager/SM.h"
#include "../lib.h"

// OpenCV and Dlib includes (ensure they are available in the env or cmake)
#include "opencv2/highgui.hpp"
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

extern "C" {
#include "../external/nanosvg.h"
#include "../external/nanosvgrast.h"
}

#include <functional>

namespace Lumos {

using PixelMapper =
    std::function<SDL_Color(uint8_t, uint8_t, uint8_t, uint8_t)>;

class TM; // Forward declaration

class TextureData {
  friend class TM;

public:
  int orgWidth = 0;
  int orgHeight = 0;
  std::string id = {};
  std::string path = {};

  TextureData();
  TextureData(const TextureData &) = default;
  TextureData &operator=(const TextureData &) = default;
  ~TextureData();

  SDL_Texture *getTexture() const { return dptr_->texture; }
  SDL_PixelFormat getFormat() const { return dptr_->format; }
  SDL_TextureAccess getAccess() const { return dptr_->access; }
  int getWidth() const { return dptr_->width; }
  int getHeight() const { return dptr_->height; }

  void setTexture(SDL_Texture *newTex);
  void reloadInfo();
  void printf(bool full = false) const;

  static inline SDL_PixelFormat defaultPixelFormat = SDL_PIXELFORMAT_RGBA32;
  static inline SDL_TextureAccess defaultAccess = SDL_TEXTUREACCESS_TARGET;

private:
  struct Impl {
    SDL_Texture *texture = nullptr;
    SDL_PixelFormat format = defaultPixelFormat;
    SDL_TextureAccess access = defaultAccess;
    int width = 0;
    int height = 0;
  };

  std::shared_ptr<Impl> dptr_;
};

class SVGIcon {
  friend class TM;

private:
  string path;
  NSVGimage *image;
  unordered_map<int, SDL_Texture *> sizes;
  SDL_Texture *loadSize(int size);

public:
  ~SVGIcon();
  SDL_Texture *getIcon(int size);
};

class TM {
  friend class TextureData;
  friend class SVGIcon;
  friend class Sys;

public:
  static inline NSVGrasterizer *rast;
  static inline std::vector<std::weak_ptr<TextureData::Impl>> loadedTextures;
  static inline std::vector<SDL_Texture *> outOfScopeTextures;
  static inline bool AUTO_DELETE_TEXTURES = true;

  static void registerTexture(shared_ptr<TextureData::Impl> const &ptr);
  static void removeTexture(shared_ptr<TextureData::Impl> dead);

public:
  static int loadTexture(TextureData &td, const string &path,
                         const string &id = "");
  static int loadSVG(SVGIcon &td, const string &path);
  static int createTextTexture(TextureData &td, const string &text,
                               int fontSize, int weight, SDL_Color color);

  static int copyTexture(const TextureData &src, TextureData &dst);
  static int resizeTexture(const TextureData &src, TextureData &dst,
                           int &newWidth, int &newHeight);
  static int resizeTexture(const TextureData &src, TextureData &dst,
                           const int &newWidth, const int &newHeight);

  static int exportTexture(const string &path, const TextureData &td);
  static int exportTexture(const string &path, SDL_Texture *tex);
  static int exportSurface(const string &path, SDL_Surface *surface);

  static int rotateTexture(const TextureData &src, TextureData &dst,
                           int angle = 90);
  static int cropTexture(const TextureData &src, TextureData &dst,
                         SDL_Rect rect);
  static int transformTexture(const TextureData &src, TextureData &dst,
                              PixelMapper pixelFunc);

  // Conversion functions
  static int convert_toTexture(const cv::Mat &cvMat, TextureData &td);
  static int convert_textureTo(const TextureData &td, cv::Mat &cvMat);
  static int convert_toTexture(const SDL_Surface *surface, TextureData &td);
  static int convert_toTexture(const SDL_Surface *surface, SDL_Texture *&tex);
  static int convert_textureTo(const TextureData &td, SDL_Surface *&surface);
  static int convert_textureTo(SDL_Texture *tex, SDL_Surface *&surface);

  static void setAutoDeleteTextures(bool prop = true);
};
} // namespace Lumos

#endif