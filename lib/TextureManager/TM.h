#pragma once
#ifndef MySDL_TM
#define MySDL_TM

#include "../lib.h"
#include "../System/Sys.h"

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include "opencv2/highgui.hpp"

// two‑stage stringify so that __LINE__ becomes "123" rather than literal __LINE__
#define STR2(x) #x
#define STR(x)  STR2(x)

class TextureData {
friend class TM;
public:
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // PUBLIC–MUTABLE
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    int          orgWidth  = 0;   ///< user‐settable “original” width
    int          orgHeight = 0;   ///< user‐settable “original” height
    std::string  id        = {};  ///< user‐settable identifier
    std::string  path      = {};  ///< user‐settable file‐path

    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // LIFETIME
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    TextureData();

    TextureData(const TextureData&) = default;
    TextureData& operator=(const TextureData&) = default;

    ~TextureData();

    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // READ‑ONLY ACCESSORS for the *live* SDL_Texture and its properties
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    SDL_Texture* getTexture() const { return dptr_->texture; }
    Uint32       getFormat()  const { return dptr_->format; }
    int          getAccess()  const { return dptr_->access; }
    int          getWidth()   const { return dptr_->width; }
    int          getHeight()  const { return dptr_->height; }

    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // SAFE MUTATORS
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

    // Replace the SDL_Texture*.  If no other TextureData still points at
    // the old texture, it will be destroyed (or stashed in outOfScopeTextures).
    void setTexture(SDL_Texture* newTex);

    // Manually re‑query width/height/format/access
    void reloadInfo();

    void printf(bool full = false) const;
    
private:
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // THE IMPL—this lives on the heap, shared by copies of the handle
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    struct Impl {
        SDL_Texture* texture = nullptr;
        Uint32       format  = 0;
        int          access  = 0;
        int          width   = 0;
        int          height  = 0;
    };

    std::shared_ptr<Impl> dptr_;

    // hack for __LINE__ stringify
    #define STR2(x) #x
    #define STR(x)  STR2(x)
};

class TM{
    friend class TextureData;
private:
    // A vector that holds all of the TextureData objects that have been
    // created, as a weak pointer. Used to keep track of loaded Textures
    static inline std::vector<std::weak_ptr<TextureData::Impl>> loadedTextures;

    // If SDL_Texture* went out of scope but AUTO_DELETE_TEXTURES is
    // set to false, the textures are stored here
    static inline std::vector<SDL_Texture*> outOfScopeTextures;


    // Private function called by TextureData::create()
    // It adds the newly created TextureDataPtr to the loadedTextures vector
    static void registerTexture(shared_ptr<TextureData::Impl> const& ptr);

    // Private function called by TextureData destructor
    // It removes the TextureData obj from loadedTextures vector
    static void removeTexture(shared_ptr<TextureData::Impl> dead);


    // A global variable that is used when deciding what do the with the textures
    // when they go out of scope, if a user tries to set a new texture to the existing 
    // TextureData object, what happends with the prevous, old, texture?
    // Firstly the function checks if there are any other objects that point to that
    // previous SDL_Texture*, if there are the texture is not freeed, if there are no
    // objects referencing the old texture and the AUTO_DELETE_TEXTURES is set to true
    // the texture gets automaticly freed.
    //
    // If you wish to change the value of this use TM::setAutoDeleteTextures(bool);
    static inline bool AUTO_DELETE_TEXTURES = true;

public:

// TextureData Managment ------------------------------------------------------------

    /**
     * Loading Textures from a Path.
     * 
     * @param td TextureData variable in which texture will be stored
     * @param path Path to the image
     * @param id Optional, if not set it will be equal to the file path
     */
    static int loadTexture(
        TextureData& td, 
        const string& path, 
        const string& id = ""
    );



    /**
     * @brief Creates a texture from a text using TTF_RenderUTF8_Blended.
     * It saves the texture into TextureDataPtr and fills the rest of the object data.
     * 
     * @param td Where should texture be saved
     * @param text Text
     * @param fontSize Font Size
     * @param color Text Color
     * @return error code
     */
    static int createTextTexture(
        TextureData& td,
        const string& text,
        int fontSize,
        SDL_Color color
    );


    /**
     * It makes a copy of a src texture and places it into dst.
     * 
     * @param src Source Texture that should be made copy of
     * @param dst Destination Texture
     */
    static int copyTexture(
        const TextureData& src, 
        TextureData& dst
    );


    /**
     * It resizes the Texture to the given width and height.
     * If one of the size params (width or height) are set to -1,
     * it will be caluclated automaticly to fit the aspect ratio
     * of the image.
     * 
     * @param td TextureData to be resized
     * @param width Desired Width
     * @param height Desired Height
     */
    static int resizeTexture(
        TextureData& td,
        int& newWidth,
        int& newHeight
    );


    /**
     * It exports a Texture into a png image, to the specified location.
     * 
     * @param path The files path, where texture should be saved to
     * @param td The Texture to be exported
     */
    static int exportTexture(
        const string& path,
        const TextureData& td
    );


    /**
     * It exports a SDL_Surface* into a image, png file.
     * 
     * @param path The file path, where surface should be saved to
     * @param surface SDL_Surface* to be exported
     */
    static int exportSurface(
        const string& path,
        SDL_Surface* surface
    );


    /* CONVERTING FUNCTIONS */

    /**
     * @brief Converts cv::Mat into TextureData, SDL_Texture.
     * RGBA format of Mat object required.
     * 
     * Use `cv::cvtColor(srcMat, dstMat, cv::COLOR_BGR2RGBA);`
     * 
     * @param cvMat RGBA format of cv::Mat object
     * @param td TextureData object where result will be saved
     * @return int error code
     */
    static int convert_toTexture(const cv::Mat& cvMat, TextureData& td);

