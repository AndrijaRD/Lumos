#define ALLOW_DIRECT_SDL

#include "./TM.h"
#include "../System/Sys.h"


SDL_Texture* TM::_CreateTexture(
    SDL_Renderer*   renderer, 
    Uint32          format, 
    int             access, 
    int             w, 
    int             h
){
    if(!Sys::isMainThread()){
        Sys::printf_warn(
            "TM::_CreateTexture called from non-main thread; "
            "This may lead to undefined behavior. Empty, black texture."
        );
    }

    SDL_ClearError();
    SDL_Texture* tex = SDL_CreateTexture(renderer, format, access, w, h);

    if(tex == nullptr){
        Sys::printf_err(
            std::string("SDL_CreateTexture failed: ") + SDL_GetError()
        );
    }

    return tex;
}


SDL_Texture* TM::_CreateTextureFromSurface(
    SDL_Renderer* renderer,
    SDL_Surface*  surface
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_CreateTextureFromSurface called from non-main thread; "
            "This may lead to undefined behavior. Empty black texture."
        );
    }

    SDL_ClearError();
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (tex == nullptr) {
        Sys::printf_err(
            std::string("SDL_CreateTextureFromSurface failed: ")
          + SDL_GetError()
        );
    }
    return tex;
}


int TM::_QueryTexture(
    SDL_Texture* texture,
    Uint32*      format,
    int*         access,
    int*         w,
    int*         h
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_QueryTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_QueryTexture(texture, format, access, w, h);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_QueryTexture failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_UpdateTexture(
    SDL_Texture*    texture,
    const SDL_Rect* rect,
    const void*     pixels,
    int             pitch
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_UpdateTexture called from non-main thread; "
            "This may lead to undefined behavior. Empty black Texture."
        );
    }

    SDL_ClearError();
    int rc = SDL_UpdateTexture(texture, rect, pixels, pitch);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_UpdateTexture failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}

int TM::_UpdateYUVTexture(
    SDL_Texture*    texture,
    const SDL_Rect* rect,
    const Uint8*    Yplane,
    int             Ypitch,
    const Uint8*    Uplane,
    int             Upitch,
    const Uint8*    Vplane,
    int             Vpitch
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_UpdateYUVTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_UpdateYUVTexture(
        texture, rect,
        Yplane, Ypitch,
        Uplane, Upitch,
        Vplane, Vpitch
    );
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_UpdateYUVTexture failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}

int TM::_LockTexture(
    SDL_Texture*    texture,
    const SDL_Rect* rect,
    void**          pixels,
    int*            pitch
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_LockTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_LockTexture(texture, rect, pixels, pitch);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_LockTexture failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


void TM::_UnlockTexture(SDL_Texture* texture) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_UnlockTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    SDL_UnlockTexture(texture);
}


int TM::_SetTextureBlendMode(
    SDL_Texture*  texture,
    SDL_BlendMode blendMode
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_SetTextureBlendMode called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_SetTextureBlendMode(texture, blendMode);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_SetTextureBlendMode failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_GetTextureBlendMode(
    SDL_Texture*   texture,
    SDL_BlendMode* blendMode
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_GetTextureBlendMode called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_GetTextureBlendMode(texture, blendMode);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetTextureBlendMode failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_SetTextureScaleMode(
    SDL_Texture*  texture,
    SDL_ScaleMode scaleMode
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_SetTextureScaleMode called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_SetTextureScaleMode(texture, scaleMode);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_SetTextureScaleMode failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}

int TM::_GetTextureScaleMode(
    SDL_Texture*   texture,
    SDL_ScaleMode* scaleMode
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_GetTextureScaleMode called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_GetTextureScaleMode(texture, scaleMode);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetTextureScaleMode failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_SetTextureAlphaMod(
    SDL_Texture* texture,
    Uint8        alpha
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_SetTextureAlphaMod called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_SetTextureAlphaMod(texture, alpha);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_SetTextureAlphaMod failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_GetTextureAlphaMod(
    SDL_Texture* texture,
    Uint8*       alpha
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_GetTextureAlphaMod called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_GetTextureAlphaMod(texture, alpha);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetTextureAlphaMod failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_SetTextureColorMod(
    SDL_Texture* texture,
    Uint8        r,
    Uint8        g,
    Uint8        b
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_SetTextureColorMod called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_SetTextureColorMod(texture, r, g, b);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_SetTextureColorMod failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


int TM::_GetTextureColorMod(
    SDL_Texture* texture,
    Uint8*       r,
    Uint8*       g,
    Uint8*       b
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_GetTextureColorMod called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }

    SDL_ClearError();
    int rc = SDL_GetTextureColorMod(texture, r, g, b);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetTextureColorMod failed: ")
          + SDL_GetError()
        );
    }
    return rc;
}


void TM::_DestroyTexture(SDL_Texture* texture) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_DestroyTexture(texture);
}


int TM::_RenderClear(SDL_Renderer* renderer) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderClear(renderer);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderClear failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderDrawPoint(SDL_Renderer* renderer, int x, int y) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawPoint(renderer, x, y);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawPoint failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderDrawPoints(
    SDL_Renderer*    renderer,
    const SDL_Point* points,
    int              count
) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawPoints(renderer, points, count);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawPoints failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderDrawLine(SDL_Renderer* renderer,
                        int           x1,
                        int           y1,
                        int           x2,
                        int           y2)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawLine failed: ") + SDL_GetError()
        );
    }
    return rc;
}

