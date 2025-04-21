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
    const SDL_Rect& rect, 
    int contentHeight
) {
    auto state = getContainerState(containerId);

    state->dRect = rect;
    state->contentHeight = contentHeight;

    activeContainer = containerId;
}


void GUI::endContainer() {
    ContainerState* state = nullptr;
    if(!activeContainer.empty()){
        state = getContainerState(activeContainer);
    } else return;


    // renderScrollbar(state);
    
    state->lastActiveFrame = Sys::getCurrentFrame();
    activeContainer.clear();
}


void GUI::renderScrollbar(){
    ContainerState* state = nullptr;

    auto it = containerStates.find(activeContainer);
    if (it != containerStates.end()) {
        state = &it->second;
    } else {
        return;
    }

     // --- Scrollbar Logic ---

    // Determine if we should show the scrollbar.
    // Show it if the container was scrolled within the last 120 frames
    // OR if the mouse is hovering over the scrollbar area.
    int currentFrame = Sys::getCurrentFrame();
    bool showScrollBar = (currentFrame - state->lastActiveFrame < 120);

    // Define the scrollbar track.
    const int scrollBarWidth = 10;
    SDL_Rect scrollBarTrack = {
        state->dRect.x + state->dRect.w - scrollBarWidth, // right edge of container
        state->dRect.y,
        scrollBarWidth,
        state->dRect.h
    };

    // Calculate the thumb (the draggable portion):
    // Thumb height is proportional to the visible area vs. full content.
    float visibleRatio = (float) state->dRect.h / state->contentHeight;
    int thumbHeight = (int)(visibleRatio * state->dRect.h);
    // Ensure a minimum thumb size.
    if(thumbHeight < 20)
        thumbHeight = 20;

    // Maximum scroll offset is the extra content beyond the visible area.
    int maxScroll = state->contentHeight - state->dRect.h;
    // Calculate thumb Y position within the track.
    int thumbY = 0;
    if(maxScroll > 0) {
         thumbY = (int)(((float) state->scrollOffset / maxScroll) * (state->dRect.h - thumbHeight));
    }
    SDL_Rect thumbRect = {
        scrollBarTrack.x,
        state->dRect.y + thumbY,
        scrollBarTrack.w,
        thumbHeight
    };

    // Check if mouse is over the scroll bar track or thumb.
    bool mouseOverScrollBar = Sys::Mouse::isHovering(scrollBarTrack);
    if(mouseOverScrollBar)
         showScrollBar = true;

    // --- Dragging Logic ---
    // If mouse is down and either over the thumb or already dragging, update the scroll offset.
    if(Sys::Mouse::isDown() && (Sys::Mouse::isHovering(thumbRect) || state->scrollbarDragging)) {
        if(!state->scrollbarDragging) {
            // Start dragging.
            state->scrollbarDragging = true;
            state->scrollbarDragStartMouseY = Sys::Mouse::getPos().y;
            state->scrollbarDragStartScrollOffset = state->scrollOffset;
        } else {
            // Update scroll offset based on drag movement.
            int currentMouseY = Sys::Mouse::getPos().y;
            int delta = currentMouseY - state->scrollbarDragStartMouseY;
            // Determine how much movement corresponds to a change in scrollOffset.
            // The track available for thumb movement is: (container visible height - thumbHeight)
            float trackMovable = state->dRect.h - thumbHeight;
            if(trackMovable > 0) {
                float scrollChange = ((float) delta / trackMovable) * maxScroll;
                state->scrollOffset = state->scrollbarDragStartScrollOffset + (int) scrollChange;
                // Clamp scrollOffset.
                if(state->scrollOffset < 0)
                    state->scrollOffset = 0;
                if(state->scrollOffset > maxScroll)
                    state->scrollOffset = maxScroll;
            }
        }
    } else {
        // Mouse is not down; end dragging.
        state->scrollbarDragging = false;
    }

    // --- Render Scrollbar ---
    if(showScrollBar) {
        // Render track.
        SDL_Color trackColor = {50, 50, 50, 150}; // semi-transparent dark
        GUI::pushBorderRadius(5);
        GUI::Rect(scrollBarTrack, trackColor, -1);

        // Render thumb.
        SDL_Color thumbColor = {200, 200, 200, 200}; // lighter color
        GUI::Rect(thumbRect, thumbColor, -1);
    }
}


/**

SDL_Point GUI::getMousePosInContainer(const std::string& containerId) {
    auto it = containerStates.find(containerId);
    if (it == containerStates.end()) return { -1, -1 }; // If container doesn't exist, return invalid position.

    ContainerState& state = it->second;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Convert mouse position to container-relative position
    SDL_Point relativeMousePos = {
        mouseX - state.dRect.x,               // Offset by container's X position
        mouseY - state.dRect.y + state.scrollOffset // Offset by container's Y position & scroll
    };

    return relativeMousePos;
}

bool GUI::isRectVisibleInContainer(const std::string& containerId, const SDL_Rect& rect) {
    auto it = containerStates.find(containerId);
    if (it == containerStates.end()) {
        // If the container does not exist, nothing is visible.
        return false;
    }
    const ContainerState& state = it->second;
    
    // The visible portion of the container's off-screen texture is:
    SDL_Rect visibleRect = { 0, state.scrollOffset, state.dRect.w, state.dRect.h };
    
    // Check for intersection:
    bool intersects = 
        (rect.x < visibleRect.x + visibleRect.w) &&
        (rect.x + rect.w > visibleRect.x) &&
        (rect.y < visibleRect.y + visibleRect.h) &&
        (rect.y + rect.h > visibleRect.y);
    
    return intersects;
}


GUI::ContainerState* GUI::getContainerState(const string& containerId){
    auto it = containerStates.find(containerId);
    if (it != containerStates.end())
        return &(it->second);

    return nullptr;
}

*/