    static int convert_textureTo(const TextureData& td, cv::Mat& cvMat);


    /**
     * Function used to set the AUTO_DELETE_TEXTURES propertie.
     */
    static void setAutoDeleteTextures(bool prop = true);
    
    









// Texture Managment ----------------------------------------------------------------

    // Texture Functions --------------------------------------------------------------------------

    /**
     * Create a texture for a rendering context.
     *
     * You can set the texture scaling method by setting
     * `SDL_HINT_RENDER_SCALE_QUALITY` before creating the texture.
     *
     * \param renderer the rendering context.
     * \param format one of the enumerated values in SDL_PixelFormatEnum.
     * \param access one of the enumerated values in SDL_TextureAccess.
     * \param w the width of the texture in pixels.
     * \param h the height of the texture in pixels.
     * \returns a pointer to the created texture or NULL if no rendering context
     *          was active, the format was unsupported, or the width or height
     *          were out of range; call SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateTextureFromSurface
     * \sa SDL_DestroyTexture
     * \sa SDL_QueryTexture
     * \sa SDL_UpdateTexture
     */
    static SDL_Texture* _CreateTexture(
        SDL_Renderer *renderer, 
        Uint32 format, 
        int access, 
        int w, 
        int h
    );

    /**
     * Create a texture from an existing surface.
     *
     * The surface is not modified or freed by this function.
     *
     * The SDL_TextureAccess hint for the created texture is
     * `SDL_TEXTUREACCESS_STATIC`.
     *
     * The pixel format of the created texture may be different from the pixel
     * format of the surface. Use SDL_QueryTexture() to query the pixel format of
     * the texture.
     *
     * \param renderer the rendering context.
     * \param surface the SDL_Surface structure containing pixel data used to fill
     *                the texture.
     * \returns the created texture or NULL on failure; call SDL_GetError() for
     *          more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateTexture
     * \sa SDL_DestroyTexture
     * \sa SDL_QueryTexture
     */
    static SDL_Texture* _CreateTextureFromSurface(
        SDL_Renderer *renderer, 
        SDL_Surface *surface
    );
    
    /**
     * Query the attributes of a texture.
     *
     * \param texture the texture to query.
     * \param format a pointer filled in with the raw format of the texture; the
     *               actual format may differ, but pixel transfers will use this
     *               format (one of the SDL_PixelFormatEnum values). This argument
     *               can be NULL if you don't need this information.
     * \param access a pointer filled in with the actual access to the texture
     *               (one of the SDL_TextureAccess values). This argument can be
     *               NULL if you don't need this information.
     * \param w a pointer filled in with the width of the texture in pixels. This
     *          argument can be NULL if you don't need this information.
     * \param h a pointer filled in with the height of the texture in pixels. This
     *          argument can be NULL if you don't need this information.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateTexture
     */
    static int _QueryTexture(
        SDL_Texture *texture, 
        Uint32 *format, 
        int *access, 
        int *w, 
        int *h
    );
    
    /**
     * Update the given texture rectangle with new pixel data.
     *
     * The pixel data must be in the pixel format of the texture. Use
     * SDL_QueryTexture() to query the pixel format of the texture.
     *
     * This is a fairly slow function, intended for use with static textures that
     * do not change often.
     *
     * If the texture is intended to be updated often, it is preferred to create
     * the texture as streaming and use the locking functions referenced below.
     * While this function will work with streaming textures, for optimization
     * reasons you may not get the pixels back if you lock the texture afterward.
     *
     * \param texture the texture to update.
     * \param rect an SDL_Rect structure representing the area to update, or NULL
     *             to update the entire texture.
     * \param pixels the raw pixel data in the format of the texture.
     * \param pitch the number of bytes in a row of pixel data, including padding
     *              between lines.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateTexture
     * \sa SDL_LockTexture
     * \sa SDL_UnlockTexture
     */
    static int _UpdateTexture(
        SDL_Texture *texture, 
        const SDL_Rect *rect, 
        const void *pixels, 
        int pitch
    );
    
    /**
     * Update a rectangle within a planar YV12 or IYUV texture with new pixel
     * data.
     *
     * You can use SDL_UpdateTexture() as long as your pixel data is a contiguous
     * block of Y and U/V planes in the proper order, but this function is
     * available if your pixel data is not contiguous.
     *
     * \param texture the texture to update.
     * \param rect a pointer to the rectangle of pixels to update, or NULL to
     *             update the entire texture.
     * \param Yplane the raw pixel data for the Y plane.
     * \param Ypitch the number of bytes between rows of pixel data for the Y
     *               plane.
     * \param Uplane the raw pixel data for the U plane.
     * \param Upitch the number of bytes between rows of pixel data for the U
     *               plane.
     * \param Vplane the raw pixel data for the V plane.
     * \param Vpitch the number of bytes between rows of pixel data for the V
     *               plane.
     * \returns 0 on success or -1 if the texture is not valid; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.1.
     *
     * \sa SDL_UpdateTexture
     */
    static int _UpdateYUVTexture(
        SDL_Texture *texture, 
        const SDL_Rect *rect, 
        const Uint8 *Yplane, 
        int Ypitch, 
        const Uint8 *Uplane, 
        int Upitch, 
        const Uint8 *Vplane, 
        int Vpitch
    );
    
