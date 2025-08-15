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

extern "C" {
    #include "../external/nanosvg.h"
    #include "../external/nanosvgrast.h"
}

#include <functional>
using PixelMapper = std::function<SDL_Color(uint8_t, uint8_t, uint8_t, uint8_t)>;


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
    SDL_Texture*      getTexture() const { return dptr_->texture; }
    SDL_PixelFormat   getFormat()  const { return dptr_->format; }
    SDL_TextureAccess getAccess()  const { return dptr_->access; }
    int               getWidth()   const { return dptr_->width; }
    int               getHeight()  const { return dptr_->height; }

    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // SAFE MUTATORS
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

    // Replace the SDL_Texture*.  If no other TextureData still points at
    // the old texture, it will be destroyed (or stashed in outOfScopeTextures).
    void setTexture(SDL_Texture* newTex);

    // Manually re‑query width/height/format/access
    void reloadInfo();

    void printf(bool full = false) const;

    static inline SDL_PixelFormat defaultPixelFormat = SDL_PIXELFORMAT_RGBA32;
    static inline SDL_TextureAccess defaultAccess = SDL_TEXTUREACCESS_TARGET;
    
        
private:
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // THE IMPL—this lives on the heap, shared by copies of the handle
    //–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    struct Impl {
        SDL_Texture*        texture = nullptr;
        SDL_PixelFormat     format  = defaultPixelFormat;
        SDL_TextureAccess   access  = defaultAccess;
        int                 width   = 0;
        int                 height  = 0;
    };

    std::shared_ptr<Impl> dptr_;
};



class SVGIcon {
    friend class TM;
private:
    string path;
    NSVGimage *image;

    unordered_map<int, SDL_Texture*> sizes;

    SDL_Texture* loadSize(int size);

public:
    SDL_Texture* getIcon(int size);
};
    


class TM{
    friend class TextureData;
    friend class SVGIcon;
    friend class Sys;
private:
    // NSVG RASTERIZER
    static inline NSVGrasterizer *rast;

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
     * 
     * @return Error code (0 means no error)
     */
    static int loadTexture(
        TextureData& td, 
        const string& path, 
        const string& id = ""
    );

    /**
     * @brief Load *.svg file into a specific dimension Texture.
     * Convinient for loading and rendering icons.
     * 
     * @param td TextureData variable in whcch texture will be stored
     * @param path Path to the svg file
     * @param size Width and height of the texture
     * @return Error code (0 means no error)
     */
    static int loadSVG(
        SVGIcon&        td,
        const string&   path
    );



    /**
     * Creates a texture from a text using TTF_RenderUTF8_Blended.
     * It saves the texture into TextureDataPtr and fills the rest of the object data.
     * 
     * @param td Where should texture be saved
     * @param text Text string
     * @param fontSize Font Size
     * @param color Text Color
     * 
     * @return Error code (0 means no error)
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
     * 
     * @return Error code (0 means no error)
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
     * @param src Source Texture
     * @param dst Destination Texture
     * @param width Desired Width
     * @param height Desired Height
     * 
     * @return Error code (0 means no error)
     */
    static int resizeTexture(
        const TextureData& src,
        TextureData& dst,
        int& newWidth,
        int& newHeight
    );

    // OVERLOAD
    static int resizeTexture(
        const TextureData& src,
        TextureData& dst,
        const int& newWidth,
        const int& newHeight
    );


    /**
     * It exports a Texture into a png image, to the specified location.
     * 
     * @param path The files path, where texture should be saved to
     * @param td The Texture to be exported
     * 
     * @return Error code (0 means no error)
     */
    static int exportTexture(
        const string& path,
        const TextureData& td
    );

    /* Don't use unless nessery! */
    static int exportTexture(
        const string& path,
        SDL_Texture* tex
    );


    /**
     * It exports a SDL_Surface* into a image, png file.
     * 
     * @param path The file path, where surface should be saved to
     * @param surface SDL_Surface* to be exported
     * 
     * @return Error code (0 means no error)
     */
    static int exportSurface(
        const string& path,
        SDL_Surface* surface
    ); // UNTESTED


    /**
     * It rotates a Texture by 90, 180 or 270 degrees.
     * 
     * @param src Source Texture
     * @param dst Destination Texture
     * @param angle Angle for rotation (only 90, 180 and 270 are allowed)
     * 
     * @return Error code (0 means no error)
     */
    static int rotateTexture(
        const TextureData& src, 
        TextureData& dst,
        int angle = 90
    );


    /**
     * It Crops a Texture defined by SDL_Rect.
     * 
     * @param src Source Texture
     * @param dst Destination Texture
     * @param rect SDL_Rect defining the crop section
     * 
     * @return Error code (0 means no error)
     */
    static int cropTexture(
        const TextureData& src, 
        TextureData& dst, 
        SDL_Rect rect
    );


