#include "SDL3/SDL_video.h"
#include <bane/controllers/icontroller.hpp>
void IController::processInput(SDL_Window*, Uint32 button, bool pressed) {}
void IController::update(float) {}
void IController::processMouse(SDL_Window*, double, double) {}
void IController::processMouseInput(SDL_Window*, int, int, int) {}
void IController::Enable() {}
void IController::Disable() {}