    /**
     * Lock a portion of the texture for **write-only** pixel access.
     *
     * As an optimization, the pixels made available for editing don't necessarily
     * contain the old texture data. This is a write-only operation, and if you
     * need to keep a copy of the texture data you should do that at the
     * application level.
     *
     * You must use SDL_UnlockTexture() to unlock the pixels and apply any
     * changes.
     *
     * \param texture the texture to lock for access, which was created with
     *                `SDL_TEXTUREACCESS_STREAMING`.
     * \param rect an SDL_Rect structure representing the area to lock for access;
     *             NULL to lock the entire texture.
     * \param pixels this is filled in with a pointer to the locked pixels,
     *               appropriately offset by the locked area.
     * \param pitch this is filled in with the pitch of the locked pixels; the
     *              pitch is the length of one row in bytes.
     * \returns 0 on success or a negative error code if the texture is not valid
     *          or was not created with `SDL_TEXTUREACCESS_STREAMING`; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_UnlockTexture
     */
    static int _LockTexture(
        SDL_Texture *texture, 
        const SDL_Rect *rect, 
        void **pixels, 
        int *pitch
    );
    
    
    /**
     * Unlock a texture, uploading the changes to video memory, if needed.
     *
     * **Warning**: Please note that SDL_LockTexture() is intended to be
     * write-only; it will not guarantee the previous contents of the texture will
     * be provided. You must fully initialize any area of a texture that you lock
     * before unlocking it, as the pixels might otherwise be uninitialized memory.
     *
     * Which is to say: locking and immediately unlocking a texture can result in
     * corrupted textures, depending on the renderer in use.
     *
     * \param texture a texture locked by SDL_LockTexture().
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_LockTexture
     */
    static void _UnlockTexture(SDL_Texture *texture);
    
    /**
     * Set the blend mode for a texture, used by SDL_RenderCopy().
     *
     * If the blend mode is not supported, the closest supported mode is chosen
     * and this function returns -1.
     *
     * \param texture the texture to update.
     * \param blendMode the SDL_BlendMode to use for texture blending.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetTextureBlendMode
     * \sa SDL_RenderCopy
     */
    static int _SetTextureBlendMode(
        SDL_Texture *texture, 
        SDL_BlendMode blendMode
    );
    
    /**
     * Get the blend mode used for texture copy operations.
     *
     * \param texture the texture to query.
     * \param blendMode a pointer filled in with the current SDL_BlendMode.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_SetTextureBlendMode
     */
    static int _GetTextureBlendMode(
        SDL_Texture *texture, 
        SDL_BlendMode *blendMode
    );
    
    /**
     * Set the scale mode used for texture scale operations.
     *
     * If the scale mode is not supported, the closest supported mode is chosen.
     *
     * \param texture The texture to update.
     * \param scaleMode the SDL_ScaleMode to use for texture scaling.
     * \returns 0 on success, or -1 if the texture is not valid.
     *
     * \since This function is available since SDL 2.0.12.
     *
     * \sa SDL_GetTextureScaleMode
     */
    static int _SetTextureScaleMode(
        SDL_Texture *texture, 
        SDL_ScaleMode scaleMode
    );
    
    /**
     * Get the scale mode used for texture scale operations.
     *
     * \param texture the texture to query.
     * \param scaleMode a pointer filled in with the current scale mode.
     * \return 0 on success, or -1 if the texture is not valid.
     *
     * \since This function is available since SDL 2.0.12.
     *
     * \sa SDL_SetTextureScaleMode
     */
    static int _GetTextureScaleMode(
        SDL_Texture *texture, 
        SDL_ScaleMode *scaleMode
    );
    
    /**
     * Set an additional alpha value multiplied into render copy operations.
     *
     * When this texture is rendered, during the copy operation the source alpha
     * value is modulated by this alpha value according to the following formula:
     *
     * `srcA = srcA * (alpha / 255)`
     *
     * Alpha modulation is not always supported by the renderer; it will return -1
     * if alpha modulation is not supported.
     *
     * \param texture the texture to update.
     * \param alpha the source alpha value multiplied into copy operations.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetTextureAlphaMod
     * \sa SDL_SetTextureColorMod
     */
    static int _SetTextureAlphaMod(
        SDL_Texture *texture, 
        Uint8 alpha
    );
    
    /**
     * Get the additional alpha value multiplied into render copy operations.
     *
     * \param texture the texture to query.
     * \param alpha a pointer filled in with the current alpha value.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetTextureColorMod
     * \sa SDL_SetTextureAlphaMod
     */
    static int _GetTextureAlphaMod(
        SDL_Texture *texture, 
        Uint8 *alpha
    );
    
    /**
     * Set an additional color value multiplied into render copy operations.
     *
     * When this texture is rendered, during the copy operation each source color
     * channel is modulated by the appropriate color value according to the
     * following formula:
     *
     * `srcC = srcC * (color / 255)`
     *
     * Color modulation is not always supported by the renderer; it will return -1
     * if color modulation is not supported.
     *
     * \param texture the texture to update.
     * \param r the red color value multiplied into copy operations.
     * \param g the green color value multiplied into copy operations.
     * \param b the blue color value multiplied into copy operations.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetTextureColorMod
     * \sa SDL_SetTextureAlphaMod
     */
    static int _SetTextureColorMod(
        SDL_Texture *texture, 
        Uint8 r, 
        Uint8 g, 
        Uint8 b
    );
    