int TM::_RenderDrawLine(
    SDL_Renderer    *renderer, 
    const SDL_Point p1, 
    const SDL_Point p2
){
    return TM::_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

int TM::_RenderDrawLines(SDL_Renderer* renderer,
                         const SDL_Point* points,
                         int               count)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawLines(renderer, points, count);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawLines failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderDrawRect(SDL_Renderer* renderer, 
                        const SDL_Rect* rect) 
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawRect(renderer, rect);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawRect failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderDrawRects(SDL_Renderer* renderer,
                         const SDL_Rect* rects,
                         int              count)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderDrawRects(renderer, rects, count);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderDrawRects failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderFillRect(SDL_Renderer* renderer, 
                        const SDL_Rect* rect) 
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderFillRect(renderer, rect);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderFillRect failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderFillRects(SDL_Renderer* renderer,
                         const SDL_Rect* rects,
                         int              count)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderFillRects(renderer, rects, count);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderFillRects failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderCopy(SDL_Renderer*    renderer,
                    SDL_Texture*    texture,
                    const SDL_Rect* srcrect,
                    const SDL_Rect* dstrect)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderCopy(renderer, texture, srcrect, dstrect);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderCopy failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderCopyEx(SDL_Renderer*    renderer,
                      SDL_Texture*    texture,
                      const SDL_Rect* srcrect,
                      const SDL_Rect* dstrect,
                      double           angle,
                      const SDL_Point* center,
                      SDL_RendererFlip flip)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderCopyEx(
        renderer, texture, srcrect, dstrect, angle, center, flip
    );
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderCopyEx failed: ") + SDL_GetError()
        );
    }
    return rc;
}


int TM::_RenderReadPixels(SDL_Renderer*    renderer,
                          const SDL_Rect*  rect,
                          Uint32           format,
                          void*            pixels,
                          int              pitch)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderReadPixels(renderer, rect, format, pixels, pitch);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderReadPixels failed: ") + SDL_GetError()
        );
    }
    return rc;
}

int TM::_RenderGeometry(SDL_Renderer*       renderer,
                        SDL_Texture*       texture,
                        const SDL_Vertex*  vertices,
                        int                num_vertices,
                        const int*         indices,
                        int                num_indices)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderGeometry(
        renderer, texture, vertices, num_vertices, indices, num_indices
    );
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_RenderGeometry failed: ") + SDL_GetError()
        );
    }
    return rc;
}


void TM::_RenderPresent(SDL_Renderer* renderer) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    // SDL_RenderPresent returns void; we can still check SDL_GetError afterward
    SDL_ClearError();
    SDL_RenderPresent(renderer);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(
            std::string("SDL_RenderPresent error: ") + err
        );
    }
}


SDL_Renderer* TM::_CreateRenderer(SDL_Window* window,
                                  int         index,
                                  Uint32      flags)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    SDL_Renderer* ren = SDL_CreateRenderer(window, index, flags);
    if (!ren) {
        Sys::printf_err(
            std::string("SDL_CreateRenderer failed: ") + SDL_GetError()
        );
    }
    return ren;
}


SDL_Renderer* TM::_CreateSoftwareRenderer(SDL_Surface* surface) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    SDL_Renderer* ren = SDL_CreateSoftwareRenderer(surface);
    if (!ren) {
        Sys::printf_err(
            std::string("SDL_CreateSoftwareRenderer failed: ") + SDL_GetError()
        );
    }
    return ren;
}


int TM::_GetRendererInfo(SDL_Renderer*        renderer,
                         SDL_RendererInfo* info)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_GetRendererInfo(renderer, info);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetRendererInfo failed: ") + SDL_GetError()
        );
    }
    return rc;
}


SDL_Renderer* TM::_GetRenderer(SDL_Window* window) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    // SDL_GetRenderer doesn’t exist in older SDL2; if yours does:
    SDL_ClearError();
    SDL_Renderer* ren = SDL_GetRenderer(window);
    if (!ren) {
        Sys::printf_err(
            std::string("SDL_GetRenderer failed: ") + SDL_GetError()
        );
    }
    return ren;
}

