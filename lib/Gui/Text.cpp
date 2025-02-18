#include "gui.h"
#include "../System/Sys.h"



/** Text
 * 
 * Draws a optimized verion of text on the screen.
 * It doesnt respond to any push styles since all of them
 * can be controlled trough dRect param.
 * 
 * @param title Text to be rendered.
 * @param dRect position and size of the text, one dimension can be left as -1 for
 *              dynamic calculation, eg. set height to font size and width as -1
 *              and after the render you can check dRect that was passed and the
 *              width will be calculated and dRect will be updated, replacing -1
 * @param color The color of the text
 * 
 */
void GUI::TextBox(const string& title, SDL_Rect& dRect, const SDL_Color& color){
    if(dRect.w < 1 && dRect.h < 1) return;

    if(dRect.h == -1) dRect.h = calcTextHeight(title, dRect.w);
    if(dRect.w == -1) dRect.w = calcTextWidth(title, dRect.h);

    LoadedText* textPointer = nullptr;
    string id = getTextID(title, dRect.h, color);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LoadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->frame = Sys::getCurrentFrame();
    } else {
        textPointer = loadNewText(title, dRect.h, color);
    }

    // Render the texture
    TM::renderTexture(textPointer->td, dRect);
}




/** Text Dynamic
 * 
 * Draws a text that is not cached anywhere, the texture is created,
 * rendered and then destroyed, its not optimal, but it shold be used for 
 * highly changable texts, like the ones that change every frame so there
 * is no point is storing them as they will never be reused unline regular
 * GUI::Text which is optimised so it stores all texture in case of re-use.
 * 
 * @param title Text to be rendered.
 * @param dRect position and size of the text, one dimension can be left as -1 for
 *              dynamic calculation, eg. set height to font size and width as -1
 *              and after the render you can check dRect that was passed and the
 *              width will be calculated and dRect will be updated, replacing -1
 * @param color The color of the text
 * 
 */
void GUI::TextDynamic(const string& title, SDL_Rect& dRect, const SDL_Color& color){
    if(dRect.w < 1 && dRect.h < 1) return;

    if(dRect.h == -1) dRect.h = calcTextHeight(title, dRect.w);
    if(dRect.w == -1) dRect.w = calcTextWidth(title, dRect.h);

    // Create the texture
    TextureData td;
    int err = TM::createTextTexture(td, title, dRect.h, color);
    CHECK_ERROR(err);

    // Render the texture
    TM::renderTexture(td, dRect);

    // Free Texture Data
    TM::freeTexture(td); 
}


int GUI::calcTextWidth(
    const string& text,
    const int& textHeight
){
    // Ensure that the font is valid.
    if (Sys::fontPath == "") {
        CHECK_ERROR(SYS_FONT_NOT_INITED);
        exit(EXIT_FAILURE);
    }

    TTF_Font* font = Sys::getFont(textHeight);

    // Measure the text using the currently active font.
    int width = 0, height = 0;
    TTF_SizeText(font, text.c_str(), &width, &height);

    int currentFontHeight = TTF_FontHeight(font);
    float scale = static_cast<float>(textHeight) / currentFontHeight;
    int textWidth = static_cast<int>(width * scale);

    // Return the scaled width.
    return textWidth;
}


int GUI::calcTextHeight(
    const string& text,
    const int& textWidth
){
    // Ensure that the font is valid.
    if (Sys::fontPath == "") {
        CHECK_ERROR(SYS_FONT_NOT_INITED);
        exit(EXIT_FAILURE);
    }

    TTF_Font* font = Sys::getFont(64);

    // Measure the text's dimensions at font size 64.
    int measuredWidth = 0, measuredHeight = 0;
    if (TTF_SizeText(font, text.c_str(), &measuredWidth, &measuredHeight) != 0) {
        SDL_Log("TTF_SizeText error: %s", TTF_GetError());
        return 0;
    }
    
    // Calculate the scale factor based on the desired text width.
    // For example, if measuredWidth is 200px and textWidth is 100px, the scale factor is 0.5.
    float scaleFactor = static_cast<float>(textWidth) / measuredWidth;
    
    // The new height is the measured height scaled accordingly.
    int requiredHeight = static_cast<int>(measuredHeight * scaleFactor);
    return requiredHeight;
}



void GUI::clearLoadedTexts(){
    for(auto text : loadedTexts){
        TM::freeTexture(text.second.td);
    }
    loadedTexts.clear();
}

string GUI::getTextID(string title, int fontSize, SDL_Color color){
    // id = "HelloWorld_24#ffffffff"
    // id = text_fontSize#color
    return title + "_" + to_string(fontSize) + color2hex(color);
}


/** Load New Text
 * 
 * INTERNAL FUNCTION
 * 
 * Funtion for inserting a new text texture into map.
 * It also calles removeOldest if map is full.
 */
GUI::LoadedText* GUI::loadNewText(
    const string& title, 
    const int& fontSize, 
    const SDL_Color& color
){
    // If there is more loaded Textures then allowed, remove the oldest
    if((int)loadedTexts.size() >= max_num_of_loaded_textures){
        removeOldest();
    }

    // Create the new LoadedText item
    LoadedText newText;
    newText.color = color;
    newText.fontSize = fontSize;
    newText.frame = Sys::getCurrentFrame();
    newText.title = title;

    // Create the texture
    int err = TM::createTextTexture(newText.td, newText.title, fontSize, newText.color);
    CHECK_ERROR(err);

    // Insert the item and return the pointer to it
    loadedTexts.insert({newText.getId(), newText});
    return &(loadedTexts.at(newText.getId()));
}