    /**
     * Get the additional color value multiplied into render copy operations.
     *
     * \param texture the texture to query.
     * \param r a pointer filled in with the current red color value.
     * \param g a pointer filled in with the current green color value.
     * \param b a pointer filled in with the current blue color value.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetTextureAlphaMod
     * \sa SDL_SetTextureColorMod
     */
    static int _GetTextureColorMod(
        SDL_Texture *texture, 
        Uint8 *r, 
        Uint8 *g, 
        Uint8 *b
    );
    
    /**
     * Destroy the specified texture.
     *
     * Passing NULL or an otherwise invalid texture will set the SDL error message
     * to "Invalid texture".
     *
     * \param texture the texture to destroy.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateTexture
     * \sa SDL_CreateTextureFromSurface
     */
    static void _DestroyTexture(SDL_Texture *texture);





    // Drawing Functions --------------------------------------------------------------------------

    /**
     * Clear the current rendering target with the drawing color.
     *
     * This function clears the entire rendering target, ignoring the viewport and
     * the clip rectangle.
     *
     * \param renderer the rendering context.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderClear(SDL_Renderer *renderer);

    /**
     * Draw a point on the current rendering target.
     *
     * SDL_RenderDrawPoint() draws a single point. If you want to draw multiple,
     * use SDL_RenderDrawPoints() instead.
     *
     * \param renderer the rendering context.
     * \param x the x coordinate of the point.
     * \param y the y coordinate of the point.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawPoint(
        SDL_Renderer *renderer, 
        int x, 
        int y
    );
    
    /**
     * Draw multiple points on the current rendering target.
     *
     * \param renderer the rendering context.
     * \param points an array of SDL_Point structures that represent the points to
     *               draw.
     * \param count the number of points to draw.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawPoints(
        SDL_Renderer *renderer, 
        const SDL_Point *points, 
        int count
    );
    
    /**
     * Draw a line on the current rendering target.
     *
     * SDL_RenderDrawLine() draws the line to include both end points. If you want
     * to draw multiple, connecting lines use SDL_RenderDrawLines() instead.
     *
     * \param renderer the rendering context.
     * \param x1 the x coordinate of the start point.
     * \param y1 the y coordinate of the start point.
     * \param x2 the x coordinate of the end point.
     * \param y2 the y coordinate of the end point.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawLine(
        SDL_Renderer *renderer, 
        int x1, 
        int y1, 
        int x2, 
        int y2
    );

    /* Overload for _RenderDrawLine(SDL_Renderer *r, int x1, int y1, int x2, int y2); */
    static int _RenderDrawLine(
        SDL_Renderer *renderer, 
        const SDL_Point p1, 
        const SDL_Point p2
    );
    
    
    /**
     * Draw a series of connected lines on the current rendering target.
     *
     * \param renderer the rendering context.
     * \param points an array of SDL_Point structures representing points along
     *               the lines.
     * \param count the number of points, drawing count-1 lines.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawLines(
        SDL_Renderer *renderer, 
        const SDL_Point *points, 
        int count
    );
    
    /**
     * Draw a rectangle on the current rendering target.
     *
     * \param renderer the rendering context.
     * \param rect an SDL_Rect structure representing the rectangle to draw, or
     *             NULL to outline the entire rendering target.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawRect(
        SDL_Renderer *renderer, 
        const SDL_Rect *rect
    );
    
    /**
     * Draw some number of rectangles on the current rendering target.
     *
     * \param renderer the rendering context.
     * \param rects an array of SDL_Rect structures representing the rectangles to
     *              be drawn.
     * \param count the number of rectangles.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderDrawRects(
        SDL_Renderer *renderer, 
        const SDL_Rect *rects, 
        int count
    );
    
    /**
     * Fill a rectangle on the current rendering target with the drawing color.
     *
     * The current drawing color is set by SDL_SetRenderDrawColor(), and the
     * color's alpha value is ignored unless blending is enabled with the
     * appropriate call to SDL_SetRenderDrawBlendMode().
     *
     * \param renderer the rendering context.
     * \param rect the SDL_Rect structure representing the rectangle to fill, or
     *             NULL for the entire rendering target.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRects
     * \sa SDL_RenderPresent
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static int _RenderFillRect(
        SDL_Renderer *renderer, 
        const SDL_Rect *rect
    );
    
    /**
     * Fill some number of rectangles on the current rendering target with the
     * drawing color.
     *
     * \param renderer the rendering context.
     * \param rects an array of SDL_Rect structures representing the rectangles to
     *              be filled.
     * \param count the number of rectangles.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderPresent
     */
    static int _RenderFillRects(
        SDL_Renderer *renderer, 
        const SDL_Rect *rects, 
        int count
    );
    
    /**
     * Copy a portion of the texture to the current rendering target.
     *
     * The texture is blended with the destination based on its blend mode set
     * with SDL_SetTextureBlendMode().
     *
     * The texture color is affected based on its color modulation set by
     * SDL_SetTextureColorMod().
     *
     * The texture alpha is affected based on its alpha modulation set by
     * SDL_SetTextureAlphaMod().
     *
     * \param renderer the rendering context.
     * \param texture the source texture.
     * \param srcrect the source SDL_Rect structure or NULL for the entire
     *                texture.
     * \param dstrect the destination SDL_Rect structure or NULL for the entire
     *                rendering target; the texture will be stretched to fill the
     *                given rectangle.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderCopyEx
     * \sa SDL_SetTextureAlphaMod
     * \sa SDL_SetTextureBlendMode
     * \sa SDL_SetTextureColorMod
     */
    static int _RenderCopy(
        SDL_Renderer *renderer, 
        SDL_Texture *texture, 
        const SDL_Rect *srcrect, 
        const SDL_Rect *dstrect
    );
    
