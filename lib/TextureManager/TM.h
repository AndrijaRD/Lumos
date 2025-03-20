#pragma once
#ifndef MySDL_TM
#define MySDL_TM

#include "../lib.h"

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include "opencv2/highgui.hpp"


// GENERAL STRUCT FOR IMAGES -----------------------------------------------------------------------
struct TextureData{
    SDL_Texture* tex;
    Uint32 format;
    int width;
    int height;
    int orgWidth;
    int orgHeight;
    string path;
    string id;

    TextureData(
        SDL_Texture* t = nullptr,
        Uint32 f = SDL_PIXELFORMAT_RGBA32,
        int w = 0,
        int h = 0,
        string p = "",
        string _id = "empty"
    ): tex(t), format(f), width(w), height(h), path(p), id(_id) {};

    int drawOverlayTexture(
        const TextureData& td,
        SDL_Rect& dr
    );

    int drawOverlayTexture(
        const TextureData& td
    );

    int drawOverlayRect(
        const SDL_Rect& rect,
        const SDL_Color& color,
        const int& thickness = -1
    );

    int drawOverlayLine(
        const SDL_Point& p1,
        const SDL_Point& p2,
        const SDL_Color& color,
        const int& thickness
    );

    int drawOverlayText(
        const string& text,
        SDL_Rect& dRect,
        const SDL_Color& color
    );
};


// CREATE << OPERATOR HANDLE FUNC ------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os, const TextureData& td){
    os << "TextureData(";
        os << "ID: " << td.id;
        os << ", Tex: " << td.tex;
        os << ", Format: " << td.format;
        os << ", W: " << td.width << "px";
        os << ", H: " << td.height << "px";
    os << ")";
    return os;
}



class TM{
    private:
    static inline unordered_map<SDL_Texture*, string> loadedTextures;

    public:
    static int loadTexture(
        TextureData& td, 
        const string& path, 
        const string& id = ""
    );

    static void freeTexture(TextureData& td);
    // static void freeTexture(SDL_Texture* tex); // Dangerous, dangling pointer left

    static void cleanup();

    static int countLoadedTextures();
    static void printLoadedTextures();
    static unordered_map<SDL_Texture*, string> getLoadedTextures();

    static int renderTexture(const TextureData& td, SDL_Rect& dr);

    static int copy(const TextureData& src, TextureData& dst);

    static int createTextTexture(
        TextureData& td, 
        const string& text,
        const int& fontSize,
        const SDL_Color& color
    );

    static int resize(
        TextureData& td,
        int width,
        int height = -1
    );

    static int cvMat2Texture(const cv::Mat& mat, TextureData& td);

    static void exportTexture(const TextureData& td, string path);
    static void exportTexture(SDL_Texture* texture, string path);

    static bool saveSurfaceToFile(SDL_Surface* surface, const string& filename);
};

#endif
// Creator: @AndrijaRD