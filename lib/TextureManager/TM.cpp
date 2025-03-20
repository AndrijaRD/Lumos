#include "./TM.h"
#include "../System/Sys.h"
#include "../Gui/gui.h"



/** Load Texture
 * 
 * Load an image from filesystem into TextureData object.
 * 
 * @param td TextureData object into which image should be loaded.
 * @param path Path to the image on the filesystem
 * @return Returns 0 on success and positive number on error, coresponding to the ERROR DEFINITIONS 
 */
int TM::loadTexture(
    TextureData& td, 
    const string& path,
    const string& id
){
    // Ensure that privous Texture is de-loaded -----------------------------------------
    if(td.tex != nullptr) TM::freeTexture(td);

    // Load the image -------------------------------------------------------------------
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == nullptr) return TM_SURFACE_CREATE_ERROR;

    // Check the format of the image ----------------------------------------------------
    if(surface->format->BitsPerPixel != 32){
        SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);  // Free the original surface
        surface = converted;
        if(surface == nullptr){
            return TM_SURFACE_CONVERT_ERROR;
        }
    }


    // CREATING THE TEXTURE --------------------------------------------------------------
    td.tex = SDL_CreateTexture(
        Sys::renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        surface->w,
        surface->h
    );
    if(td.tex == nullptr) {
        SDL_FreeSurface(surface);
        return TM_TEXTURE_CREATE_ERROR;
    }

    int err = SDL_SetTextureScaleMode(td.tex, SDL_ScaleModeLinear);
    if(err){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_STSM_FAILED;
    }


    // FILL THE TEXTURE WITH THE IMAGE DATA -----------------------------------------------
    int s = SDL_UpdateTexture(td.tex, NULL, surface->pixels, surface->pitch);
    if(s != 0){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_UPDATE_ERROR;
    }


    // MAKE TEXTURE UPDATATBLE/MODIFIABLE -------------------------------------------------
    if(SDL_SetTextureBlendMode(td.tex, SDL_BLENDMODE_BLEND)){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_SET_BLENDMODE_ERROR;
    }

    // GET TEXTURE DIMENSIONS -------------------------------------------------------------
    SDL_QueryTexture(td.tex, &td.format, NULL, &td.width, &td.height);
    td.orgWidth = td.width;
    td.orgHeight = td.height;

    // FILL TD DATA
    td.path = path;
    if(id.empty()) td.id = fs::path(path).filename().string();
    else td.id = id;

    // CLEAN UP ---------------------------------------------------------------------------
    SDL_FreeSurface(surface);
    loadedTextures.insert({td.tex, td.id});
    
    return NO_ERROR;
}




/** Free Texture
 * 
 * Frees the texture from memory and sets the TextureData properties to null
 * @param td TextureData to be freeed
 */
void TM::freeTexture(TextureData& td){
    if(td.tex == nullptr) return;

    loadedTextures.erase(td.tex);

    SDL_DestroyTexture(td.tex);
    td.tex = nullptr;

    td.width = 0;
    td.height = 0;
}

// This should be avoided as it leaves dangling pointer and risks the use-after-free segmentation fault
// void TM::freeTexture(SDL_Texture* tex){
//     if(tex != nullptr){
//         auto it = std::remove(loadedTextures.begin(), loadedTextures.end(), tex);
//         loadedTextures.erase(it, loadedTextures.end());
//
//         SDL_DestroyTexture(tex);
//     }
// }





/** Cleanup 
 * 
 * Called at the end of the program to free all of the textures
*/
void TM::cleanup(){
    for(auto td : loadedTextures) SDL_DestroyTexture(td.first);
    loadedTextures.clear();
}





/** Get Number of Loaded Textures
 * 
 * Returns the number of currently loaded textures.
 */
int TM::countLoadedTextures(){ return loadedTextures.size(); }
void TM::printLoadedTextures(){
    cout << "There are " << loadedTextures.size() << " loaded Textures: " << endl;
    int i = 0;
    for(auto td : loadedTextures){
        cout << "\t(" << i << ") " << td.second << ": " << td.first << endl;
        i++;
    }
}
unordered_map<SDL_Texture*, string> TM::getLoadedTextures(){
    return loadedTextures;
}




