#include "../../lib/Lumos.h"

int main(){
    int error;
    error = Sys::initWindow("Rectangle Example");
    if(error != NO_ERROR) exit(EXIT_FAILURE);


    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning){
        Sys::handleEvents();


        // App Code and Logic...

        // First we create a variable that will
        // hold the position and size of the rect
        SDL_Rect dRect = {
            20,     // Xpos = 20px
            20,     // Ypos = 20px
            100,    // Width = 100px
            100     // Height = 100px
        };
        // Now we run the GUI function Rect
        // This will draw a filled white rect on the (20, 20) size of 100x100
        GUI::Rect(dRect);

        

        // We can also make a rect width custom color
        // And with custom Border Radius
        dRect.x += 200; // (moving the x pos of the rect 200px to the right)
        GUI::pushBorderRadius(20);
        GUI::Rect(dRect, SDL_COLOR_BLUE);



        // And also we can make an outline of the rectangle
        // By setting the thickness to 1
        dRect.x += 200;
        GUI::Rect(dRect, SDL_COLOR_RED, 1);



        // And we can make one animating rectangle
        // By making it so the width will range
        // from 0 to 500, every frame growing by 5px
        SDL_Rect animatingRect = {
            20, 
            220,
            (Sys::getCurrentFrame() % 100)* 5, 
            100
        };
        GUI::pushBorderRadius({20, 0, 20, 0});
        GUI::Rect(animatingRect, SDL_COLOR_GREEN);

        Sys::presentFrame();
    }

    TM::cleanup();
    return Sys::cleanup();
}