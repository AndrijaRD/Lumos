#include "./gui.h"
#include "../System/Sys.h"



void GUI::Image(
    SDL_Texture* texture,
    SDL_Rect& dr_org
) {
    // ===== CREATE FRECT ===== ===== =====
    SDL_FRect dr = {
        static_cast<float>(int(std::round(dr_org.x))),
        static_cast<float>(int(std::round(dr_org.y))),
        static_cast<float>(int(std::round(dr_org.w))),
        static_cast<float>(int(std::round(dr_org.h)))
    };

    // ===== CHECK POINTER ===== ===== =====
    if(texture == nullptr) {
        Sys::printf_err("Got Empty Texture, GUI::Image(SDL_Texture*);");
        return;
    }

    // ===== CHECK RECT SIZE ===== ===== =====
    if(dr_org.w <= 0 && dr_org.h <= 0) {
        Sys::printf_err("Got invalid SDL_Rect, GUI::Image(SDL_Texture*);");
        return;
    }

    // ===== FETCH TEXTURE SIZE ===== ===== =====
    SDL_PropertiesID prop = SDL_GetTextureProperties(texture);
    int texWidth = (int)SDL_GetNumberProperty(prop, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
    int texHeight = (int)SDL_GetNumberProperty(prop, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);

    // ===== UPDATE MISSING DIMENSIONS ===== ===== =====
    if(dr.w == -1) dr.w = dr.h * (float)texWidth / texHeight;
    if(dr.h == -1) dr.h = dr.w * (float)texHeight / texWidth;

    dr_org.w = static_cast<int>(dr.w);
    dr_org.h = static_cast<int>(dr.h);



    // ===== IF THERE ARE NO ACTIVE CONTAINERS ===== ===== =====
    if(activeContainer.empty()){

        int err = SDL_RenderTexture(Sys::renderer, texture, NULL, &dr);

        if(!err) {
            Sys::printf_err("[GUI::Image(SDL_Texture*)] SDL_RenderTexture Failed: ");
            Sys::printf_err(SDL_GetError());
        }

        return;
    }

    // ===== IF THERE ARE ACTIVE CONTAINERS ===== ===== =====

    auto container = getContainerState(activeContainer);
    container->contentHeight = std::max(dr_org.y + dr_org.h, container->contentHeight);

    // ===== ===== ===== IF TEXTURE SHOULD BE 100% VISSIBLE
    if( dr.y > container->scrollOffset &&
        (dr.y + dr.h) < (container->scrollOffset + container->dRect.h)
    ) {
        // FIND ABSOLUTE POSITION ON THE SCREEN
        SDL_FRect absoluteRect = {
            static_cast<float>(dr.x + container->dRect.x),
            static_cast<float>(dr.y - container->scrollOffset + container->dRect.y),
            static_cast<float>(dr.w),
            static_cast<float>(dr.h)
        };

        // RENDER
        SDL_RenderTexture(Sys::renderer, texture, NULL, &absoluteRect);
        
        return;
    }

    // ===== ===== ===== IF TEXTURE'S TOP IS COVERED
    if(
        dr.y < container->scrollOffset &&
        (dr.y + dr.h) > container->scrollOffset
    ) {
        int invisiblePart = container->scrollOffset - dr.y;
        int visiblePart = dr.h - invisiblePart;

        SDL_FRect absoluteRect = {
            static_cast<float>(dr.x + container->dRect.x),
            static_cast<float>(container->dRect.y),
            static_cast<float>(dr.w),
            static_cast<float>(visiblePart)
        };

        SDL_FRect srcRect = {
            static_cast<float>(0),
            static_cast<float>(texHeight * static_cast<float>(invisiblePart) / dr.h),
            static_cast<float>(texWidth),
            static_cast<float>(texHeight * static_cast<float>(visiblePart) / dr.h)
        };

        SDL_RenderTexture(Sys::renderer, texture, &srcRect, &absoluteRect);
        return;
    }

    // ===== ===== ===== IF TEXTURES'S BOTTOM IS COVERED
    int bottomEdge = container->scrollOffset + container->dRect.h;
    if(
        dr.y < bottomEdge &&
        (dr.y + dr.h) > bottomEdge
    ) {
        float visiblePart = static_cast<float>( bottomEdge - dr.y );

        SDL_FRect absoluteRect = {
            static_cast<float>(dr.x + container->dRect.x),
            static_cast<float>(dr.y - container->scrollOffset + container->dRect.y),
            static_cast<float>(dr.w),
            visiblePart
        };

        SDL_FRect srcRect = {
            0,
            0,
            static_cast<float>(texWidth),
            static_cast<float>(texHeight * visiblePart / dr.h)
        };

        SDL_RenderTexture(Sys::renderer, texture, &srcRect, &absoluteRect);
        return;
    }
}






void GUI::Image(
    TextureData& td, 
    SDL_Rect& dr_org
) {
    Image(td.getTexture(), dr_org);
}


void GUI::Image(
    SVGIcon& icon,
    SDL_Rect& dRect
) {
    SDL_Rect dr = { dRect.x, dRect.y, dRect.w, dRect.w };
    GUI::Image(icon.getIcon(dRect.w), dr);
}
