#include "./gui.h"
#include "../System/Sys.h"

void GUI::Image(
    const TextureData& td, 
    SDL_Rect& dr
) {

    // CHECK IF TEXTURE IS VALID ---------------------------------------------------------
    if(td.getTexture() == nullptr){
        Sys::printf_err("Error message emitted from GUI Image:");
        Sys::printf_err(TM_GOT_NULLPTR_TEX);
        return;
    }

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(dr.w < 0 && dr.h < 0) {
        Sys::printf_err("Error message emitted from GUI Image:");
        Sys::printf_err(TM_INVALID_DRECT);
        return;
    }

    // CALCULATE IF ONE DIMENSION IS MISSING ----------------------------------------------
    if(dr.w == -1) dr.w = dr.h * (float)td.getWidth() / td.getHeight();
    if(dr.h == -1) dr.h = dr.w * (float)td.getHeight() / td.getWidth();

    
    // RENDER THE TEXTURE ON SCREEN -------------------------------------------------------
    
    // If no container is active
    if(activeContainer.empty()){
        int err = TM::_RenderCopy(Sys::renderer, td.getTexture(), NULL, &dr);

        if(err) {
            Sys::printf_err("Error message emitted from GUI Image:");
            Sys::printf_err(TM_RCPY_FAILED);
        }

        return;
    }

    auto container = containerStates.at(activeContainer);

    // If the image is entierly in the visible part of the container
    if(
        dr.y > container.scrollOffset &&
        (dr.y + dr.h) < (container.scrollOffset + container.dRect.h)
    ) {
        SDL_Rect absoluteRect = {
            dr.x + container.dRect.x,
            dr.y - container.scrollOffset + container.dRect.y,
            dr.w,
            dr.h
        };
        TM::_RenderCopy(Sys::renderer, td.getTexture(), NULL, &absoluteRect);
        return;
    }

    // if the image is partialy visible, top edge
    if(
        dr.y < container.scrollOffset &&
        (dr.y + dr.h) > container.scrollOffset
    ) {
        int invisiblePart = container.scrollOffset - dr.y;
        int visiblePart = dr.h - invisiblePart;

        SDL_Rect absoluteRect = {
            dr.x + container.dRect.x,
            container.dRect.y,
            dr.w,
            visiblePart
        };

        SDL_Rect srcRect = {
            0,
            static_cast<int>(td.getHeight() * static_cast<float>(invisiblePart) / dr.h),
            td.getWidth(),
            static_cast<int>(td.getHeight() * static_cast<float>(visiblePart) / dr.h)
        };

        TM::_RenderCopy(Sys::renderer, td.getTexture(), &srcRect, &absoluteRect);
        return;
    }

    // if the image is partialy visible, bottom edge
    int bottomEdge = container.scrollOffset + container.dRect.h;
    if(
        dr.y < bottomEdge &&
        (dr.y + dr.h) > bottomEdge
    ) {
        int visiblePart = bottomEdge - dr.y;

        SDL_Rect absoluteRect = {
            dr.x + container.dRect.x,
            dr.y - container.scrollOffset + container.dRect.y,
            dr.w,
            visiblePart
        };

        SDL_Rect srcRect = {
            0,
            0,
            td.getWidth(),
            static_cast<int>(td.getHeight() * static_cast<float>(visiblePart) / dr.h)
        };

        TM::_RenderCopy(Sys::renderer, td.getTexture(), &srcRect, &absoluteRect);
        return;
    }
}