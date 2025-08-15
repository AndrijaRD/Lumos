#include "./gui.h"
#include "../System/Sys.h"
/** TODO
 * 
 * Make Button with Icons
 * Make Number Only Input (maybe pushable style)
 * 
 */




string color2hex(const SDL_Color& color){
    ostringstream oss;
    oss << "#";
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.r);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.g);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.b);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.a);
    return oss.str();
}



void GUI::setMaxNumOfLoadedTextures(int num){ MAX_LOADED_TEXTS = num; }


void GUI::pushFontSize(uint fontSize)    { pFontSize = fontSize; }

void GUI::pushTextAlignX(Align value)      { pTextAlignX = value;  }
void GUI::pushTextAlignY(Align value)      { pTextAlignY = value;  }

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
        Sys::printf_warn("No input with that id found.");
        return;
    }

    state->value = newValue;
}

void GUI::pushOutlineStyle(int thickness, SDL_Color color){
    pOutlineThickness = thickness;
    pOutlineColor = color;
}


void GUI::pushPadding(int padding){ pPaddingRect = PaddingRect(padding); }
void GUI::pushPadding(PaddingRect paddingRect) { pPaddingRect = paddingRect; }

void GUI::pushDashLineStyle(int dashLineSize, int dashGapSize){
    pDashLine = true;
    pDashSize = dashLineSize;
    pDashGapSize = dashGapSize;
}