/** Render Texture
 * 
 * Renders the texture on the screen width the specific x and y coordinates and custom width and height
 * 
 * As dr param function can take either width or hight as -1 meaning the function will determene that 
 * dimension automaticly using the aspect ratio of the image, and the dr propertie will be updated with
 * that newly calculated dimension.
 * 
 * Example: If func gets {10, 10, 100, -1} that means that height will be automaticly caluclated and
 * dr will be updated with that and then rendered, so after function call you have compeat dRect.
 * 
 * @param td TextureData to be rendered on the screen
 * @param dr SDL_Rect specifing where and how big should Texture be rendered
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TM::renderTexture(const TextureData& td, SDL_Rect& dr) {
    // CHECK IF TEXTURE IS VALID ---------------------------------------------------------
    if(td.tex == nullptr) return TM_GOT_NULLPTR_TEX;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(dr.w < 0 && dr.h < 0) return TM_INVALID_DRECT;


    // CALCULATE IF ONE DIMENSION IS MISSING ----------------------------------------------
    if(dr.w == -1) dr.w = dr.h * (float)td.width / td.height;
    if(dr.h == -1) dr.h = dr.w * (float)td.height / td.width;


    // RENDER THE TEXTURE ON SCREEN -------------------------------------------------------
    if(SDL_RenderCopy(Sys::renderer, td.tex, NULL, &dr)) return TM_RCPY_FAILED;


    return NO_ERROR;
}





/** Create Text Texture
 * 
 * Creates a texture from a text using TTF_RenderUTF8_Blended.
 * It saves the texture into td TextureData and fills the rest of the object data.
 * 
 * @param td TextureData object where texture and side data will be placed
 * @param text The text to be compiled into texture
 * @param colo SDL_Color object, representing the color of the text
 * @return 0 on success and positive error code on error
 */
int TM::createTextTexture(
    TextureData& td, 
    const string& text,
    const int& fontSize,
    const SDL_Color& color
){
    if(Sys::fontPath == "") {
        CHECK_ERROR(SYS_FONT_NOT_INITED);
        exit(EXIT_FAILURE);
    }

    // Ensure that privous Texture is de-loaded -----------------------------------------
    if(td.tex != nullptr) TM::freeTexture(td);

    // Get the font with that particular fontSize
    TTF_Font* font = Sys::getFont(fontSize);

    // Create the text texture and store it as surface ----------------------------------
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if(surface == nullptr) return TM_SURFACE_CREATE_ERROR;

    // Create texture from it and store it in TextureData
    td.tex = SDL_CreateTextureFromSurface(Sys::renderer, surface);
    if(td.tex == nullptr) {
        SDL_FreeSurface(surface);
        return TM_TEXTURE_CREATE_ERROR;
    }

    // MAKE TEXTURE UPDATATBLE/MODIFIABLE -------------------------------------------------
    if(SDL_SetTextureBlendMode(td.tex, SDL_BLENDMODE_BLEND)){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_SET_BLENDMODE_ERROR;
    }

    td.path = "TEXT";
    td.id = "TEXT-" + text;

    // GET TEXTURE DIMENSIONS -------------------------------------------------------------
    SDL_QueryTexture(td.tex, &td.format, NULL, &td.width, &td.height);
    td.orgWidth = td.width;
    td.orgHeight = td.height;

    SDL_FreeSurface(surface);
    loadedTextures.insert({td.tex, td.id});

    return NO_ERROR;
}





/** COPY
 * 
 * Creates a new Texture, copy, of the src, and places
 * it into dst TextureData. This function frees the old
 * dst Texture and creates a new one same as src.
 * 
 * If you just want to re-draw the src ontop of the dst
 * without creating a whole new texture from the start
 * you can use src.drawOverlayTexture() wich does exacly that.
 * 
 * @param src The source TextureData object, original
 * @param dst The destinaion object, the copy
 * @return 0 on success and positive error code on error
 */
