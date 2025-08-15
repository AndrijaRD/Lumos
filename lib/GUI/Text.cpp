#include "gui.h"
#include "../System/Sys.h"


void GUI::Text(
    const string& title, 
    SDL_Rect& dRect, 
    const SDL_Color& color
) {
    if(dRect.w < 1 && dRect.h < 1) return;

    if(dRect.h == -1) dRect.h = calcTextHeight(title, dRect.w);
    if(dRect.w == -1) dRect.w = calcTextWidth(title, dRect.h);

    LoadedText* textPointer = nullptr;
    string id = getTextId(title, dRect.h, color);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LoadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->lastUsedFrame = Sys::getCurrentFrame();
    } else {
        textPointer = loadNewText(title, dRect.h, color);
    }

    // Render the texture
    GUI::Image(textPointer->td, dRect);
}


void GUI::TextDynamic(
    const string& title, 
    SDL_Rect& dRect, 
    const SDL_Color& color
) {
    if(dRect.w < 1 && dRect.h < 1) return;

    if(dRect.h == -1) dRect.h = calcTextHeight(title, dRect.w);
    if(dRect.w == -1) dRect.w = calcTextWidth(title, dRect.h);

    // Create the texture
    TextureData td;
    int err = TM::createTextTexture(td, title, dRect.h, color);
    CHECK_ERROR(err);

    // Render the texture
    GUI::Image(td, dRect);
}


int GUI::calcTextWidth(
    const string& text,
    int textHeight
) {
    // Ensure that the font is valid.
    if (Sys::fontPath == "") {
        Sys::printf_err(SYS_FONT_NOT_INITED);
        exit(EXIT_FAILURE);
    }

    TTF_Font* font = Sys::getFont(textHeight);

    // Measure the text using the currently active font.
    int width = 0, height = 0;
    TTF_GetStringSize(
        font, 
        text.c_str(), 
        text.size(),
        &width,
        &height
    );

    int currentFontHeight = TTF_GetFontHeight(font);
    float scale = static_cast<float>(textHeight) / currentFontHeight;
    int textWidth = static_cast<int>(width * scale);

    // Return the scaled width.
    return textWidth;
}


int GUI::calcTextHeight(
    const string& text,
    int textWidth
) {
    // Ensure that the font is valid.
    if (Sys::fontPath == "") {
        Sys::printf_err(SYS_FONT_NOT_INITED);
        exit(EXIT_FAILURE);
    }

    TTF_Font* font = Sys::getFont(64);

    // Measure the text's dimensions at font size 64.
    int width = 0, height = 0;
    TTF_GetStringSize(
        font, 
        text.c_str(), 
        text.size(),
        &width,
        &height
    );
    
    // Calculate the scale factor based on the desired text width.
    // For example, if measuredWidth is 200px and textWidth is 100px, the scale factor is 0.5.
    float scaleFactor = static_cast<float>(textWidth) / width;
    
    // The new height is the measured height scaled accordingly.
    int requiredHeight = static_cast<int>(height * scaleFactor);
    return requiredHeight;
}


string GUI::getTextId(
    const string& title, 
    int fontSize, 
    SDL_Color color
) {
    // id = "HelloWorld_24#ffffffff"
    // id = text_fontSize#color
    return title + "_" + to_string(fontSize) + sdlColor2hex(color);
}


GUI::LoadedText* GUI::loadNewText(
    string title,
    int fontSize,
    SDL_Color color
) {
    // If there are more elements then allowed remove the oldest
    if((int)loadedTexts.size() >= MAX_LOADED_TEXTS && MAX_LOADED_TEXTS > 0){
        removeOldestText();
    }

    // FIll the data
    LoadedText newText;
    newText.title = title;
    newText.fontSize = fontSize;
    newText.color = color;
    newText.lastUsedFrame = Sys::getCurrentFrame();

    // Create the texture
    int err = TM::createTextTexture(
        newText.td, 
        newText.title, 
        newText.fontSize, 
        newText.color
    );
    CHECK_ERROR(err);

    // Insert the item
    loadedTexts.insert({newText.getId(), newText});

    // Return the pointer to the item
    return &(loadedTexts.at(newText.getId()));
}


void GUI::removeOldestText(){
    if (loadedTexts.empty()) return;

    // Find the map‐element whose LoadedText::lastUsedFrame is smallest
    auto it = std::min_element(
        loadedTexts.begin(), 
        loadedTexts.end(),
        [](auto const &a, auto const &b) {
            return a.second.lastUsedFrame < b.second.lastUsedFrame;
        }
    );

    // Erase it
    if (it != loadedTexts.end()) {
        loadedTexts.erase(it);
    }
}


