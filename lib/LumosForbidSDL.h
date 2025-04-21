#ifndef LUMOS_FORBID_SDL_H
#define LUMOS_FORBID_SDL_H

#ifndef ALLOW_DIRECT_SDL
    // Texture Managment
    #define SDL_CreateTexture(...)              static_assert(false, "Direct usage of SDL_CreateTexture is forbidden! Use TM::_CreateTexture wrapper instead.")
    #define SDL_CreateTextureFromSurface(...)   static_assert(false, "Direct usage of SDL_CreateTextureFromSurface is forbidden! Use TM::_CreateTextureFromSurface wrapper instead.")
    #define SDL_QueryTexture(...)               static_assert(false, "Direct usage of SDL_QueryTexture is forbidden! Use TM::_QueryTexture wrapper instead.")
    #define SDL_UpdateTexture(...)              static_assert(false, "Direct usage of SDL_UpdateTexture is forbidden! Use TM::_UpdateTexture wrapper instead.")
    #define SDL_UpdateYUVTexture(...)           static_assert(false, "Direct usage of SDL_UpdateYUVTexture is forbidden! Use TM::_UpdateYUVTexture wrapper instead.")
    #define SDL_LockTexture(...)                static_assert(false, "Direct usage of SDL_LockTexture is forbidden! Use TM::_LockTexture wrapper instead.")
    #define SDL_UnlockTexture(...)              static_assert(false, "Direct usage of SDL_UnlockTexture is forbidden! Use TM::_UnlockTexture wrapper instead.")
    #define SDL_SetTextureBlendMode(...)        static_assert(false, "Direct usage of SDL_SetTextureBlendMode is forbidden! Use TM::_SetTextureBlendMode wrapper instead.")
    #define SDL_GetTextureBlendMode(...)        static_assert(false, "Direct usage of SDL_GetTextureBlendMode is forbidden! Use TM::_GetTextureBlendMode wrapper instead.")
    #define SDL_SetTextureScaleMode(...)        static_assert(false, "Direct usage of SDL_SetTextureScaleMode is forbidden! Use TM::_SetTextureScaleMode wrapper instead.")
    #define SDL_GetTextureScaleMode(...)        static_assert(false, "Direct usage of SDL_GetTextureScaleMode is forbidden! Use TM::_GetTextureScaleMode wrapper instead.")
    #define SDL_SetTextureAlphaMod(...)         static_assert(false, "Direct usage of SDL_SetTextureAlphaMod is forbidden! Use TM::_SetTextureAlphaMod wrapper instead.")
    #define SDL_GetTextureAlphaMod(...)         static_assert(false, "Direct usage of SDL_GetTextureAlphaMod is forbidden! Use TM::_GetTextureAlphaMod wrapper instead.")
    #define SDL_SetTextureColorMod(...)         static_assert(false, "Direct usage of SDL_SetTextureColorMod is forbidden! Use TM::_SetTextureColorMod wrapper instead.")
    #define SDL_GetTextureColorMod(...)         static_assert(false, "Direct usage of SDL_GetTextureColorMod is forbidden! Use TM::_GetTextureColorMod wrapper instead.")
    #define SDL_DestroyTexture(...)             static_assert(false, "Direct usage of SDL_DestroyTexture is forbidden! Use TM::_DestroyTexture wrapper instead.")

    // Drawing Functions
    #define SDL_RenderClear(...)                static_assert(false, "Direct usage of SDL_RenderClear is forbidden! Use TM::_RenderClear wrapper instead.")
    #define SDL_RenderDrawPoint(...)            static_assert(false, "Direct usage of SDL_RenderDrawPoint is forbidden! Use TM::_RenderDrawPoint wrapper instead.")
    #define SDL_RenderDrawPoints(...)           static_assert(false, "Direct usage of SDL_RenderDrawPoints is forbidden! Use TM::_RenderDrawPoints wrapper instead.")
    #define SDL_RenderDrawLine(...)             static_assert(false, "Direct usage of SDL_RenderDrawLine is forbidden! Use TM::_RenderDrawLine wrapper instead.")
    #define SDL_RenderDrawLines(...)            static_assert(false, "Direct usage of SDL_RenderDrawLines is forbidden! Use TM::_RenderDrawLines wrapper instead.")
    #define SDL_RenderDrawRect(...)             static_assert(false, "Direct usage of SDL_RenderDrawRect is forbidden! Use TM::_RenderDrawRect wrapper instead.")
    #define SDL_RenderDrawRects(...)            static_assert(false, "Direct usage of SDL_RenderDrawRects is forbidden! Use TM::_RenderDrawRects wrapper instead.")
    #define SDL_RenderFillRect(...)             static_assert(false, "Direct usage of SDL_RenderFillRect is forbidden! Use TM::_RenderFillRect wrapper instead.")
    #define SDL_RenderFillRects(...)            static_assert(false, "Direct usage of SDL_RenderFillRects is forbidden! Use TM::_RenderFillRects wrapper instead.")
    #define SDL_RenderCopy(...)                 static_assert(false, "Direct usage of SDL_RenderCopy is forbidden! Use TM::_RenderCopy wrapper instead.")
    #define SDL_RenderCopyEx(...)               static_assert(false, "Direct usage of SDL_RenderCopyEx is forbidden! Use TM::_RenderCopyEx wrapper instead.")
    #define SDL_RenderReadPixels(...)           static_assert(false, "Direct usage of SDL_RenderReadPixels is forbidden! Use TM::_RenderReadPixels wrapper instead.")
    #define SDL_RenderGeometry(...)             static_assert(false, "Direct usage of SDL_RenderGeometry is forbidden! Use TM::_RenderGeometry wrapper instead.")
    #define SDL_RenderPresent(...)              static_assert(false, "Direct usage of SDL_RenderPresent is forbidden! Use TM::_RenderPresent wrapper instead.")

    // Renderer Create and Managment 
    #define SDL_CreateRenderer(...)             static_assert(false, "Direct usage of SDL_CreateRenderer is forbidden! Use TM::_CreateRenderer wrapper instead.")
    #define SDL_CreateSoftwareRenderer(...)     static_assert(false, "Direct usage of SDL_CreateSoftwareRenderer is forbidden! Use TM::_CreateSoftwareRenderer wrapper instead.")
    #define SDL_GetRendererInfo(...)            static_assert(false, "Direct usage of SDL_GetRendererInfo is forbidden! Use TM::_GetRendererInfo wrapper instead.")
    #define SDL_GetRenderer(...)                static_assert(false, "Direct usage of SDL_GetRenderer is forbidden! Use TM::_GetRenderer wrapper instead.")
    #define SDL_GetRendererOutputSize(...)      static_assert(false, "Direct usage of SDL_GetRendererOutputSize is forbidden! Use TM::_GetRendererOutputSize wrapper instead.")
    #define SDL_DestroyRenderer(...)            static_assert(false, "Direct usage of SDL_DestroyRenderer is forbidden! Use TM::_DestroyRenderer wrapper instead.")

    // Render Target Managment
    #define SDL_SetRenderTarget(...)            static_assert(false, "Direct usage of SDL_SetRenderTarget is forbidden! Use TM::_SetRenderTarget wrapper instead.")
    #define SDL_GetRenderTarget(...)            static_assert(false, "Direct usage of SDL_GetRenderTarget is forbidden! Use TM::_GetRenderTarget wrapper instead.")

    // Viewport and Clipping
    #define SDL_RenderSetViewport(...)          static_assert(false, "Direct usage of SDL_RenderSetViewport is forbidden! Use TM::_RenderSetViewport wrapper instead.")
    #define SDL_RenderGetViewport(...)          static_assert(false, "Direct usage of SDL_RenderGetViewport is forbidden! Use TM::_RenderGetViewport wrapper instead.")
    #define SDL_RenderSetClipRect(...)          static_assert(false, "Direct usage of SDL_RenderSetClipRect is forbidden! Use TM::_RenderSetClipRect wrapper instead.")
    #define SDL_RenderGetClipRect(...)          static_assert(false, "Direct usage of SDL_RenderGetClipRect is forbidden! Use TM::_RenderGetClipRect wrapper instead.")
    #define SDL_RenderIsClipEnabled(...)        static_assert(false, "Direct usage of SDL_RenderIsClipEnabled is forbidden! Use TM::_RenderIsClipEnabled wrapper instead.")

    // Scaling and Logical Size
    #define SDL_RenderSetScale(...)             static_assert(false, "Direct usage of SDL_RenderSetScale is forbidden! Use TM::_RenderSetScale wrapper instead.")
    #define SDL_RenderGetScale(...)             static_assert(false, "Direct usage of SDL_RenderGetScale is forbidden! Use TM::_RenderGetScale wrapper instead.")
    #define SDL_RenderSetLogicalSize(...)       static_assert(false, "Direct usage of SDL_RenderSetLogicalSize is forbidden! Use TM::_RenderSetLogicalSize wrapper instead.")
    #define SDL_RenderGetLogicalSize(...)       static_assert(false, "Direct usage of SDL_RenderGetLogicalSize is forbidden! Use TM::_RenderGetLogicalSize wrapper instead.")

    // Drawing Color and Blend Mode
    #define SDL_SetRenderDrawColor(...)         static_assert(false, "Direct usage of SDL_SetRenderDrawColor is forbidden! Use TM::_SetRenderDrawColor wrapper instead.")
    #define SDL_GetRenderDrawColor(...)         static_assert(false, "Direct usage of SDL_GetRenderDrawColor is forbidden! Use TM::_GetRenderDrawColor wrapper instead.")
    #define SDL_SetRenderDrawBlendMode(...)     static_assert(false, "Direct usage of SDL_SetRenderDrawBlendMode is forbidden! Use TM::_SetRenderDrawBlendMode wrapper instead.")
    #define SDL_GetRenderDrawBlendMode(...)     static_assert(false, "Direct usage of SDL_GetRenderDrawBlendMode is forbidden! Use TM::_GetRenderDrawBlendMode wrapper instead.")

    // Surface
    // #define SDL_ConvertSurfaceFormat(...)       static_assert(false, "Direct usage of SDL_ConvertSurfaceFormat is forbidden! Use TM::_ConvertSurfaceFormat wrapper instead.")
    // #define SDL_FreeSurface(...)                static_assert(false, "Direct usage of SDL_FreeSurface is forbidden! Use TM::_FreeSurface wrapper instead.")
#endif // ALLOW_DIRECT_SDL

#endif // LUMOS_FORBID_SDL_H