int TM::_GetRendererOutputSize(SDL_Renderer* renderer,
                               int*          w,
                               int*          h)
{
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_GetRendererOutputSize(renderer, w, h);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_GetRendererOutputSize failed: ") + SDL_GetError()
        );
    }
    return rc;
}

void TM::_DestroyRenderer(SDL_Renderer* renderer) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_DestroyRenderer(renderer);
}


int TM::_SetRenderTarget(SDL_Renderer* renderer, SDL_Texture* texture) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_SetRenderTarget(renderer, texture);
    if (rc != 0) {
        Sys::printf_err(
            std::string("SDL_SetRenderTarget failed: ") + SDL_GetError()
        );
    }
    return rc;
}


SDL_Texture* TM::_GetRenderTarget(SDL_Renderer* renderer) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    SDL_Texture* tex = SDL_GetRenderTarget(renderer);
    if (!tex && *SDL_GetError()) {
        Sys::printf_err(
            std::string("SDL_GetRenderTarget failed: ") + SDL_GetError()
        );
    }
    return tex;
}


int TM::_RenderSetViewport(SDL_Renderer* renderer, const SDL_Rect* rect) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderSetViewport(renderer, rect);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_RenderSetViewport failed: ") + SDL_GetError());
    }
    return rc;
}


void TM::_RenderGetViewport(SDL_Renderer* renderer, SDL_Rect* rect) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_RenderGetViewport(renderer, rect);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(std::string("SDL_RenderGetViewport error: ") + err);
    }
}


int TM::_RenderSetClipRect(SDL_Renderer* renderer, const SDL_Rect* rect) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderSetClipRect(renderer, rect);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_RenderSetClipRect failed: ") + SDL_GetError());
    }
    return rc;
}


void TM::_RenderGetClipRect(SDL_Renderer* renderer, SDL_Rect* rect) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_RenderGetClipRect(renderer, rect);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(std::string("SDL_RenderGetClipRect error: ") + err);
    }
}


SDL_bool TM::_RenderIsClipEnabled(SDL_Renderer* renderer) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    SDL_bool result = SDL_RenderIsClipEnabled(renderer);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(std::string("SDL_RenderIsClipEnabled error: ") + err);
    }
    return result;
}


int TM::_RenderSetScale(SDL_Renderer* renderer, float scaleX, float scaleY) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderSetScale(renderer, scaleX, scaleY);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_RenderSetScale failed: ") + SDL_GetError());
    }
    return rc;
}


void TM::_RenderGetScale(SDL_Renderer* renderer, float* scaleX, float* scaleY) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_RenderGetScale(renderer, scaleX, scaleY);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(std::string("SDL_RenderGetScale error: ") + err);
    }
}


int TM::_RenderSetLogicalSize(SDL_Renderer* renderer, int w, int h) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_RenderSetLogicalSize(renderer, w, h);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_RenderSetLogicalSize failed: ") + SDL_GetError());
    }
    return rc;
}


void TM::_RenderGetLogicalSize(SDL_Renderer* renderer, int* w, int* h) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_RenderGetLogicalSize(renderer, w, h);
    const char* err = SDL_GetError();
    if (err && *err) {
        Sys::printf_err(std::string("SDL_RenderGetLogicalSize error: ") + err);
    }
}


int TM::_SetRenderDrawColor(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_SetRenderDrawColor failed: ") + SDL_GetError());
    }
    return rc;
}


int TM::_SetRenderDrawColor(SDL_Renderer* renderer, SDL_Color color) {
    return TM::_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}


int TM::_GetRenderDrawColor(SDL_Renderer* renderer, Uint8* r, Uint8* g, Uint8* b, Uint8* a) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_GetRenderDrawColor(renderer, r, g, b, a);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_GetRenderDrawColor failed: ") + SDL_GetError());
    }
    return rc;
}


int TM::_SetRenderDrawBlendMode(SDL_Renderer* renderer, SDL_BlendMode blendMode) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_SetRenderDrawBlendMode(renderer, blendMode);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_SetRenderDrawBlendMode failed: ") + SDL_GetError());
    }
    return rc;
}


int TM::_GetRenderDrawBlendMode(SDL_Renderer* renderer, SDL_BlendMode* blendMode) {
    if (!Sys::isMainThread()) {
        Sys::printf_warn(
            "TM::_DestroyTexture called from non-main thread; "
            "This may lead to undefined behavior."
        );
    }
    SDL_ClearError();
    int rc = SDL_GetRenderDrawBlendMode(renderer, blendMode);
    if (rc != 0) {
        Sys::printf_err(std::string("SDL_GetRenderDrawBlendMode failed: ") + SDL_GetError());
    }
    return rc;
}




