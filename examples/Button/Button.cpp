#include "../../lib/Lumos.h"


int main(){
    int error;
    error = Sys::initWindow("Button Example");
    if(error != NO_ERROR) exit(EXIT_FAILURE);

    error = Sys::initFont("../../assets/fonts/font.ttf");
    if(error != NO_ERROR) exit(EXIT_FAILURE);

    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    int clicks = 0;
    while(Sys::isRunning){
        Sys::handleEvents();


        // App Code and Logic...

        // Firstly we will render a button
        // For args it takes title, dRect, text color and backgroud color
        // *Text and background colors can be left empty to use the default
        GUI::Button(
            "I am also a Button",   // Button text
            {20, 20, 200, 40},      // Button pos and size
            SDL_COLOR_RED,           // Button color
            SDL_COLOR_GREEN        // Text color
        );
        

        // And we can also return the state of the button
        string text = "Click count: " + to_string(clicks);
        int state = GUI::Button(text, {20, 80, 300, 40}, SDL_COLOR_BLACK, SDL_COLOR_WHITE);

        // If user has clicked on the button
        if(state == GUI_CURSOR_CLICKED) clicks++;

        // If the users cursor is hovering the button
        if(state == GUI_CURSOR_HOVERING) cout << "You are hovering a button" << endl;

        // If the mouse is down on the button
        if(state == GUI_CURSOR_DRAGGING) cout << "You are holding the button down" << endl;

        // If the cursor is outside of the button 
        if(state == GUI_CURSOR_OUTSIDE) cout << "Your cursor is outside the button" << endl;

        // If users clicks outside of the button, unfocus
        if(
            state == GUI_CURSOR_OUTSIDE && 
            Sys::Mouse::isClicked()
        ){
            cout << "You have clicked outside of the button. Reseting the click count." << endl;
            clicks = 0;
        }


        // STYLES --------------------------------------------------------
        GUI::pushFontSize(26);
        GUI::pushTextAlignX(GUI_ALIGN_RIGHT);
        GUI::pushTextAlignY(GUI_ALIGN_BOTTOM);
        GUI::Button("Custom Styles", {20, 140, 220, 50});


        Sys::presentFrame();
    }

    TM::cleanup();
    return Sys::cleanup();
}