    /**
     * Copy a portion of the texture to the current rendering, with optional
     * rotation and flipping.
     *
     * Copy a portion of the texture to the current rendering target, optionally
     * rotating it by angle around the given center and also flipping it
     * top-bottom and/or left-right.
     *
     * The texture is blended with the destination based on its blend mode set
     * with SDL_SetTextureBlendMode().
     *
     * The texture color is affected based on its color modulation set by
     * SDL_SetTextureColorMod().
     *
     * The texture alpha is affected based on its alpha modulation set by
     * SDL_SetTextureAlphaMod().
     *
     * \param renderer the rendering context.
     * \param texture the source texture.
     * \param srcrect the source SDL_Rect structure or NULL for the entire
     *                texture.
     * \param dstrect the destination SDL_Rect structure or NULL for the entire
     *                rendering target.
     * \param angle an angle in degrees that indicates the rotation that will be
     *              applied to dstrect, rotating it in a clockwise direction.
     * \param center a pointer to a point indicating the point around which
     *               dstrect will be rotated (if NULL, rotation will be done
     *               around `dstrect.w / 2`, `dstrect.h / 2`).
     * \param flip a SDL_RendererFlip value stating which flipping actions should
     *             be performed on the texture.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderCopy
     * \sa SDL_SetTextureAlphaMod
     * \sa SDL_SetTextureBlendMode
     * \sa SDL_SetTextureColorMod
     */
    static int _RenderCopyEx(
        SDL_Renderer *renderer, 
        SDL_Texture *texture, 
        const SDL_Rect *srcrect, 
        const SDL_Rect *dstrect, 
        double angle, 
        const SDL_Point *center, 
        SDL_RendererFlip flip
    );
    
    /**
     * Read pixels from the current rendering target to an array of pixels.
     *
     * **WARNING**: This is a very slow operation, and should not be used
     * frequently. If you're using this on the main rendering target, it should be
     * called after rendering and before SDL_RenderPresent().
     *
     * `pitch` specifies the number of bytes between rows in the destination
     * `pixels` data. This allows you to write to a subrectangle or have padded
     * rows in the destination. Generally, `pitch` should equal the number of
     * pixels per row in the `pixels` data times the number of bytes per pixel,
     * but it might contain additional padding (for example, 24bit RGB Windows
     * Bitmap data pads all rows to multiples of 4 bytes).
     *
     * \param renderer the rendering context.
     * \param rect an SDL_Rect structure representing the area to read, or NULL
     *             for the entire render target.
     * \param format an SDL_PixelFormatEnum value of the desired format of the
     *               pixel data, or 0 to use the format of the rendering target.
     * \param pixels a pointer to the pixel data to copy into.
     * \param pitch the pitch of the `pixels` parameter.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     */
    static int _RenderReadPixels(
        SDL_Renderer *renderer, 
        const SDL_Rect *rect, 
        Uint32 format, 
        void *pixels, 
        int pitch
    );
    
    /**
     * Render a list of triangles, optionally using a texture and indices into the
     * vertex array Color and alpha modulation is done per vertex
     * (SDL_SetTextureColorMod and SDL_SetTextureAlphaMod are ignored).
     *
     * \param renderer The rendering context.
     * \param texture (optional) The SDL texture to use.
     * \param vertices Vertices.
     * \param num_vertices Number of vertices.
     * \param indices (optional) An array of integer indices into the 'vertices'
     *                array, if NULL all vertices will be rendered in sequential
     *                order.
     * \param num_indices Number of indices.
     * \return 0 on success, or -1 if the operation is not supported.
     *
     * \since This function is available since SDL 2.0.18.
     *
     * \sa SDL_RenderGeometryRaw
     * \sa SDL_Vertex
     */
    static int _RenderGeometry(
        SDL_Renderer *renderer, 
        SDL_Texture *texture, 
        const SDL_Vertex *vertices, 
        int num_vertices, 
        const int *indices, 
        int num_indices
    );
    
    /**
     * Update the screen with any rendering performed since the previous call.
     *
     * SDL's rendering functions operate on a backbuffer; that is, calling a
     * rendering function such as SDL_RenderDrawLine() does not directly put a
     * line on the screen, but rather updates the backbuffer. As such, you compose
     * your entire scene and *present* the composed backbuffer to the screen as a
     * complete picture.
     *
     * Therefore, when using SDL's rendering API, one does all drawing intended
     * for the frame, and then calls this function once per frame to present the
     * final drawing to the user.
     *
     * The backbuffer should be considered invalidated after each present; do not
     * assume that previous contents will exist between frames. You are strongly
     * encouraged to call SDL_RenderClear() to initialize the backbuffer before
     * starting each new frame's drawing, even if you plan to overwrite every
     * pixel.
     *
     * \param renderer the rendering context.
     *
     * \threadsafety You may only call this function on the main thread. If this
     *               happens to work on a background thread on any given platform
     *               or backend, it's purely by luck and you should not rely on it
     *               to work next time.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateRenderer
     * \sa SDL_RenderClear
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     * \sa SDL_SetRenderDrawBlendMode
     * \sa SDL_SetRenderDrawColor
     */
    static void _RenderPresent(SDL_Renderer *renderer);





    // Renderer Create and Managment --------------------------------------------------------------

    /**
     * Create a 2D rendering context for a window.
     *
     * \param window the window where rendering is displayed.
     * \param index the index of the rendering driver to initialize, or -1 to
     *              initialize the first one supporting the requested flags.
     * \param flags 0, or one or more SDL_RendererFlags OR'd together.
     * \returns a valid rendering context or NULL if there was an error; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateSoftwareRenderer
     * \sa SDL_DestroyRenderer
     * \sa SDL_GetNumRenderDrivers
     * \sa SDL_GetRendererInfo
     */
    static SDL_Renderer* _CreateRenderer(
        SDL_Window *window, 
        int index, 
        Uint32 flags
    );
    
