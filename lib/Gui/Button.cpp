#include "gui.h"
#include "../System/Sys.h"



/** GUI Button
 * 
 * This function renders a button. It supports all style pushs.
 * 
 * @param title Button text, which is dinamicly calculated to fit into the button and to be centered
 * @param dRect It represents position and size of the button, must be possitive, all of the value
 * @param buttonColor Background color of the button
 * @param textColor Color of the text inside the Button
 * 
 * @returns It returns the state of the button: 
 *  `GUI_CURSOR_CLICKED`, 
 *  `GUI_CURSOR_HOVERING`,
 *  `GUI_CUROSR_DRAGGING`,
 *  `GUI_CURSOR_OUTSIDE`
 * 
 */
int GUI::Button(
    const string& title, 
    const SDL_Rect& dRect,
    const SDL_Color& buttonColor,
    const SDL_Color& textColor
){
    // COPY STYLES -------------------------------------------------------
    // First we copy the pushed styles
    int fontSize = GUI::pFontSize;
    GUI::pFontSize = -1;

    int textAlignX = GUI::pTextAlignX;
    if(textAlignX == -1) textAlignX = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignX = -1;

    int textAlignY = GUI::pTextAlignY;
    if(textAlignY == -1) textAlignY = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignY = -1;

    BorderRadiusRect borderRadius = pBorderRadius;
    pBorderRadius = {0, 0, 0, 0};



    // CHECK VALIDITY OF dRect -------------------------------------------
    if(dRect.w < 1 || dRect.h < 1) return GUI_CURSOR_OUTSIDE;



    // FIND BUTTON TEXTURE
    LoadedText* textPointer = nullptr;
    string id = title + color2hex(textColor);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LOadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->frame = Sys::getCurrentFrame();
    } else {
        // Create new Text Texture
        textPointer = loadNewText(title, textColor);
    }



    // BUTTON BACK -------------------------------------------------------
    // Render the buttons background, filled rect, 
    // and then on top of it will be white text
    //SDL_SetRenderDrawColor(Sys::renderer, buttonColor);
    //SDL_RenderFillRect(Sys::renderer, &dRect);
    GUI::pushBorderRadius(borderRadius);
    GUI::Rect(dRect, buttonColor);
    


    // Add a 5px padding on the sides, unless the total 
    // width or height of the button is less then 10x10
    int padding = 5;
    if(dRect.w < 10 || dRect.h < 10) padding = 0;

    // Calculate what is the max width and height of 
    // the text that can fit in the button without overflow
    int maxTextWidth = dRect.w - padding*2;
    int maxTextHeight = dRect.h - padding*2;

    // TEXT DRECT --------------------------------------------------------
    SDL_Rect text_dRect;

    if(fontSize == -1){
        // if required width is less then maximal allowed then
        // the height is the limiting factor, so its max height

        // We calc the text width in case of max height and compare it to max allowed width
        int textWidth = textPointer->td.width * (float)maxTextHeight / textPointer->td.height;
        if(textWidth <= maxTextWidth){
            // This means that text is maximal height, 
            // and width is less then max, there will be
            // left-right free space
            text_dRect.h = maxTextHeight;
        } else {
            // This means that text is maximal width, 
            // and height is less then max, there will be
            // top-bottom free space
            textWidth = maxTextWidth;
            text_dRect.h = textPointer->td.height * (float)maxTextWidth / textPointer->td.width;
        }
    } else{
        text_dRect.h = fontSize;
    }


    // DRECT WIDTH -----------------------------------------------------------------
    text_dRect.w = text_dRect.h * (float)textPointer->td.width / textPointer->td.height;

    // DRECT X POS
    if(textAlignX == GUI_ALIGN_LEFT){
        text_dRect.x = dRect.x + padding;
    } else if(textAlignX == GUI_ALIGN_CENTER){
        text_dRect.x = dRect.x + dRect.w/2 - text_dRect.w/2;
    } else if(textAlignX == GUI_ALIGN_RIGHT){
        text_dRect.x = dRect.x + dRect.w - padding - text_dRect.w;
    }

    // DRECT Y POS
    if(textAlignY == GUI_ALIGN_TOP){
        text_dRect.y = dRect.y + padding;
    } else if(textAlignY == GUI_ALIGN_CENTER) {
        text_dRect.y = dRect.y + dRect.h/2 - text_dRect.h/2;
    } else if(textAlignY == GUI_ALIGN_BOTTOM) {
        text_dRect.y = dRect.y + dRect.h - padding - text_dRect.h;
    }


    // Now render the text
    int err = TM::renderTexture(textPointer->td, text_dRect);
    CHECK_ERROR(err);

    if(Sys::Mouse::isHovering(dRect)){
        if(Sys::Mouse::isClicked()){
            return GUI_CURSOR_CLICKED;
        }
        if(Sys::Mouse::isDown()){
            return GUI_CURSOR_DRAGGING;
        }
        return GUI_CURSOR_HOVERING;
    }

    return GUI_CURSOR_OUTSIDE;
}




