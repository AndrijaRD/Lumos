#include "gui.h"
#include "../System/Sys.h"




/** Input
 * 
 * Renders a Input field using a unique id.
 * 
 * The UniqueId should not be changes and needs to be
 * unique to each input field as it is used for accessing
 * the stored state of the field. Returing the value of
 * the filed, the user inputed text.
 * 
 * @param uniqueId A unique string intrenaly used to itentify the input field acrros diffrent frames
 * @param dRect A SDL_Rect representing where and what size should field be rendered
 * @param placeholder A text displayed on the input filed when input is empty
 * @param background SDL_Color representing the background field color
 * @param foreground SDL_Color representing the text color 
 * @return The value of the field
 */
string GUI::Input(
    const string& uniqueId,
    const SDL_Rect& dRect,
    const string& placeholder,
    const SDL_Color& background,
    const SDL_Color& foreground
){  
    /**
     * Improvements:
     *      - Char Position changer, like give the arrows some functionality
     *        left and right curosr movement, on each focus it should be set to
     *        max, the last char, but also make it changable using arrows and even
     *        with mouse click, advanced
     *        ! Problem: Each char should then be treated individualy as the program
     *                   must known where to draw the caret, where each char begins
     */


    // COPY STYLES -------------------------------------------------------
    // First we copy the pushed styles
    int fontSize = GUI::pFontSize;
    GUI::pFontSize = -1;

    int textAlignX = GUI::pTextAlignX;
    if(textAlignX == -1) textAlignX = GUI_ALIGN_LEFT; // Default
    GUI::pTextAlignX = -1;

    int textAlignY = GUI::pTextAlignY;
    if(textAlignY == -1) textAlignY = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignY = -1;

    bool autoFocus = GUI::pAutoFocus;
    GUI::pAutoFocus = false;

    bool inputLock = GUI::pInputLock;
    GUI::pInputLock = false;




    // FIND STATE --------------------------------------------------------
    // Get the pointer to the buttons state using uniqueId
    InputState* state = nullptr;
    auto it = inputStates.find(uniqueId);
    if(it != inputStates.end()){
        state = &it->second;
    } else{
        // If the state doesnt exist, create it
        inputStates.insert({uniqueId, InputState(uniqueId)});
        state = &inputStates.at(uniqueId);
    }

    if(inputLock){
        autoFocus = false;
        if(state->focused) Sys::Keyboard::unfocus();
        state->focused = false;
    }


    if(autoFocus){
        Sys::Keyboard::focus();
        state->focused = true;
    }


    // DRAW BACKGROUND ---------------------------------------------------
    GUI::Rect(dRect, background);           // Background field
    GUI::Rect(dRect, SDL_COLOR_BLACK, 1);   // 1px black Outline



    // DEFINE PADDING ----------------------------------------------------
    // This is the paddings for the text relative to the field
    int paddingX = 5;
    int paddingY = 3;



    // TEXT RECT CALCULATION ---------------------------------------------
    SDL_Rect textRect; // Calculate the position and size of the text

    // TEXT RECT HEIGHT --------------------------------------------------
    // If there is no fontSize speicified make the font max height
    if(fontSize == -1){
        textRect.h = dRect.h - paddingY*2;
    } else {
        textRect.h = fontSize;
    }

    // TEXT RECT WIDTH ---------------------------------------------------
    textRect.w = textRect.h * (float)state->td.width / state->td.height;

    // TEXT RECT X POS ---------------------------------------------------
    // Depending on the placement, left, center, right. Default: left
    if(textAlignX == GUI_ALIGN_LEFT){
        textRect.x = dRect.x + paddingX;
    } else if(textAlignX == GUI_ALIGN_RIGHT){
        textRect.x = dRect.x + dRect.w - paddingX - textRect.w;
    } else if(textAlignX == GUI_ALIGN_CENTER){
        textRect.x = dRect.x + dRect.w/2 - textRect.w/2;
    }

    // TEXT RECT Y POS ---------------------------------------------------
    if(textAlignY == GUI_ALIGN_BOTTOM){
        textRect.y = dRect.y + dRect.h - paddingY - textRect.h;
    } else if(textAlignY == GUI_ALIGN_CENTER){
        textRect.y = dRect.y + dRect.h/2 - textRect.h/2;
    } else if(textAlignY == GUI_ALIGN_TOP){
        textRect.y = dRect.y + paddingY;
    }
    

    // Render the text
    TM::renderTexture(state->td, textRect);

    if(inputLock) GUI::Rect(dRect, {120, 120, 120, 120});

    // Check if placeholder was rendered
    bool placeholderActive = false;
    if(state->value == "") placeholderActive = true;

    // If the rendered text is larger then the allowed width
    // and we are also not talking about the placeholder THEN
    // make the removed variable larger by one, signaling to the
    // next frame that it should cut out one more frame from the left
    if(textRect.w > (dRect.w - paddingX*2) && !placeholderActive){
        state->removed++;
        state->change = true;
    }



    // If the keyboard is focused on the field draw the caret, text cursor
    if(state->focused){
        // The cursor would be drawn for half a second and then be not drawn for 
        // another half a second, this is acheved trough clock which changes value
        // every (FPS / 2)frames so it can be devieded using %2 operator
        int blinkClock = static_cast<int>( Sys::getCurrentFrame() / (Sys::FPS / 2) );
        if(blinkClock % 2){
            // Top point of the line, default the
            // x is equal to the starting position of the text
            SDL_Point p1 = {0, dRect.y + paddingY};
            
            // If the placeholder is not active then draw the 
            // line at the end of the text.
            // Else, the placeholder is active, then draw the line
            // just few pixels behind the placeholder, so its vissible
            if(placeholderActive){
                if(textAlignX == GUI_ALIGN_LEFT){
                    p1.x = dRect.x + paddingX;
                } else if(textAlignX == GUI_ALIGN_CENTER){
                    p1.x = dRect.x + dRect.w/2;
                } else if(textAlignX == GUI_ALIGN_RIGHT){
                    p1.x = dRect.x + dRect.w - paddingX;
                }
            } else {
                p1.x = textRect.x + textRect.w + 1;
            }

            // Bottom point of the line
            SDL_Point p2 = {
                p1.x, 
                p1.y + (dRect.h - paddingY*2)
            };
            
            // Draw the line
            GUI::Line(p1, p2, SDL_COLOR_BLACK);
        }
    }
    

    // EVENTS ---------------------------------
    // If the user is hovering the input filed
    // and he clicks then focus the input onto the filed
    if(
        Sys::Mouse::isHovering(dRect) &&
        Sys::Mouse::isClicked()
    ) {
        Sys::Keyboard::focus();
        state->focused = true;
    }

    // If user is not hovering the button, cursor is outside
    // and there is click, that means that there has been unfocus
    if(
        !Sys::Mouse::isHovering(dRect) &&
        Sys::Mouse::isClicked()
    ){
        Sys::Keyboard::unfocus();
        state->focused = false;
    }

    // If Sys::Keyboard::unfocus() was runned but the input still wants the focus
    if(state->focused && !Sys::Keyboard::isFocused()) Sys::Keyboard::focus(); 

    // A small lambda funtion for handling deletion
    auto deleteLastChar = [&]() {
        // If value is already empty skip
        if (state->value.empty()) return;

        // Pop last char and mark change has been made
        state->value.pop_back();
        state->change = true;

        // If there was any chars to be removed, decrease the amount
        if (state->removed != 0) state->removed--;
    };

    // If the input is focused handle key presses
    if(state->focused){
        // Add the text inputed into the value
        if(Sys::Keyboard::getText() != "") {
            state->change = true;
            state->value += Sys::Keyboard::getText();
        }

        // If the backspace has been pressed handle deletion
        if(Sys::Keyboard::getKeyDown() == SDLK_BACKSPACE){
            // If the input is not in the state of deletion
            // that menas that this is the first frame that
            // backspace was pressed, so delete the last char
            // and mark that the hold of the button has begun
            if(!state->deleting){
                deleteLastChar();
                state->deleting = true;
            } else{
                // If the deletion has been going, and the
                // backspace is still down, holding, every
                // 200ms delete one last char from the field
                if(Sys::getCurrentFrame() % (Sys::FPS / 5)){
                    deleteLastChar();
                }
            }
        } else{
            // If the backspace is not pressed end the deletion process
            state->deleting = false;
        }
    }

    // Now handle the value changes and updating the texture
    if(state->change){
        TM::freeTexture(state->td);
        
        string textToCompile;
        SDL_Color colorOfText = foreground;
        if(state->value == ""){
            colorOfText.a *= 0.75;
            textToCompile = placeholder;
        } else {
            // So bc text can get longer then it can fit in the field
            // program also keeps track of how many characters it needs
            // to hide from the left side in order for text from the right
            // side to be vissible and with in the bounds
            textToCompile = state->value;
            textToCompile.erase(0, state->removed);
        }

        TM::createTextTexture(state->td, textToCompile, colorOfText);
        state->change = false;
    }

    // Return the value
    return state->value;
}





/** Destroy Input / Reset Input
 * 
 * This function  should be called if you need to 
 * restart the field or you are sure that its out 
 * of scope and its data is no longer important.
 * 
 * @param uniqueId Id of the input to be reset, destroyed.
 */
void GUI::DestroyInput(const string& uniqueId){
    InputState* state = nullptr;
    auto it = inputStates.find(uniqueId);
    if(it != inputStates.end()){
        state = &it->second;
    } else{
        return;
    }

    TM::freeTexture(state->td);
    Sys::Keyboard::unfocus();
    state->focused = false;
    state->value.clear();
    inputStates.erase(it);
    return;
}