    /**
     * Create a 2D software rendering context for a surface.
     *
     * Two other API which can be used to create SDL_Renderer:
     * SDL_CreateRenderer() and SDL_CreateWindowAndRenderer(). These can _also_
     * create a software renderer, but they are intended to be used with an
     * SDL_Window as the final destination and not an SDL_Surface.
     *
     * \param surface the SDL_Surface structure representing the surface where
     *                rendering is done.
     * \returns a valid rendering context or NULL if there was an error; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateRenderer
     * \sa SDL_CreateWindowAndRenderer
     * \sa SDL_DestroyRenderer
     */
    static SDL_Renderer* _CreateSoftwareRenderer(
        SDL_Surface *surface
    );
    
    /**
     * Get information about a rendering context.
     *
     * \param renderer the rendering context.
     * \param info an SDL_RendererInfo structure filled with information about the
     *             current renderer.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateRenderer
     */
    static int _GetRendererInfo(
        SDL_Renderer *renderer, 
        SDL_RendererInfo *info
    );
    
    /**
     * Get the renderer associated with a window.
     *
     * \param window the window to query.
     * \returns the rendering context on success or NULL on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateRenderer
     */
    static SDL_Renderer* _GetRenderer(SDL_Window *window);
    
    /**
     * Get the output size in pixels of a rendering context.
     *
     * Due to high-dpi displays, you might end up with a rendering context that
     * has more pixels than the window that contains it, so use this instead of
     * SDL_GetWindowSize() to decide how much drawing area you have.
     *
     * \param renderer the rendering context.
     * \param w an int filled with the width.
     * \param h an int filled with the height.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetRenderer
     */
    static int _GetRendererOutputSize(
        SDL_Renderer *renderer, 
        int *w, 
        int *h
    );
    
    /**
     * Destroy the rendering context for a window and free associated textures.
     *
     * If `renderer` is NULL, this function will return immediately after setting
     * the SDL error message to "Invalid renderer". See SDL_GetError().
     *
     * \param renderer the rendering context.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_CreateRenderer
     */
    static void _DestroyRenderer(SDL_Renderer *renderer);





    // Render Target Managment --------------------------------------------------------------------

    /**
     * Set a texture as the current rendering target.
     *
     * Before using this function, you should check the
     * `SDL_RENDERER_TARGETTEXTURE` bit in the flags of SDL_RendererInfo to see if
     * render targets are supported.
     *
     * The default render target is the window for which the renderer was created.
     * To stop rendering to a texture and render to the window again, call this
     * function with a NULL `texture`. This will reset the renderer's viewport,
     * clipping rectangle, and scaling settings to the state they were in before
     * setting a non-NULL `texture` target, losing any changes made in the
     * meantime.
     *
     * \param renderer the rendering context.
     * \param texture the targeted texture, which must be created with the
     *                `SDL_TEXTUREACCESS_TARGET` flag, or NULL to render to the
     *                window instead of a texture.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetRenderTarget
     */
    static int _SetRenderTarget(
        SDL_Renderer *renderer, 
        SDL_Texture *texture
    );

    /**
     * Get the current render target.
     *
     * The default render target is the window for which the renderer was created,
     * and is reported as NULL here.
     *
     * \param renderer the rendering context.
     * \returns the current render target or NULL for the default render target.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_SetRenderTarget
     */
    static SDL_Texture* _GetRenderTarget(SDL_Renderer *renderer);





    // Viewport and Clipping ----------------------------------------------------------------------
    
    /**
     * Set the drawing area for rendering on the current target.
     *
     * When the window is resized, the viewport is reset to fill the entire new
     * window size.
     *
     * \param renderer the rendering context.
     * \param rect the SDL_Rect structure representing the drawing area, or NULL
     *             to set the viewport to the entire target.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderGetViewport
     */
    static int _RenderSetViewport(
        SDL_Renderer *renderer, 
        const SDL_Rect *rect
    );
    
    /**
     * Get the drawing area for the current target.
     *
     * \param renderer the rendering context.
     * \param rect an SDL_Rect structure filled in with the current drawing area.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderSetViewport
     */
    static void _RenderGetViewport(
        SDL_Renderer *renderer, 
        SDL_Rect *rect
    );
    
    /**
     * Set the clip rectangle for rendering on the specified target.
     *
     * \param renderer the rendering context for which clip rectangle should be
     *                 set.
     * \param rect an SDL_Rect structure representing the clip area, relative to
     *             the viewport, or NULL to disable clipping.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderGetClipRect
     * \sa SDL_RenderIsClipEnabled
     */
    static int _RenderSetClipRect(
        SDL_Renderer *renderer, 
        const SDL_Rect *rect
    );
    
    /**
     * Get the clip rectangle for the current target.
     *
     * \param renderer the rendering context from which clip rectangle should be
     *                 queried.
     * \param rect an SDL_Rect structure filled in with the current clipping area
     *             or an empty rectangle if clipping is disabled.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderIsClipEnabled
     * \sa SDL_RenderSetClipRect
     */
    static void _RenderGetClipRect(
        SDL_Renderer *renderer, 
        SDL_Rect *rect
    );
    
