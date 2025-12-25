#include "SM.h"
#include "../FontManager/FM.h"
#include "../Input/Input.h"
#include "../TextureManager/TM.h" // Needed for TM::rast initialization if we keep it there

namespace Lumos {

// DEFINE STATIC MEMBERS
unordered_map<int, string> Sys::errorMap = {
    {NO_ERROR, "NO_ERROR"},
    {SYS_SDL_INIT_ERROR, "SYS_SDL_INIT_ERROR"},
    {SYS_WINDOW_INIT_ERROR, "SYS_WINDOW_INIT_ERROR"},
    {SYS_RENDERER_INIT_ERROR, "SYS_RENDERER_INIT_ERROR"},
    // Add more as needed or keep it simple
    {UNKNOWN_ERROR, "UNKNOWN_ERROR"}};

/** System Init
 *
 * Sets up the entier SDL system.
 * Creates Window.
 * Creates Renderer.
 *
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::initWindow(const string &winTitle, const bool &_fullscreen,
                    const int &windowWidth, const int &windowHeight) {
  // SET THE MAIN THREAD ID
  _mainThreadId = this_thread::get_id();

  windowTitle = winTitle;
  isFullscreen = _fullscreen;
  wWidth = windowWidth;
  wHeight = windowHeight;

  // SDL INIT
  auto flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO;

  if (SDL_Init(flags)) {
    printf_info("[INIT] Subsystem Initialized...");
  } else {
    printf_err("[FATAL] Failed to initialize subsystems!");
    printf_err(string("Message: ") + SDL_GetError());
    return SYS_SDL_INIT_ERROR;
  }

  // CREATE WINDOW
  auto winFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  if (isFullscreen)
    winFlags |= SDL_WINDOW_FULLSCREEN;

  win = SDL_CreateWindow(windowTitle.c_str(), wWidth, wHeight, winFlags);
  if (win) {
    printf_info("[INIT] Window created...");
  } else {
    printf_err("[FATAL] Failed to create window!");
    return SYS_WINDOW_INIT_ERROR;
  }

  // CREATE RENDERER
  // SDL3 uses CreateRenderer without index usually, or different arguments?
  // Checking doc or existing code: SDL_CreateRenderer(win, NULL) or similar.
  // Old code had: r = SDL_CreateRenderer(win, backends.c_str());
  // For SDL3: SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, const char*
  // name) name can be NULL for default.
  const char *backend = nullptr; // Let SDL choose
  r = SDL_CreateRenderer(win, backend);

  if (!r) {
    printf_err("[FATAL] Failed to create rederer!");
    return SYS_RENDERER_INIT_ERROR;
  }

  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

  printf_info("[INIT] Renderer created...");
  printf_info(string("\tVideoSubsystem: ") + SDL_GetCurrentVideoDriver());
  printf_info(string("\tRender backend: ") + SDL_GetRendererName(r));

  // TM::rast = nsvgCreateRasterizer(); // TODO: Add this when TextureManager is
  // ready

  return NO_ERROR;
}

int Sys::handleEvents() {
  int error = NO_ERROR;

  // Calculate Delta Time
  frameStart = SDL_GetTicks();
  if (previousFrameStart != 0)
    deltaTime = (frameStart - previousFrameStart);
  previousFrameStart = frameStart;

  // Clear Screen
  SDL_SetRenderDrawColor(r, clearColor.r, clearColor.g, clearColor.b,
                         clearColor.a);
  SDL_RenderClear(r);

  // Get Window Size
  SDL_GetWindowSize(win, &wWidth, &wHeight);

  // Update Inputs
  // Assuming Input::Keyboard::Update() handles polling internally or we pass
  // events? Wait, if Input::Keyboard uses SDL_GetKeyboardState, we still need
  // to pump events. SDL_PollEvent pumps events implicitly.

  // We MUST pump events for the window responsivness.
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      isRunning = false;
    }
    Input::Keyboard::HandleEvent(event);
    // If Input manager relies on events, we'd pass them here.
    // If it uses polling state, we just pump events.
  }
  Input::Keyboard::Update();
  Input::Mouse::Update();

  return error;
}

int Sys::presentFrame() {
  // Present
  SDL_RenderPresent(r);

  // Frame Delay
  Uint64 currentFrameDuration = SDL_GetTicks() - frameStart;
  if (FPS > 0 && static_cast<Uint64>(1000 / FPS) > currentFrameDuration) {
    SDL_Delay((1000 / FPS) - currentFrameDuration);
  } else {
    // FPS too high or unlimited
  }

  // Update Frame Counter
  frameCounter++;
  if (frameCounter >= 0xFFFFFFFF)
    frameCounter = 0;

  return NO_ERROR;
}

int Sys::cleanup() {
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(r);
  TTF_Quit();
  SDL_Quit();
  printf_info("Game Finished.");
  return NO_ERROR;
}

// Setters / Getters
int Sys::getFPS() { return FPS; }
void Sys::setFPS(int newFPS) { FPS = std::clamp(newFPS, 20, 144); }
int Sys::getCurrentFrame() { return frameCounter; }
int Sys::getOS() { return detectedOS; }
void Sys::setClearColor(const SDL_Color &color) { clearColor = color; }

string Sys::checkError(int error) {
  if (errorMap.count(error))
    return errorMap[error];
  return "Unknown Error";
}

bool Sys::isMainThread() { return this_thread::get_id() == _mainThreadId; }

// Logging
void Sys::printf_info(string msg) {
  if (debugLevel == All)
    cout << "[INFO] " << msg << endl;
}

void Sys::printf_warn(string msg) {
  if (debugLevel == All || debugLevel == Warnings)
    cout << "[WARN] " << msg << endl;
}

void Sys::printf_err(string msg) {
  if (debugLevel != Silent)
    cout << "[ERROR] " << msg << endl;
}

void Sys::printf_err(int error_code) {
  if (debugLevel != Silent)
    cout << "[ERROR] " << checkError(error_code) << endl;
}

TTF_Font *Sys::getFont(int fontSize) { return FM::get(fontSize); }

} // namespace Lumos
