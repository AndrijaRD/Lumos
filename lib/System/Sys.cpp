#include "Sys.h"
#include "../GUI/gui.h"


unordered_map<int, string> Sys::errorMap = {
    {NO_ERROR,                          "NO_ERROR"},

    {SYS_SDL_INIT_ERROR,                "SYS_SDL_INIT_ERROR"},
    {SYS_FONT_INIT_ERROR,               "SYS_FONT_INIT_ERROR"},
    {SYS_FONT_PATH_ERROR,               "SYS_FONT_PATH_ERROR"},
    {SYS_WINDOW_INIT_ERROR,             "SYS_WINDOW_INIT_ERROR"},
    {SYS_RENDERER_INIT_ERROR,           "SYS_RENDERER_INIT_ERROR"},
    {SYS_FPS_TOO_LOW,                   "SYS_FPS_TOO_LOW"},
    {SYS_FPS_TOO_HIGH,                  "SYS_FPS_TOO_HIGH"},
    {SYS_FONT_NOT_INITED,               "SYS_FONT_NOT_INITED"},

    {TM_SURFACE_CREATE_ERROR,           "TM_SURFACE_CREATE_ERROR"},
    {TM_SURFACE_CONVERT_ERROR,          "TM_SURFACE_CONVERT_ERROR"},
    {TM_TEXTURE_CREATE_ERROR,           "TM_TEXTURE_CREATE_ERROR"},
    {TM_TEXTURE_SET_BLENDMODE_ERROR,    "TM_TEXTURE_SET_BLENDMODE_ERROR"},
    {TM_TEXTURE_UPDATE_ERROR,           "TM_TEXTURE_UPDATE_ERROR"},
    {TM_GOT_NULLPTR_TEX,                "TM_GOT_NULLPTR_TEX"},
    {TM_INVALID_DRECT,                  "TM_INVALID_DRECT"},
    {TM_RCPY_FAILED,                    "TM_RCPY_FAILED"},
    {TM_SRT_FAILED,                     "TM_SRT_FAILED"},
    {TM_SRDC_FAILED,                    "TM_SRDC_FAILED"},
    {TM_RCLR_FAILED,                    "TM_RCLR_FAILED"},
    {TM_STSM_FAILED,                    "TM_STSM_FAILED"},
    {TM_FILL_RECT_ERROR,                "TM_FILL_RECT_ERROR"},
    {TM_INVALID_LINE_LENGTH,            "TM_INVALID_LINE_LENGTH"},
    {TM_MAT_INVALID_FORMAT,             "TM_MAT_INVALID_FORMAT"},
    {TM_RRP_FAILED,                     "TM_RRP_FAILED"},
    
    {DB_CONNECTION_ERROR,               "DB_CONNECTION_ERROR"},
    {DB_PREPARE_ERROR,                  "DB_PREPARE_ERROR"},
    {DB_EXEC_RESULT_ERROR,              "DB_EXEC_RESULT_ERROR"},
    {DB_EXEC_NOT_PREPARED_ERROR,        "DB_EXEC_NOT_PREPARED_ERROR"},
    {DB_INVALID_RESULT,                 "DB_INVALID_RESULT"},
    {DB_INVALID_ROW_COLUMN,             "DB_INVALID_ROW_COLUMN"},
    {DB_INVALID_RES_VALUE,              "DB_INVALID_RES_VALUE"},
    {DB_EMPTY_STATEMENT_PARAM,          "DB_EMPTY_STATEMENT_PARAM"},
    {DB_REPREPARATION,                  "DB_REPREPARATION"},

    {INVALID_ARGUMENTS_PASSED,          "INVALID_ARGUMENTS_PASSED"}
};




