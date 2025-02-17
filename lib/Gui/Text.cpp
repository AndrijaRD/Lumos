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

    LoadedText* textPointer = nullptr;
    string id = title + color2hex(color);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LoadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->frame = Sys::getCurrentFrame();
    } else {
        // Create new Text Texture
        textPointer = loadNewText(title, color);
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

    // Create the texture
    TextureData td;
    int err = TM::createTextTexture(td, title, color);
    CHECK_ERROR(err);

    // Render the texture
    TM::renderTexture(td, dRect);

    // Free Texture Data
    TM::freeTexture(td); 
}


void GUI::clearLoadedTexts(){
    for(auto text : loadedTexts){
        TM::freeTexture(text.second.td);
    }
    loadedTexts.clear();
}


/** Load New Text
 * 
 * INTERNAL FUNCTION
 * 
 * Funtion for inserting a new text texture into map.
 * It also calles removeOldest if map is full.
 */
GUI::LoadedText* GUI::loadNewText(const string& title, const SDL_Color& color){
    // If there is more loaded Textures then allowed, remove the oldest
    if((int)loadedTexts.size() >= max_num_of_loaded_textures){
        removeOldest();
    }

    // Create the new LoadedText item
    LoadedText newText;
    newText.color = color;
    newText.frame = Sys::getCurrentFrame();
    newText.title = title;

    // Create the texture
    int err = TM::createTextTexture(newText.td, newText.title, newText.color);
    CHECK_ERROR(err);

    // Insert the item and return the pointer to it
    loadedTexts.insert({newText.getId(), newText});
    return &(loadedTexts.at(newText.getId()));
}