int TM::copy(const TextureData& src, TextureData& dst){
    int err;
    if(dst.tex != nullptr){
        TM::freeTexture(dst);
    }

    // Create a new texture with the same format and size as the original
    dst.tex = SDL_CreateTexture(
        Sys::renderer, 
        src.format, 
        SDL_TEXTUREACCESS_TARGET, 
        src.width, 
        src.height
    );
    if (dst.tex == nullptr) {
        return TM_TEXTURE_CREATE_ERROR;
    }  

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);
    
    // Set the render target to the new texture to copy from the source texture
    err = SDL_SetRenderTarget(Sys::renderer, dst.tex);
    if(err) return TM_SRT_FAILED;

    // Clear the target texture
    err = SDL_RenderClear(Sys::renderer);
    if(err) return TM_RCLR_FAILED;

    // Copy the content of the source texture to the new one
    err = SDL_RenderCopy(Sys::renderer, src.tex, NULL, NULL);
    if(err) return TM_RCPY_FAILED;

    // Reset the render target to the default (the screen)
    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err) return TM_SRT_FAILED;

    dst.width = src.width;
    dst.height = src.height;
    dst.format = src.format;
    dst.orgWidth = src.orgWidth;
    dst.orgHeight = src.orgHeight;
    dst.path = src.path;
    dst.id = dst.id + "(copy)";

    loadedTextures.insert({dst.tex, dst.id});

    return NO_ERROR;
};





int TM::resize(TextureData& td, int targetWidth, int targetHeight){
    if(targetWidth == -1 && targetHeight == -1) return TM_INVALID_DRECT;

    if(targetWidth == -1) targetWidth = static_cast<int>(td.width * (static_cast<float>(targetHeight) / td.height));
    if(targetHeight == -1) targetHeight = static_cast<int>(td.height * (static_cast<float>(targetWidth) / td.width));

    SDL_Texture* resizedTexture = SDL_CreateTexture(
        Sys::renderer,
        td.format, // Use a standard pixel format
        SDL_TEXTUREACCESS_TARGET, // Texture will be used as a render target
        targetWidth, 
        targetHeight
    );

    if(!resizedTexture) return TM_TEXTURE_CREATE_ERROR;

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);
    
    int err = SDL_SetRenderTarget(Sys::renderer, resizedTexture);
    if(err != 0) return TM_SRT_FAILED;

    SDL_Rect srcRect = { 0, 0, td.width, td.height };
    SDL_Rect dstRect = { 0, 0, targetWidth, targetHeight };
    err = SDL_RenderCopy(Sys::renderer, td.tex, &srcRect, &dstRect);
    if(err != 0) return TM_RCPY_FAILED;


    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err != 0) return TM_SRT_FAILED;

    if(SDL_SetTextureBlendMode(resizedTexture, SDL_BLENDMODE_BLEND)){
        SDL_DestroyTexture(resizedTexture);
        return TM_TEXTURE_SET_BLENDMODE_ERROR;
    }

    SDL_DestroyTexture(td.tex);

    td.tex = resizedTexture;
    td.width = targetWidth;
    td.height = targetHeight;

    return NO_ERROR;
}




