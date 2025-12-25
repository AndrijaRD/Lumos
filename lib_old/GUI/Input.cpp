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

    Align textAlignX = GUI::pTextAlignX;
    if(textAlignX == Align::DEFAULT) textAlignX = Align::LEFT; // Default
    GUI::pTextAlignX = Align::DEFAULT;

    Align textAlignY = GUI::pTextAlignY;
    if(textAlignY == Align::DEFAULT) textAlignY = Align::CENTER; // Default
    GUI::pTextAlignY = Align::DEFAULT;

    bool autoFocus = GUI::pAutoFocus;
    GUI::pAutoFocus = false;

    bool inputLock = GUI::pInputLock;
    GUI::pInputLock = false;

    string defaultValue = GUI::pDefaultValue;
    GUI::pDefaultValue = "";

    BorderRadiusRect borderRadius = pBorderRadius;
    pBorderRadius = {0, 0, 0, 0};

    int outlineThickness = pOutlineThickness;
    pOutlineThickness = -1;

    // if outlineThickness is -1 then that means outline thickness
    // was not set, not pushed, so Input should use default thickness
    if(outlineThickness == -1) outlineThickness = 1;

    SDL_Color outlineColor = pOutlineColor;
    pOutlineColor = {0, 0, 0, 0};

    // if color.a is 0, then that means that outline color was not pushed
    // its default value, so default for input outline is black
    if(outlineColor.a == 0) outlineColor = SDL_COLOR_BLACK;

    PaddingRect paddingRect = pPaddingRect;
    pPaddingRect = {-1, -1, -1, -1};

    // If paddingRect is filled with -1 then those values should be
    // replaced by default values for input element
    if(paddingRect.top == -1) paddingRect.top = 3;
    if(paddingRect.right == -1) paddingRect.right = 5;
    if(paddingRect.bottom == -1) paddingRect.bottom = 3;
    if(paddingRect.left == -1) paddingRect.left = 5;


    // FIND STATE --------------------------------------------------------
    // Get the pointer to the inputs state using uniqueId
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
        if(state->focused) Keyboard::unfocus();
        state->focused = false;
    }


    if(autoFocus && state->firstRender){
        Keyboard::focus();
        state->focused = true;
    }

    if(state->firstRender && defaultValue != ""){
        state->value = defaultValue;
    }


    // DRAW BACKGROUND ---------------------------------------------------
    GUI::pushBorderRadius(borderRadius);
    GUI::Rect(dRect, background);           // Background field

    if(outlineThickness != 0){
        GUI::pushBorderRadius(borderRadius);
        GUI::Rect(dRect, outlineColor, outlineThickness);   // 1px black Outline
    }
    





    // TEXT RECT CALCULATION ---------------------------------------------
    SDL_Rect textRect; // Calculate the position and size of the text

    // TEXT RECT HEIGHT --------------------------------------------------
    // If there is no fontSize speicified make the font max height
    if(fontSize == -1){
        textRect.h = dRect.h - paddingRect.top - paddingRect.bottom;
    } else {
        textRect.h = fontSize;
    }

    // TEXT RECT WIDTH ---------------------------------------------------
    textRect.w = textRect.h * (float)state->td.getWidth() / state->td.getHeight();

    // TEXT RECT X POS ---------------------------------------------------
    // Depending on the placement, left, center, right. Default: left
    if(textAlignX == Align::LEFT){
        textRect.x = dRect.x + paddingRect.left;
    } else if(textAlignX == Align::RIGHT){
        textRect.x = dRect.x + dRect.w - paddingRect.right - textRect.w;
    } else if(textAlignX == Align::CENTER){
        textRect.x = dRect.x + dRect.w/2 - textRect.w/2;
    }

    // TEXT RECT Y POS ---------------------------------------------------
    if(textAlignY == Align::BOTTOM){
        textRect.y = dRect.y + dRect.h - paddingRect.bottom - textRect.h;
    } else if(textAlignY == Align::CENTER){
        textRect.y = dRect.y + dRect.h/2 - textRect.h/2;
    } else if(textAlignY == Align::TOP){
        textRect.y = dRect.y + paddingRect.top;
    }
    


    // Render the text
    GUI::Image(state->td, textRect);

    if(inputLock) GUI::Rect(dRect, {120, 120, 120, 120});

    // Check if placeholder was rendered
    bool placeholderActive = false;
    if(state->value == "") placeholderActive = true;

    // If the rendered text is larger then the allowed width
    // and we are also not talking about the placeholder THEN
    // make the removed variable larger by one, signaling to the
    // next frame that it should cut out one more frame from the left
    if(textRect.w > (dRect.w - paddingRect.left - paddingRect.right) && !placeholderActive){
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
            SDL_Point p1 = {0, dRect.y + paddingRect.top};
            
            // If the placeholder is not active then draw the 
            // line at the end of the text.
            // Else, the placeholder is active, then draw the line
            // just few pixels behind the placeholder, so its vissible
            if(placeholderActive){
                if(textAlignX == Align::LEFT){
                    p1.x = dRect.x + paddingRect.left;
                } else if(textAlignX == Align::CENTER){
                    p1.x = dRect.x + dRect.w/2;
                } else if(textAlignX == Align::RIGHT){
                    p1.x = dRect.x + dRect.w - paddingRect.right;
                }
            } else {
                p1.x = textRect.x + textRect.w + 1;
            }

            // Bottom point of the line
            SDL_Point p2 = {
                p1.x, 
                p1.y + (dRect.h - paddingRect.top - paddingRect.bottom)
            };
            
            // Draw the line
            GUI::Line(p1, p2, foreground);
        }
    }
    

    // EVENTS ---------------------------------
    // If the user is hovering the input filed
    // and he clicks then focus the input onto the filed
    if(
        Mouse::isHovering(dRect) &&
        Mouse::wasReleasedLeft()
    ) {
        Keyboard::focus();
        state->focused = true;
    }

    // If user is not hovering the button, cursor is outside
    // and there is click, that means that there has been unfocus
    if(
        !Mouse::isHovering(dRect) &&
        Mouse::wasReleasedLeft() &&
        !state->firstRender
    ) {
        Keyboard::unfocus();
        state->focused = false;
    }

    // If Sys::Keyboard::unfocus() was runned but the input still wants the focus
    if(state->focused && !Keyboard::isFocused()) Keyboard::focus(); 

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
        if(Keyboard::getText() != "") {
            state->change = true;
            state->value += Keyboard::getText();
        }

        // If the backspace has been pressed handle deletion
        if(Keyboard::isDown(SDL_SCANCODE_BACKSPACE)){
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
        state->td.setTexture(nullptr);
        
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

        int err = TM::createTextTexture(state->td, textToCompile, textRect.h, colorOfText);
        CHECK_ERROR(err);
         
        state->change = false;
    }

    if(state->firstRender) state->firstRender = false;

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

    state->td.setTexture(nullptr);
    Keyboard::unfocus();
    state->focused = false;
    state->value.clear();
    inputStates.erase(it);
    return;
}

GUI::InputState* GUI::getInputState(const string& inputId){
    auto it = inputStates.find(inputId);
    if(it != inputStates.end())
        return &it->second;

    return nullptr;
}



