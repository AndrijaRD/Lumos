#include "../lib/System/Sys.h"
#include "../lib/TextureManager/TM.h"
#include "../lib/GUI/gui.h"

void Image(TextureData td, SDL_Rect dRect){
    SDL_FRect dFRect = {
        static_cast<float>(dRect.x), 
        static_cast<float>(dRect.y), 
        static_cast<float>(dRect.w), 
        static_cast<float>(dRect.h)
    };
    SDL_RenderTexture(Sys::renderer, td.getTexture(), NULL, &dFRect);
}

int main(){
    int err = Sys::initWindow();
    CHECK_ERROR(err);

    err = Sys::initFont("/home/data/DATA/ASSETS/Poppins/Poppins-Regular.ttf");
    CHECK_ERROR(err);

    while(Sys::isRunning){
        Sys::handleEvents();

        GUI::pushBorderRadius(20);
        GUI::Rect({10, 10, 100, 200}, SDL_COLOR_RED, 2);

        Sys::presentFrame();
    }

    return 0;
}
//export PATH="/usr/lib/ccache:$PATH"