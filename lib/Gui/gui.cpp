#include "./gui.h"


/** Remove Oldest
 * 
 * INTERNAL USE
 * 
 * Used for removing the oldest text texture of loadedTexts map.
 */
void GUI::removeOldest(){
    // Iterator to the oldest object
    auto oldestIt = loadedTexts.begin();

    // Iterate through the map and find the oldest element
    for (auto it = loadedTexts.begin(); it != loadedTexts.end(); ++it) {
        if (oldestIt == loadedTexts.begin() || oldestIt->second > it->second) {
            oldestIt = it;
        }
    }

    // Free the texture and erase the item from the map
    TM::freeTexture(oldestIt->second.td);
    loadedTexts.erase(oldestIt);
}




string color2hex(const SDL_Color& color){
    ostringstream oss;
    oss << "#";
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.r);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.g);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.b);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.a);
    return oss.str();
}



void GUI::setMaxNumOfLoadedTextures(const int& num){ max_num_of_loaded_textures = num; }


void GUI::pushFontSize(const uint& fontSize)    { pFontSize = fontSize; }

void GUI::pushTextAlignX(const int& value)      { pTextAlignX = value;  }
void GUI::pushTextAlignY(const int& value)      { pTextAlignY = value;  }

void GUI::pushAutoFocus()                       { pAutoFocus = true;    }
void GUI::pushInputLock()                       { pInputLock = true;    }

void GUI::pushBorderRadius(const uint& radius)  { 
    pBorderRadius = {radius, radius, radius, radius};
}
void GUI::pushBorderRadius(const BorderRadiusRect& radiusRect) { 
    pBorderRadius = radiusRect; 
}
void GUI::pushBorderRadius(
    const uint& topLeft,
    const uint& topRight,
    const uint& bottomLeft,
    const uint& bottomRight
){
    pBorderRadius = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight
    };
}
