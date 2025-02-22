#include "gui.h"
#include "../System/Sys.h"

void GUI::startContainer(string containerId, SDL_Rect rect, int contentHeight) {
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

    state->containerTex = SDL_CreateTexture(
        Sys::r,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        rect.w,
        contentHeight
    );

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(Sys::r, &r, &g, &b, &a);

    state->previousRenderTarget = SDL_GetRenderTarget(Sys::r);

    // Set the off-screen texture as the current render target.
    SDL_SetRenderTarget(Sys::r, state->containerTex);
    SDL_SetRenderDrawColor(Sys::r, r, g, b, a);
    SDL_RenderClear(Sys::r);
    state->dRect = rect;
    state->contentHeight = contentHeight;
}


void GUI::endContainer(string containerId) {
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

    SDL_SetRenderTarget(Sys::r, state->previousRenderTarget);

    SDL_Rect srcRect = {
        0,                         // start at left edge of texture
        state->scrollOffset,       // vertical offset into the texture
        state->dRect.w,            // same width as container
        state->dRect.h             // visible height of the container
    };

    // Copy the off-screen texture onto the main renderer.
    SDL_RenderCopy(Sys::r, state->containerTex, &srcRect, &state->dRect);


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

    // Clean up the off-screen texture.
    SDL_DestroyTexture(state->containerTex);
    state->containerTex = nullptr;
    state->lastActiveFrame = currentFrame;
}


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

