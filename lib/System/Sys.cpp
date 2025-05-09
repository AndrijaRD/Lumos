#include "./Sys.h"
#include "../Gui/gui.h"

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
    int status = SDL_Init(SDL_INIT_EVERYTHING);
    if(status == 0){
        cout << "[INIT] Subsystem Initialized..." << endl;
    } else {
        cout << "[FATAL] Failed to initialize subsystems!" << endl;
        return SYS_SDL_INIT_ERROR;
    }


    // CREATE WINDOW ------------------------------------------------------
    int flags = 0;
    if(isFullscreen) flags = SDL_WINDOW_FULLSCREEN;

    win = SDL_CreateWindow(
        windowTitle.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
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
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    r = TM::_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if(r){
        cout << "[INIT] Renderer created..." << endl;
    }
    else{
        cout << "[FATAL] Failed to create rederer!" << endl;
        return SYS_RENDERER_INIT_ERROR;
    }
    
    TM::_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

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
    if(status != 0){
        cout << "[FATAL] Failed to initialize fonts!" << endl;
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




void Sys::processPendingJobs(){
    // steal the queue under lock
    std::vector<weak_ptr<asyncData>> jobs;
    {
        std::lock_guard lk(pendingJobsMutex);
        jobs.swap(pendingJobs);
    }

    // execute each job immediately on the main thread
    for (auto& job : jobs) {
        auto sp = job.lock();
        if(!sp) continue;

        int code = 0xffffffff;

        switch(sp->pendingFunction){
            case LOAD_TEXTURE: {
                auto td   = std::any_cast<TextureData>( sp->params[0] );
                auto path = std::any_cast<std::string>( sp->params[1] );
                auto id   = std::any_cast<std::string>( sp->params[2] );

                // call your real implementation *on* the main thread:
                code = TM::loadTexture(td, path, id);
                break;
            }

            case CREATE_TEXT_TEXTURE: {
                auto td       = std::any_cast<TextureData>( sp->params[0] );
                auto text     = std::any_cast<string>(      sp->params[1] );
                auto fontSize = std::any_cast<int>(         sp->params[2] );
                auto color    = std::any_cast<SDL_Color>(   sp->params[3] );

                // call your real implementation *on* the main thread:
                code = TM::createTextTexture(td, text, fontSize, color);
                break;
            }
            
            case COPY_TEXTURE: {
                auto src = std::any_cast<const TextureData&>( sp->params[0] );
                auto dst = std::any_cast<TextureData&>( sp->params[1] );

                // call your real implementation *on* the main thread:
                code = TM::copyTexture(src, dst);
                break;
            }
            
            case RESIZE_TEXTURE: {
                auto td = std::any_cast<TextureData>(    sp->params[0] );
                auto newWidth  =     std::any_cast<int>( sp->params[1] );
                auto newHeight =     std::any_cast<int>( sp->params[2] );

                // call your real implementation *on* the main thread:
                code = TM::resizeTexture(td, newWidth, newHeight);
                break;
            }

            case EXPORT_TEXTURE: {
                auto path = std::any_cast<std::string>( sp->params[0] );
                auto td   = std::any_cast<TextureData>( sp->params[1] );

                // call your real implementation *on* the main thread:
                code = TM::exportTexture(path, td);
                break;
            }

            case CONVERT_TO_TEX: {
                any input   = sp->params[0];
                auto output = std::any_cast<TextureData&>( sp->params[1] );
                
                // Check what type is the input
                if(input.type() == typeid(const cv::Mat&)){

                    // call your real implementation *on* the main thread:
                    code = TM::convert_toTexture(
                        std::any_cast<const cv::Mat&>(input), 
                        output
                    );
                }

                break;
            }

            case CONVERT_TEX_TO: {
                auto input = std::any_cast<const TextureData&>(sp->params[0]);
                any output = sp->params[1];
                
                // Check what type is the output
                if(output.type() == typeid(cv::Mat&)){

                    // call your real implementation *on* the main thread:
                    code = TM::convert_textureTo(
                        input, 
                        std::any_cast<cv::Mat&>(output)
                    );
                }

                break;
            }

            default:
                break;  
        }

        // If the code hasnt been changed
        // meaning some bad variables have been passed as the job
        if(code == (int)0xffffffff){
            Sys::printf_warn(
                "ProcessPendingJobs(): got 0xffffffff code;" 
                "job->pendingFunction = " + 
                to_string(sp->pendingFunction)
            );
        }

        // mark it done and wake the waiter
        {
            std::lock_guard lk(pendingJobsMutex);
            sp->result = code;
            sp->done   = true;
        }
        pendingJobsCV.notify_all();
    }
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

    Sys::processPendingJobs();

    // Calculate the delta Time --------------------------------------------------------------------------------------
    frameStart = SDL_GetTicks();

    // Avoid calculation in the first frame
    if (previousFrameStart != 0) 
        deltaTime = (frameStart - previousFrameStart);
    
    previousFrameStart = frameStart;



    // Clear the screen ----------------------------------------------------------------------------------------------
    TM::_SetRenderDrawColor(Sys::r, clearColor);
    TM::_RenderClear(Sys::r);


    // Keyboard Focus
    if(Keyboard::pendingUnFocus) Keyboard::focused = false;
    if(Keyboard::pendingFocus) Keyboard::focused = true;
    Keyboard::pendingFocus = false;
    Keyboard::pendingUnFocus = false;


    // GET NEW FRAME VALUES -------------------------------------------------------------------------------------------
    SDL_GetWindowSize(Sys::win, &Sys::wWidth, &Sys::wHeight);               // Getting window width and height
    Uint32 mouseState = SDL_GetMouseState(&Mouse::pos.x, &Mouse::pos.y);    // Getting mouse states and position

    // Calculating the new frame mouse status, is it down
    bool isMouseDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

    // Check to see if this is the first frame that mouse is down
    // If last frame Mouse::down was false and now isMouseDown is true
    // then set mouseDownStartFrame to current Frame
    if(!Mouse::down && isMouseDown) Mouse::mouseDownStartFrame = Sys::frameCounter;
    
    // If the mouse was clicked last frame (Mouse::down) but now its not
    // that means that it was released this frame, making it a compleat "click"
    Mouse::clicked = false; // Firstly reset it
    // Check if the release of the mouse is still considered a click, 
    // by checking the duretion that the mouse was held down
    int downDuration = frameCounter - Mouse::mouseDownStartFrame;
    bool timer = (downDuration <= Mouse::clickDuration);
    if(Mouse::down && !isMouseDown && timer){
        Mouse::clicked = true;
    }

    // Now update the Mouse::down
    Mouse::down = isMouseDown;

    // HANDLE EVENTS --------------------------------------------------------------------------------------------------
    SDL_Event event;
    Keyboard::text.clear();
    Keyboard::keyUp = 0;
    Keyboard::keyDown = 0;

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            isRunning = false;
        }

        if(event.type == SDL_KEYUP){
            Keyboard::keyUp = event.key.keysym.sym;
        }

        if(event.type == SDL_KEYDOWN){
            Keyboard::keyDown = event.key.keysym.sym;
        }

        if(event.type == SDL_TEXTINPUT){
            Keyboard::text += event.text.text;
        }

        // Scrolling
        if(event.type == SDL_MOUSEWHEEL){

            // Container updating
            for(auto& state : GUI::containerStates){

                if(state.second.lastActiveFrame + 1 == getCurrentFrame()){
                    if(Mouse::isHovering(state.second.dRect)){
                        state.second.scrollOffset -= event.wheel.y * state.second.scrollSpeed;
                        
                        int maxScroll = state.second.contentHeight - state.second.dRect.h;
                        if(maxScroll < 0) maxScroll = 0;  // if content is smaller than container, no scrolling
                        state.second.scrollOffset = clamp(state.second.scrollOffset, 0, maxScroll);
                    }
                }
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
    TM::_RenderPresent(Sys::r);


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
    TM::_DestroyRenderer(r);
    TTF_Quit();
    IMG_Quit();
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

// MOUSE ----------------------------------------------------------------------------

SDL_Point Sys::Mouse::getPos(bool containerRelative) { 

    // Check if there are any container to take into account
    if(containerRelative && !GUI::activeContainer.empty()){
        auto container = GUI::getContainerState(GUI::activeContainer);
        SDL_Point relativePoint = pos;

        relativePoint.x -= container->dRect.x;
        relativePoint.y = pos.y - container->dRect.y + container->scrollOffset;

        return relativePoint;
    }

    return pos;
}

bool Sys::Mouse::isClicked() { return clicked; }

bool Sys::Mouse::isDown() { return down; }

bool Sys::Mouse::isHovering(const SDL_Rect& rect, bool containerRelative) { 
    SDL_Point relativePosition = getPos(containerRelative);

    return (
        relativePosition.x >= rect.x && 
        relativePosition.x <= rect.x + rect.w &&
        relativePosition.y >= rect.y && 
        relativePosition.y <= rect.y + rect.h
    ); 
}

SDL_Keycode Sys::Keyboard::getKeyUp() { return keyUp; }
SDL_Keycode Sys::Keyboard::getKeyDown() { return keyDown; }
string Sys::Keyboard::getText() { return text; }

bool Sys::Keyboard::isFocused() { return focused; }
void Sys::Keyboard::focus() { pendingFocus = true; }
void Sys::Keyboard::unfocus() { pendingUnFocus = true; }



bool isPointInRect(SDL_Point point, SDL_Rect rect){
    return (
        point.x >= rect.x && 
        point.x <= rect.x + rect.w &&
        point.y >= rect.y && 
        point.y <= rect.y + rect.h
    );
}

bool isAbsolutePointInContainerRect(SDL_Point point, SDL_Rect rect){
    // Check if there are any container to take into account
    if(!GUI::activeContainer.empty()){
        auto container = GUI::getContainerState(GUI::activeContainer);
        
        rect.x += container->dRect.x;
        rect.y = rect.y - container->scrollOffset + container->dRect.y;
    }

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



