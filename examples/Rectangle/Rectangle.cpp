#include "../../lib/Lumos.h"

int main(){
    int error;
    error = Sys::initWindow("Rectangle Example");
    if(error != NO_ERROR) exit(EXIT_FAILURE);


    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning){
        Sys::handleEvents();

        GUI::pushBorderRadius(20);
        GUI::pushDashLineStyle(15, 10);
        GUI::Rect(
            {50, 50, 200, 200}, 
            SDL_COLOR_BLUE, 
            5
        );

        Sys::presentFrame();
    }

    TM::cleanup();
    return Sys::cleanup();
}