/** Draw Overlay Texture
 * 
 * Draws a texture on top of this texture.
 * Just like the TM::renderTexture it is able to dinamicly calculate one
 * missing dimension of the dRect, {10, 10, 100, -1}, and it calculates the
 * missing height and it updates it and then renderes the texture
 * 
 * @param td TextureData object to be rendered on top
 * @param dr SDL_Rect specifing where and how big should Texture be rendered
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayTexture(const TextureData& td, SDL_Rect& dr){
    int err;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(dr.w < 0 && dr.h < 0) return TM_INVALID_DRECT;

    // CALCULATE IF ONE DIMENSION IS MISSING ----------------------------------------------
    if(dr.w == -1) dr.w = dr.h * (float)td.width / td.height;
    if(dr.h == -1) dr.h = dr.w * (float)td.height / td.width;

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);

    // Set the render target to the new texture ----------------------------------------
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    // Copy the content of the source texture to the new one ---------------------------
    err = SDL_RenderCopy(Sys::renderer, td.tex, NULL, &dr);
    if(err != 0) return TM_RCPY_FAILED;

    // Reset the render target to the default (the screen) -----------------------------
    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}





/** Draw Overlay Texture
 * 
 * Draws a texture on top of this texture.
 * This drawOverlayTexture overload draws the 
 * texture asuming that this->td and param td
 * have at least the same aspect ratio and its
 * drawing from top-left croner to bottom-right.
 * 
 * @param td TextureData object to be rendered on top
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayTexture(const TextureData& td){
    SDL_Rect dRect = {0, 0, this->width, this->height};
    return drawOverlayTexture(td, dRect);
}





/** Draw Overlay Filled Rect
 * 
 * Draws a filled rect on top of this texture.
 * 
 * @param rect SDL_Rect specifing where and how big should rect be
 * @param color SDL_Color representing what color should rect be
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayRect(const SDL_Rect& rect, const SDL_Color& color, const int& thickness){
    int err;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(rect.w < 0 && rect.h < 0) return TM_INVALID_DRECT;

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);

    // Set the texture as the target for drawing
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    GUI::Rect(rect, color, thickness);

    // // Set the color for use by renderer
    // err = SDL_SetRenderDrawColor(Sys::renderer, color);
    // if(err != 0) return TM_SRDC_FAILED;

    // // Draw Filled Rect
    // err = SDL_RenderFillRect(Sys::renderer, &rect);
    // if(err != 0) return TM_FILL_RECT_ERROR;

    // Reset the render target back to window
    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}





/** Draw Overlay Line
 * 
 * Draws a line with custom thickness on top of this texture.
 * 
 * @param p1 SDL_Point representing coordinates for starting point of the line
 * @param p2 SDL_Point representing coordinates for ending point of the line
 * @param color SDL_Color color of the line
 * @param thickness Thickness of the line
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color, const int& thickness){
    int err;

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);

    // Set the texture as the target for drawing
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    // Set the color for use by renderer
    err = SDL_SetRenderDrawColor(Sys::renderer, color);
    if(err != 0) return TM_SRDC_FAILED;

    // DRAWING LINE ------------------------------------------

    // Calculate the line's direction vector
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double length = std::sqrt(dx * dx + dy * dy);

    if(length <= 0) return TM_INVALID_LINE_LENGTH;

    // Normalize the direction vector
    dx /= length;
    dy /= length;

    // Calculate the perpendicular vector for thickness
    double px = -dy * (thickness / 2.0);
    double py = dx * (thickness / 2.0);

    // Define the four corners of the rectangle representing the thick line
    SDL_Vertex vertices[4] = {
        {SDL_FPoint{static_cast<float>(p1.x + px), static_cast<float>(p1.y + py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p1.x - px), static_cast<float>(p1.y - py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p2.x - px), static_cast<float>(p2.y - py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p2.x + px), static_cast<float>(p2.y + py)}, color, SDL_FPoint{0, 0}}
    };


    // Draw the thick line as a filled polygon
    SDL_RenderGeometry(Sys::renderer, nullptr, vertices, 4, nullptr, 0);
    
    // Reset the render target back to window
    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}





int TextureData::drawOverlayText(const string& text, SDL_Rect& dRect, const SDL_Color& color){
    int err;

    SDL_Texture* previousRenderTarget = SDL_GetRenderTarget(Sys::r);

    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    GUI::TextBox(text, dRect, color);

    err = SDL_SetRenderTarget(Sys::renderer, previousRenderTarget);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}


bool TM::saveSurfaceToFile(SDL_Surface* surface, const string& filename) {
    if (!surface) {
        std::cerr << "Error: Surface is null." << std::endl;
        return false;
    }

    if (IMG_SavePNG(surface, filename.c_str()) != 0) {
        std::cerr << "IMG_SavePNG error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}


/**
 * cv::Mat must be RGBA format.
 */
int TM::cvMat2Texture(const cv::Mat& mat, TextureData& td) {
    if (td.tex != nullptr) TM::freeTexture(td);

    if(mat.channels() != 4) return TM_MAT_INVALID_FORMAT;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        (void*)mat.data,
        mat.cols,
        mat.rows,
        32,
        static_cast<int>(mat.step[0]),
        0x000000ff,
        0x0000ff00,
        0x00ff0000,
        0xff000000
    );
    td.tex = SDL_CreateTextureFromSurface(Sys::renderer, surface);

    // Free the temporary surface.
    SDL_FreeSurface(surface);

    // Retrieve texture dimensions.
    SDL_QueryTexture(td.tex, &td.format, NULL, &td.width, &td.height);
    td.orgWidth = td.width;
    td.orgHeight = td.height;

    // Store the texture pointer in your global container.
    loadedTextures.insert({td.tex, td.id});
    return NO_ERROR;
}





void TM::exportTexture(SDL_Texture* texture, string path){
    if(texture == nullptr) return;

    SDL_Texture* target = SDL_GetRenderTarget(Sys::renderer);
    SDL_SetRenderTarget(Sys::renderer, texture);

    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    SDL_RenderReadPixels(Sys::r, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, path.c_str());
    
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(Sys::renderer, target);
}

void TM::exportTexture(const TextureData& td, string path){ exportTexture(td.tex, path); }


