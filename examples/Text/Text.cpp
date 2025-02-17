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

        // Text Element renders text on the screen
        // It takes 3 args, one is optional
        //  - Title
        //  - Position and size
        //  - Color (optional)
        //
        // The Text() function stores the text texture
        // into memory for faster reuse next frame
        // that is why this function is good for
        // rendering some texts that are not that too
        // often changing, like titles, descriptions, 
        // states, but not somthing that changes every frame

        SDL_Rect dRect = {20, 20, -1, 48};
        GUI::TextBox(
            "Hello World!",
            dRect,
            SDL_COLOR_WHITE
        );


        // On the other hand we have Dynamic Text which would be used
        // for something like frame counter, that can definatly not be
        // reused the next frame, or not that offten
        dRect = {20, 80, -1, 24};
        GUI::TextDynamic(
            "Frame Counter: " + to_string(Sys::getCurrentFrame()),
            dRect
        );


        // STYLES --------------------------------------------------------
        // No styles


        Sys::presentFrame();
    }

    TM::cleanup();
    return Sys::cleanup();
}