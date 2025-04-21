#include "../../lib/lib.h"
#include "../../lib/System/Sys.h"
#include "../../lib/TextureManager/TM.h"
#include "../../lib/Gui/gui.h"
#include "../../lib/PqDB/db.h"

int main(){
    int error;

    cout << "VERSION: " << VERSION << endl;

    // First thing that needs to be done is for general system
    // to be initialized which created some variabled like:
    //      - Sys::renderer
    //      - Sys::window
    // As arguments it takes window title, fullscreen, and 
    // width and height of the windiw
    error = Sys::initWindow(
        "Empty Window Example", // WINDOW TITLE
        false,                  // SHOULD WINDOW BE FULLSCREEN
        (1920*0.75),            // WINDOW WIDTH
        (1080*0.75)             // WINDOW HEIGHT
    );

    // Now if the error code is not 0 (NO_ERROR) we exit as 
    // that means something went wrong
    if(error != NO_ERROR) exit(EXIT_FAILURE);



    // We can set the window background color using setClearColor
    // and then passing the SDL_Color object.
    // We can also use some built in color:
    //      - SDL_COLOR_BLACK
    //      - SDL_COLOR_WHITE
    //      - ... (SDL_COLOR_*)
    Sys::setClearColor({21, 20, 21, 255});


    // Now the fonts needs to be initialized
    // They allow the app to render GUI elements like
    //      - GUI::Button
    //      - GUI::Text
    //      - GUI::Input
    // The function takes the path to the font
    error = Sys::initFont("../../assets/fonts/font.ttf");
    

    // If the access to the Posgresql Database is needed
    // then this function needs to be runned as it
    // establishes the connection to the database
    error = DB::init("projectdata", "postgres", "root");
    CHECK_ERROR(error);

    // Using CHECK_ERROR(err_code) we can check what
    // error message did the function return the macro
    // will, if error is present, print out the message

    // GOOD FUNCTIONS
    // TM::exportTexture()
    // TM::loadTexture()
    // TM::exportSurface()
    // TM::convert_textureTo(td, myMat);
    // TM::convert_toTexture(myMat, td);
    // TM::copyTexture(srcTd, dstTd);
    // TM::resizeTexture(td, newWidth, newHeight);
    //
    // GUI::Image();
    // GUI::Text();
    // GUI::Button();
    //
    // GUI::Rect();
    // GUI::Circle();
    // GUI::Line();
    // 
    // GUI::pushBorderRadius();

    TextureData td;
    TM::loadTexture(td, "./opencv_test.png");
    
    

    SDL_Rect dRect = {50, 50, 400, 200};
    SDL_Rect imgRect = {10, 250, 200, 100};

    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning) {
        // This function needs to be runned
        // at the start of every frame, it:
        //      - Clears the screen
        //      - Updates the variables like mouse position and window size
        //      - Handles events like keypresses and window exit code
        Sys::handleEvents();

        // App Code and Logic...
        GUI::Rect(dRect, SDL_COLOR_RED, 1);
        GUI::beginContainer("myContainer", dRect, 600);

        GUI::Button("Hello", imgRect);

        GUI::endContainer();

        // TAnd this is runned at the end of the each frame to make
        // sure that frame rate is stable and to draw the current 
        // renderer onto the screen
        Sys::presentFrame();
        cout << "---" << endl;
    }


    // At the end of the app these two lines need to be
    // called to makes sure that all of the textures
    // and resources are freed
    // TM::cleanup();
    return Sys::cleanup();
}