#pragma once
#ifndef MySDL_TM
#define MySDL_TM

#include "../lib.h"


// GENERAL STRUCT FOR IMAGES -----------------------------------------------------------------------
struct TextureData{
    SDL_Texture* tex;
    Uint32 format;
    int width;
    int height;
    int orgWidth;
    int orgHeight;

    TextureData(
        SDL_Texture* t = nullptr,
        Uint32 f = SDL_PIXELFORMAT_RGBA32,
        int w = 0,
        int h = 0
    ): tex(t), format(f), width(w), height(h) {};

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
        os << td.tex;
        os << ", Format: " << td.format;
        os << ", w: " << td.width;
        os << "px, h: " << td.height;
    os << "px)";
    return os;
}



class TM{
    private:
    static inline vector<SDL_Texture*> loadedTextures;

    public:
    static int loadTexture(TextureData& td, const string& path);

    static void freeTexture(TextureData& td);
    // static void freeTexture(SDL_Texture* tex); // Dangerous, dangling pointer left

    static void cleanup();
    static int getLoadedTextures();

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
};

#endif
// Creator: @AndrijaRD