    /**
     * @brief Applies a per‑pixel transformation to an SDL texture.
     *
     * This templated function reads every pixel from the source texture, invokes the
     * user‑provided callable on its RGBA components, then writes the returned color
     * back into a new destination texture—all in a single pass.
     *
     * @tparam PixelFunc
     *   A callable type satisfying:
     *     SDL_Color PixelFunc(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
     *   i.e. takes four 8‑bit channel values and returns an SDL_Color.
     *
     * @param src
     *   The source TextureData object. Must wrap a valid SDL_Texture.  If the texture
     *   is not already in 32 bpp RGBA format, it will be converted internally.
     *
     * @param dst
     *   An uninitialized TextureData object. On success, will be populated with a new
     *   SDL_Texture containing the transformed pixels, plus updated width/height info.
     *
     * @param pixelFunc
     *   The per‑pixel mapping function. For each pixel in the source, this function
     *   is called exactly once with the current (r,g,b,a).  Its return value is clamped
     *   to [0,255] per channel and written to the corresponding pixel in the output.
     *
     * @returns
     *   NO_ERROR (0) on success, or one of:
     *     - INVALID_ARGUMENTS_PASSED if src is null or renderer is not initialized
     *     - error codes returned by convert_textureTo or ensureSurfaceFormat
     *     - error codes from convert_toTexture when creating the new SDL_Texture
     *
     * @notes
     *   - This function locks and unlocks the intermediate SDL_Surface internally.
     *   - All channel math and clamping happens in floating‑point, but final storage
     *     is in 8‑bit integers.
     *   - Because the loop visits every pixel exactly once, the overall complexity is
     *     O(width × height).  Use efficient lambdas to avoid excessive per‑pixel overhead.
     *
     * @example
     * // Convert a texture to grayscale:
     * auto toGray = [](uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
     *     float y = 0.299f*r + 0.587f*g + 0.114f*b;
     *     uint8_t yi = static_cast<uint8_t>(std::clamp(y, 0.0f, 255.0f));
     *     return SDL_Color{ yi, yi, yi, a };
     * };
     * TextureData dst;
     * int err = TM::transformTexture(src, dst, toGray);
     * if (err != NO_ERROR) { handle error }
    */
    static int transformTexture(
        const TextureData& src,
        TextureData& dst,
        PixelMapper pixelFunc
    );






    //––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // CONVERTING FUNCTIONS     /* OPENCV */
    //––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

    /**
     * Converts cv::Mat into TextureData, SDL_Texture.
     * RGBA format of Mat object required.
     * 
     * Use `cv::cvtColor(srcMat, dstMat, cv::COLOR_BGR2RGBA);`
     * 
     * @param cvMat RGBA format of cv::Mat object
     * @param td TextureData object where result will be saved
     * @return int error code (0 means no error)
     */
    static int convert_toTexture(
        const cv::Mat& cvMat, 
        TextureData& td
    );

    /**
     * Converts TextureData into cv::Mat.
     * Output is RGBA cv::Mat object
     * 
     * @param td TextureData object to be converted
     * @param cvMat cv::Mat object as output
     * @return int error code (0 means no error)
     */
    static int convert_textureTo(
        const TextureData& td, 
        cv::Mat& cvMat
    ); // UNTESTED




    //––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
    // CONVERTING FUNCTIONS     /* SURFACE */
    //––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

    /**
     * Converts SDL_Surface* into TextureData.
     * The the surface must be in SDL_PIXELFORMAT_RGBA32 format.
     * 
     * You can check the format (it needs to be 32):
     * `SDL_BYTESPERPIXEL(surface->format);`
     * 
     * You can convert the Surface with:
     *  `SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);`
     * 
     * 
     * @param surface SDL_Surface* with SDL_PIXELFORMAT_RGBA32 format
     * @param td TextureData&
     * @return int error code (0 means no error)
     */
    static int convert_toTexture(
        const SDL_Surface* surface, 
        TextureData& td
    );
    
    /**
     * Converts SDL_Surface* into SDL_Texture*.
     * (INTENDED FOR INTERNAL USE ONLY)
     * 
     * Be sure to free the previous value of SDL_Texture* tex
     * if it was set.
     * 
     * @param surface SDL_Surface*
     * @param td SDL_Texture*
     * @return int error code (0 means no error)
     */
    static int convert_toTexture(
        const SDL_Surface* surface, 
        SDL_Texture*& tex
    );

    /**
     * Converts TextureData into SDL_Surface*.
     * 
     * @param td TextureData object to be converted
     * @param surface Result Surface
     * @return int error code (0 means no error)
     */
    static int convert_textureTo(
        const TextureData& td, 
        SDL_Surface*& surface
    );

    /**
     * Converts SDL_Texture* into SDL_Surface*.
     * 
     * @param tex SDL_Texture* to be converted
     * @param surface Result Surface
     * @return int error code (0 means no error)
     */
    static int convert_textureTo(
        SDL_Texture* tex, 
        SDL_Surface*& surface
    );






    /* SETTING FUNCTIONS */

    /**
     * Function used to set the AUTO_DELETE_TEXTURES propertie.
     */
    static void setAutoDeleteTextures(bool prop = true);
};




#endif