    /**
     * Get whether clipping is enabled on the given renderer.
     *
     * \param renderer the renderer from which clip state should be queried.
     * \returns SDL_TRUE if clipping is enabled or SDL_FALSE if not; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.4.
     *
     * \sa SDL_RenderGetClipRect
     * \sa SDL_RenderSetClipRect
     */
    static SDL_bool _RenderIsClipEnabled(SDL_Renderer *renderer);

    
    
    
    
    // Scaling and Logical Size -------------------------------------------------------------------
    
    /**
     * Set the drawing scale for rendering on the current target.
     *
     * The drawing coordinates are scaled by the x/y scaling factors before they
     * are used by the renderer. This allows resolution independent drawing with a
     * single coordinate system.
     *
     * If this results in scaling or subpixel drawing by the rendering backend, it
     * will be handled using the appropriate quality hints. For best results use
     * integer scaling factors.
     *
     * \param renderer a rendering context.
     * \param scaleX the horizontal scaling factor.
     * \param scaleY the vertical scaling factor.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderGetScale
     * \sa SDL_RenderSetLogicalSize
     */
    static int _RenderSetScale(SDL_Renderer *renderer, float scaleX, float scaleY);
    
    /**
     * Get the drawing scale for the current target.
     *
     * \param renderer the renderer from which drawing scale should be queried.
     * \param scaleX a pointer filled in with the horizontal scaling factor.
     * \param scaleY a pointer filled in with the vertical scaling factor.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderSetScale
     */
    static void _RenderGetScale(SDL_Renderer *renderer, float *scaleX, float *scaleY);
    
    /**
     * Set a device independent resolution for rendering.
     *
     * This function uses the viewport and scaling functionality to allow a fixed
     * logical resolution for rendering, regardless of the actual output
     * resolution. If the actual output resolution doesn't have the same aspect
     * ratio the output rendering will be centered within the output display.
     *
     * If the output display is a window, mouse and touch events in the window
     * will be filtered and scaled so they seem to arrive within the logical
     * resolution. The SDL_HINT_MOUSE_RELATIVE_SCALING hint controls whether
     * relative motion events are also scaled.
     *
     * If this function results in scaling or subpixel drawing by the rendering
     * backend, it will be handled using the appropriate quality hints.
     *
     * \param renderer the renderer for which resolution should be set.
     * \param w the width of the logical resolution.
     * \param h the height of the logical resolution.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderGetLogicalSize
     */
    static int _RenderSetLogicalSize(SDL_Renderer *renderer, int w, int h);
    
    /**
     * Get device independent resolution for rendering.
     *
     * When using the main rendering target (eg no target texture is set): this
     * may return 0 for `w` and `h` if the SDL_Renderer has never had its logical
     * size set by SDL_RenderSetLogicalSize(). Otherwise it returns the logical
     * width and height.
     *
     * When using a target texture: Never return 0 for `w` and `h` at first. Then
     * it returns the logical width and height that are set.
     *
     * \param renderer a rendering context.
     * \param w an int to be filled with the width.
     * \param h an int to be filled with the height.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_RenderSetLogicalSize
     */
    static void _RenderGetLogicalSize(SDL_Renderer *renderer, int *w, int *h);





    // Drawing Color and Blend Mode ---------------------------------------------------------------
    
    /**
     * Set the color used for drawing operations (Rect, Line and Clear).
     *
     * Set the color for drawing or filling rectangles, lines, and points, and for
     * SDL_RenderClear().
     *
     * \param renderer the rendering context.
     * \param r the red value used to draw on the rendering target.
     * \param g the green value used to draw on the rendering target.
     * \param b the blue value used to draw on the rendering target.
     * \param a the alpha value used to draw on the rendering target; usually
     *          `SDL_ALPHA_OPAQUE` (255). Use SDL_SetRenderDrawBlendMode to
     *          specify how the alpha channel is used.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetRenderDrawColor
     * \sa SDL_RenderClear
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     */
    static int _SetRenderDrawColor(
        SDL_Renderer *renderer, 
        Uint8 r, 
        Uint8 g, 
        Uint8 b, 
        Uint8 a
    );

    /* Overload function for _SetRenderDrawColor(SDL_Renderer *r, Uint8 r, Uint8 g, Uint8 b, Uint8 a) */
    static int _SetRenderDrawColor(
        SDL_Renderer *renderer, 
        SDL_Color color
    );
    
    /**
     * Get the color used for drawing operations (Rect, Line and Clear).
     *
     * \param renderer the rendering context.
     * \param r a pointer filled in with the red value used to draw on the
     *          rendering target.
     * \param g a pointer filled in with the green value used to draw on the
     *          rendering target.
     * \param b a pointer filled in with the blue value used to draw on the
     *          rendering target.
     * \param a a pointer filled in with the alpha value used to draw on the
     *          rendering target; usually `SDL_ALPHA_OPAQUE` (255).
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_SetRenderDrawColor
     */
    static int _GetRenderDrawColor(
        SDL_Renderer *renderer, 
        Uint8 *r, 
        Uint8 *g, 
        Uint8 *b, 
        Uint8 *a
    );
    
    /**
     * Set the blend mode used for drawing operations (Fill and Line).
     *
     * If the blend mode is not supported, the closest supported mode is chosen.
     *
     * \param renderer the rendering context.
     * \param blendMode the SDL_BlendMode to use for blending.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_GetRenderDrawBlendMode
     * \sa SDL_RenderDrawLine
     * \sa SDL_RenderDrawLines
     * \sa SDL_RenderDrawPoint
     * \sa SDL_RenderDrawPoints
     * \sa SDL_RenderDrawRect
     * \sa SDL_RenderDrawRects
     * \sa SDL_RenderFillRect
     * \sa SDL_RenderFillRects
     */
    static int _SetRenderDrawBlendMode(
        SDL_Renderer *renderer, 
        SDL_BlendMode blendMode
    );
    