/** System Init
 * 
 * Sets up the entier SDL system.
 * Creates Window.
 * Creates Renderer.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::initWindow(
    const string& winTitle,
    const bool& fullscreen,
    const int& windowWidth,
    const int& windowHeight
){
    // SET THE MAIN THREAD ID ----------------------------------------------
    mainThreadId = this_thread::get_id();

    Sys::windowTitle = winTitle;
    Sys::isFullscreen = fullscreen;
    Sys::wWidth = windowWidth;
    Sys::wHeight = windowHeight;

    // SDL INIT ----------------------------------------------------------
    auto flags = SDL_INIT_VIDEO | 
                SDL_INIT_EVENTS |
                SDL_INIT_AUDIO  ;

    int status = SDL_Init(flags);

    if(status){
        cout << "[INIT] Subsystem Initialized..." << endl;
    } else {
        cout << "[FATAL] Failed to initialize subsystems!" << endl;
        cout << "Message: " << SDL_GetError() << endl;
        return SYS_SDL_INIT_ERROR;
    }


    // CREATE WINDOW ------------------------------------------------------
    flags = SDL_WINDOW_RESIZABLE  |
            SDL_WINDOW_HIGH_PIXEL_DENSITY;
            //SDL_WINDOW_BORDERLESS;
    if(isFullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    win = SDL_CreateWindow(
        windowTitle.c_str(),
        wWidth, 
        wHeight, 
        flags
    );
    if(win){
        cout << "[INIT] Window created..." << endl;
    } else{
        cout << "[FATAL] Failed to create window!" << endl;
        return SYS_WINDOW_INIT_ERROR;
    }



    // CREATE RENDERER -----------------------------------------------------
    const string backends = "gpu,vulkan,opengl,opengles2,software";
    r = SDL_CreateRenderer(win, backends.c_str());

    if(!r){
        cout << "[FATAL] Failed to create rederer!" << endl;
        return SYS_RENDERER_INIT_ERROR;
    }

    SDL_SetRenderDrawBlendMode(Sys::renderer, SDL_BLENDMODE_BLEND);
    
    cout << "[INIT] Renderer created..."  << endl;
    cout << "\tVideoSubsystem: " << SDL_GetCurrentVideoDriver() << endl;
    cout << "\tRender backend: " << SDL_GetRendererName(r) << "\n";
    

    Mouse::defaultCursor    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    Mouse::pointerCursor    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    Mouse::textCursor       = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
    Mouse::waitCursor       = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
    Mouse::notAllowedCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);

    TM::rast = nsvgCreateRasterizer();

    return NO_ERROR;
}



/** Font Init
 * 
 * Initializes the Fonts.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::initFont(const string& fontPath){
    // FONT INIT ---------------------------------------------------------
    int status = TTF_Init();
    if(!status){
        cout << "[FATAL] Failed to initialize fonts! TTF_Init() failed." << endl;
        return SYS_FONT_INIT_ERROR;
    }

    if(!fs::exists(fontPath)){
        cout << "[FATAL] Failed to initialize fonts! Incorect path." << endl;
        return SYS_FONT_INIT_ERROR;
    }

    Sys::fontPath = fontPath;

    cout << "[INIT] Fonts Initialized..." << endl;
    return NO_ERROR;
}


TTF_Font* Sys::getFont(const int& fontSize){
    TTF_Font* font = nullptr;
    if(Sys::fontMap.count(fontSize)){
        font = Sys::fontMap[fontSize];
    } else {
        font = TTF_OpenFont(Sys::fontPath.c_str(), fontSize);
        if(font == nullptr){
            cout << "[FATAL] Failed to load font!" << endl;
            return nullptr;
        }
        Sys::fontMap.insert({fontSize, font});
    }
    return font;
}



/** Handle Events
 * 
 * This function should be runned at the start of each frame.
 * 
 * It updates all of the values accordingly and clears the screen.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::handleEvents(){
    uint64_t error = NO_ERROR;

    // Calculate the delta Time --------------------------------------------------------------------------------------
    frameStart = SDL_GetTicks();

    // Avoid calculation in the first frame
    if (previousFrameStart != 0) 
        deltaTime = (frameStart - previousFrameStart);
    
    previousFrameStart = frameStart;



    // Clear the screen ----------------------------------------------------------------------------------------------
    SDL_SetRenderDrawColor(Sys::r, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(Sys::r);


    // GET NEW FRAME VALUES -------------------------------------------------------------------------------------------
    SDL_GetWindowSize(Sys::win, &Sys::wWidth, &Sys::wHeight);               // Getting window width and height

    // RESET INPUT STATE FOR THIS FRAME
    Keyboard::clearFrame();
    Mouse   ::clearFrame();

    // HANDLE EVENTS --------------------------------------------------------------------------------------------------
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_EVENT_QUIT){
            isRunning = false;
        }

        // Let keyboard & mouse update their internal state
        Keyboard::handleEvent(event);
        Mouse   ::handleEvent(event);

        // Scrolling
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            // Get mouse position once
            SDL_Point m = Mouse::getAbsolutePos();
    
            // For each container that was drawn in the previous frame...
            for (auto& [id, state] : GUI::containerStates) {
                // Only consider containers that were actually rendered last frame
                if (state.lastActiveFrame != getCurrentFrame() - 1) {
                    continue;
                }
    
                // Is mouse over that container's rectangle?
                const SDL_Rect& r = state.dRect;
                if (m.x < r.x || m.x >= r.x + r.w ||
                    m.y < r.y || m.y >= r.y + r.h)
                {
                    continue;
                }
    
                // Scroll it
                int maxScroll = state.contentHeight - r.h;
                if (maxScroll <= 0) break;
    
                // Wheel.y > 0 means wheel-up (away from user)
                // We subtract so wheel-up scrolls content up (offset ↓)
                int delta = event.wheel.y * state.scrollSpeed;
                state.scrollOffset = std::clamp(
                    state.scrollOffset - delta,
                    0,
                    maxScroll
                );
    
                // Keep the scrollbar visible
                state.lastActiveFrame = getCurrentFrame();
    
                // If you only ever want one container to scroll per wheel event,
                // uncomment the next line to break after the first hit.
                break;
            }
        }
    }

    return error;
}




/** Present Frame
 * 
 * This function should be called at the end of each frame cycle.
 * 
 * It presents the frame and and makes a delay for stable FPS and delta time.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::presentFrame(){
    uint64_t error = NO_ERROR;

    // PRESENT THE NEW FRAME ON THE SCREEN ----------------------------------------------------------------------------
    SDL_RenderPresent(Sys::r);


    // FRAME DELAY ----------------------------------------------------------------------------------------------------
    Uint64 currentFrameDuration = SDL_GetTicks() - frameStart;
    if((Uint64)(1000 / FPS) > currentFrameDuration){
        uint64_t timeRemaining = (1000 / FPS) - currentFrameDuration;
        SDL_Delay(timeRemaining);

        // if(dynamicFPS){
        //     int additionalFrames = timeRemaining / (1000 / FPS);
        //     FPS = std::min(maxFPS, FPS + additionalFrames);
        // }
    } else {
        error = SYS_FPS_TOO_HIGH;
        // if(dynamicFPS) {
        //     FPS = std::max(minFPS, FPS - 5);
        // }
    }


    // UPDATE FRAME COUNTER -------------------------------------------------------------------------------------------
    frameCounter++;
    
    // Since the int has a finite size if it comes to its limit, the counter is restarted
    if(frameCounter >= 0xFFFFFFFF) frameCounter = 0; 

    return error;
}




/** Cleanup
 * 
 * This function should be called at the end of the application to properly shutdown all of the systems.
 *
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::cleanup(){
    // DESTROY AND FREE EVERYTHING ------------------------------------------------------------------------------------
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(r);
    TTF_Quit();
    SDL_Quit();

    cout << "Game Finished." << endl;
    return NO_ERROR;
}



string Sys::checkError(int error){
    if (errorMap.count(error)) {
        return errorMap[error];
    } else {
        return "Unknown Error";
    }
}


int Sys::getOS() { return OS; }
void Sys::setClearColor(const SDL_Color& color) { clearColor = color; }


int Sys::getFPS() { return FPS; }
void Sys::setFPS(const int& newFPS ) { FPS = min(max(20, newFPS), 144); }
// void Sys::setDynamicFPS(bool dFPS, int maxF, int minF){
//     dynamicFPS = dFPS;
//     maxFPS = std::max(20, maxF);
//     minFPS = std::min(20, minF);
// }
int Sys::getCurrentFrame() { return frameCounter; }



bool isPointInRect(SDL_Point point, SDL_Rect rect){
    return (
        point.x >= rect.x && 
        point.x <= rect.x + rect.w &&
        point.y >= rect.y && 
        point.y <= rect.y + rect.h
    );
}


bool Sys::isMainThread(){
    return this_thread::get_id() == mainThreadId;
}


void Sys::printf_info(string msg){
    if(Sys::debugLevel == All){
        cout << "[INFO] " << msg << endl;
    }
}

void Sys::printf_warn(string msg){
    if(
        Sys::debugLevel == All || 
        Sys::debugLevel == Warnings
    ){
        cout << "[WARN] " << msg << endl;
    }
}

void Sys::printf_err(string msg){
    if(
        Sys::debugLevel == All      || 
        Sys::debugLevel == Warnings ||
        Sys::debugLevel == Errors
    ){
        cout << "[ERROR] " << msg << endl;
    }
}

void Sys::printf_err(int error_code){
    if(
        Sys::debugLevel == All      || 
        Sys::debugLevel == Warnings ||
        Sys::debugLevel == Errors
    ){
        cout << "[ERROR] " << errorMap[error_code] << endl;
    }
}
