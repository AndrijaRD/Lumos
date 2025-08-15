#include "gui.h"
#include "../System/Sys.h"


GUI::ContainerState* GUI::getContainerState(const string& containerId){
    ContainerState* state = nullptr;

    auto it = containerStates.find(containerId);
    if (it != containerStates.end()) {
        // Save the pointer to that LoadedText item
        state = &it->second;
    } else {
        // If the state doesnt exist, create it
        containerStates.insert({containerId, ContainerState(containerId)});
        state = &containerStates.at(containerId);
    }

    return state;
}


void GUI::beginContainer(
    const string& containerId, 
    const SDL_Rect& rect
) {
    auto state = getContainerState(containerId);

    state->dRect = rect;
    state->contentHeight = 0;

    activeContainer = containerId;
}


void GUI::endContainer() {
    ContainerState* state = nullptr;
    if(!activeContainer.empty()){
        state = getContainerState(activeContainer);
    } else return;

    state->contentHeight += 20;
    state->contentHeight = std::max(state->dRect.h, state->contentHeight);
    

    renderVerticalScrollbar(
        state->dRect,
        state->contentHeight,
        state->scrollOffset
    );
    
    state->lastActiveFrame = Sys::getCurrentFrame();
    activeContainer.clear();
}

void GUI::renderVerticalScrollbar(
    const SDL_Rect&   container,      // x,y,w,h of the visible area
    int                contentHeight, // total height of scrollable content
    int&               scrollOffset   // current scroll offset—in/out
) {
    // 1) If nothing to scroll, bail
    if (contentHeight <= container.h) {
        return;
    }

    // 2) Geometry
    const float barW    = 8.0f;  
    const SDL_FRect track = {
        float(container.x + container.w - barW),
        float(container.y),
        barW,
        float(container.h)
    };

    // Thumb height: visible ratio of content
    float visibleRatio = float(container.h) / float(contentHeight);
    float thumbH       = std::max(track.h * visibleRatio, 20.0f);

    // Max scrollable and max thumb travel
    int   maxScroll    = contentHeight - container.h;
    float maxTravel    = track.h - thumbH;

    // Thumb Y based on scrollOffset
    float thumbY = track.y + (float(scrollOffset) / maxScroll) * maxTravel;

    SDL_FRect thumb = { track.x, thumbY, track.w, thumbH };

    // 3) Dragging state (function‐static)
    static bool dragging = false;
    static int  dragStartMouseY = 0;
    static int  dragStartOffset = 0;

    SDL_Point m = Mouse::getAbsolutePos();  // your SDL3 mouse wrapper

    // Are we over the thumb?
    bool overThumb = (m.x >= int(thumb.x) && m.x < int(thumb.x + thumb.w)
                   && m.y >= int(thumb.y) && m.y < int(thumb.y + thumb.h));

    // Mouse pressed?
    if (Mouse::isDownLeft()) {
        if (!dragging && overThumb) {
            dragging = true;
            dragStartMouseY = m.y;
            dragStartOffset = scrollOffset;
        }
        if (dragging) {
            int dy = m.y - dragStartMouseY;
            float frac = float(dy) / maxTravel;
            int   newOff = dragStartOffset + int(frac * maxScroll);
            scrollOffset = std::clamp(newOff, 0, maxScroll);
        }
    } else {
        dragging = false;
    }

    // 4) Draw track (dark gray, semi‐opaque)
    SDL_SetRenderDrawColor(Sys::renderer, 45, 45, 45, 128);
    SDL_RenderFillRect(Sys::renderer, &track);

    // 5) Draw thumb (light gray)
    SDL_SetRenderDrawColor(Sys::renderer, 180, 180, 180, 200);
    SDL_RenderFillRect(Sys::renderer, &thumb);
}






bool GUI::isRectVisible(SDL_Rect rect){
    if( rect.x < 0 && rect.x + rect.w < 0 ) return false;
    if( rect.y < 0 && rect.y + rect.y < 0 ) return false;

    if(activeContainer.empty()){
        if( rect.x > Sys::winWidth &&  rect.x + rect.w > Sys::winWidth ) return false;
        if( rect.y > Sys::winHeight && rect.y + rect.h > Sys::winHeight ) return false;

        return true;
    } else {
        auto container = containerStates.at(activeContainer);

        if( rect.x > container.dRect.w &&  rect.x + rect.w > container.dRect.w ) return false;

        if( 
            rect.y < container.scrollOffset && 
            rect.y + rect.h < container.scrollOffset 
        ) return false;

        if(
            rect.y > container.scrollOffset + container.dRect.h &&
            rect.y + rect.h > container.scrollOffset + container.dRect.h
        ) return false;

        return true;
    }

    return true;
}