    /**
     * Get the blend mode used for drawing operations.
     *
     * \param renderer the rendering context.
     * \param blendMode a pointer filled in with the current SDL_BlendMode.
     * \returns 0 on success or a negative error code on failure; call
     *          SDL_GetError() for more information.
     *
     * \since This function is available since SDL 2.0.0.
     *
     * \sa SDL_SetRenderDrawBlendMode
     */
    static int _GetRenderDrawBlendMode(SDL_Renderer *renderer, SDL_BlendMode *blendMode);
};






/** GENERAL STRUCT FOR IMAGES -----------------------------------------------------------------------
 * This is a TextureData object which allows easy managment of Textures
 * 
 * It holds all of the relative data about a texture such as:
 *      - Format
 *      - Access
 *      - Width
 *      - Heigh
 *      - Id
 *      - Path
 *      - Original Width
 *      - Original Height
 * 
 * The SDL_Texture* variable is called tex and its read-only so
 * the only way to set a new value for it would be to use build-in
 * function called .setTexture() which handles automaticly what happends
 * with the prviously set texture. The mechanism:
 *  - If the object was just created the default value for tex would be nullptr
 *    in which case the new texture passed to the setTextre() would be just set
 *  - If there was already some texture set as tex, the function checks for the
 *    global variable called AUTO_DELETE_TEXTURES which if its set to true that
 *    means that if no other objects are pointing to the old SDL_Texture* it will
 *    be deleted, if the AUTO_DELETE_TEXTURES is set to false, or any other TextureData
 *    object is pointing to the old SDL_Texture* that it will be just replaced and
 *    not freeed
 * 
 * Also it is important to note that creating a TextureData object directly is 
 * illegal, in order to create a TextureData object you need to you TextureDataPtr
 * which is a typedef expantion of the TextureData class, it expands to:
 *   using TextureDataPtr = std::shared_ptr<TextureData>;
 * That is becuase the TM class must have the access to all of the created TextureData
 * objects so it must be a shared pointer.
 * 
 * In order to create a object you would do something like this:
 *      TextureDataPtr myTexture = TextureData::create(...);
 * 
 * 
 */  


/**
class TextureData{
    friend class TM;
private:
    / * This is the real variable that holds the 
     * value of the object's texture.
     * It can only be modified internaly
     * using the .setTexture(SDL_Texture*) function
     * /
    SDL_Texture* realTexture;
                                

public:
    SDL_Texture* const& tex;    // This is exposed SDL_Texture pointer that is set to be read-only
    string id;                  // Id used mostly as a label so user can distinguish different objects
    Uint32 format;              // Texture Format
    int access;                 // Texture Access
    int width;                  // Current Texture Width
    int height;                 // Current Texture Height
    int orgWidth;               // Original Texture Width
    int orgHeight;              // Original Texture Height
    string path;                // If texture was loaded from a file

    // Factory method: creates and returns a shared_ptr to a TextureData instance.
    static TextureDataPtr create(
        SDL_Texture* texture = nullptr,
        string id            = "empty",
        Uint32 format        = SDL_PIXELFORMAT_RGBA32,
        int access           = SDL_TEXTUREACCESS_TARGET,
        int width            = -1,
        int height           = -1,
        string path          = ""
    ) {
        TextureDataPtr ptr(new TextureData(
            texture, 
            id,
            format,
            access,
            width,
            height,
            path
        ));
        TM::registerTexture(ptr);
        return ptr;
    }

    // Delete copy constructor and assignment to avoid accidental copies.
    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;
    friend std::ostream& operator<<(std::ostream& os, const TextureData& td);

    void setTexture(SDL_Texture* newTex){
        if(realTexture == nullptr){
            realTexture = newTex;
            return;
        }

        auto it = std::find_if(
            TM::loadedTextures.begin(), 
            TM::loadedTextures.end(),
            [this](const weak_ptr<TextureData>& ptr){
                if(auto sp = ptr.lock()){
                    return sp->realTexture == this->realTexture;
                }
                return false;
            }
        );

        // No other object is using the old texture
        if(it == TM::loadedTextures.end()){

            // If auto delete is turned on, free the old texture
            // Otherwise just put the texture into outOfScopeTextures vector
            if(TM::AUTO_DELETE_TEXTURES){
                TM::_DestroyTexture(realTexture);
            } else{
                TM::outOfScopeTextures.push_back(realTexture);
            }
        }
        
        // Then update the texture.
        realTexture = newTex;
    }

    // TextureData destructor
    // It frees the SDL_Texture* and resets all variables to 0
    ~TextureData() {
        // Perform any necessary cleanup here.
        if(realTexture) {
            TM::_DestroyTexture(tex);
            realTexture = nullptr;
        }

        TM::removeTexture(this);

        id = "[Deleted]";
        format = 0;
        access = 0;
        width = 0;
        height = 0;
        orgWidth = 0;
        orgHeight = 0;
        path = "";
    }


// PRIVATE CONSTRUCTORS AND VARIABLES
private:
    // Default Constructor
    TextureData(
        SDL_Texture* t,
        string _id,
        Uint32 f,
        int a,
        int w,
        int h,
        string p
    ) : realTexture(t),
        tex(realTexture),
        id(_id),
        format(f),
        access(a),
        width(w),
        height(h),
        orgWidth(w),
        orgHeight(h),
        path(p)
    {}
};


*/

#endif
// Creator: @AndrijaRD