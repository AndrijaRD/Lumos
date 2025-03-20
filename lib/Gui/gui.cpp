#include "./gui.h"

/** TODO
 * 
 * Make Button with Icons
 * Make Number Only Input (maybe pushable style)
 * 
 * 
 * 
 */



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


void GUI::pushFontSize(uint fontSize)    { pFontSize = fontSize; }

void GUI::pushTextAlignX(int value)      { pTextAlignX = value;  }
void GUI::pushTextAlignY(int value)      { pTextAlignY = value;  }

void GUI::pushAutoFocus()                       { pAutoFocus = true;    }
void GUI::pushInputLock()                       { pInputLock = true;    }

void GUI::pushBorderRadius(uint radius)         { pBorderRadius = BorderRadiusRect(radius); }
void GUI::pushBorderRadius(BorderRadiusRect radiusRect) { pBorderRadius = radiusRect; }
void GUI::pushBorderRadius(
    uint topLeft,
    uint topRight,
    uint bottomLeft,
    uint bottomRight
){
    pBorderRadius = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight
    };
}

void GUI::pushDefaultValue(string value)     { pDefaultValue = value; }
void GUI::setInputValue(string inputId, string newValue){
    // Get the pointer to the inputs state using uniqueId
    InputState* state = nullptr;
    auto it = inputStates.find(inputId);
    if(it != inputStates.end()){
        state = &it->second;
    } else{
        cout << "[WARNING] No input with that id found." << endl;
        return;
    }

    state->value = newValue;
}

void GUI::pushOutlineStyle(int thickness, SDL_Color color){
    pOutlineThickness = thickness;
    pOutlineColor = color;
}


void GUI::pushPadding(int padding){ pPaddingRect = paddingRect(padding); }
void GUI::pushPadding(paddingRect paddingRect) { pPaddingRect = paddingRect; }

void GUI::pushDashLineStyle(int dashLineSize, int dashGapSize){
    pDashLine = true;
    pDashSize = dashLineSize;
    pDashGapSize = dashGapSize;
}


