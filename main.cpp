#ifndef PLATFORM_DINGOO
#include <cstdlib>
#endif
#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "def.h"
#include "sdlutils.h"
#include "resourceManager.h"
#include "commander.h"

// Globals
SDL_Surface *ScreenSurface;
SDL_Surface *Globals::g_screen = NULL;
static SDL_Joystick *g_pJoy = NULL;
const SDL_Color Globals::g_colorTextNormal = {COLOR_TEXT_NORMAL};
const SDL_Color Globals::g_colorTextTitle = {COLOR_TEXT_TITLE};
const SDL_Color Globals::g_colorTextDir = {COLOR_TEXT_DIR};
const SDL_Color Globals::g_colorTextSelected = {COLOR_TEXT_SELECTED};
std::vector<CWindow *> Globals::g_windows;

int main(int argc, char **argv)
{
    // Avoid crash due to the absence of mouse
    {
        char l_s[] = "SDL_NOMOUSE=1";
        putenv(l_s);
    }

    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Screen
    ScreenSurface = SDL_SetVideoMode(320, 480, SCREEN_BPP, SURFACE_FLAGS);
    Globals::g_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
    if (Globals::g_screen == NULL)
    {
        std::cerr << "SDL_SetVideoMode failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Hide cursor
    SDL_ShowCursor(SDL_DISABLE);

    if (SDL_NumJoysticks() > 0)
    {
        std::cout(printf("Found Joysticks %d\n", SDL_NumJoysticks()));
        JY_Debug("The names of the joysticks are:\n");

        for (i = 0; i < SDL_NumJoysticks(); i++)
        {
            std::cout(printf(("    %s\n", SDL_JoystickName(i)));
        }
        SDL_JoystickEventState(SDL_ENABLE);
        g_pJoy = SDL_JoystickOpen(0);

        if (g_pJoy != NULL)
        {
            std::cout(printf(("    Open ok for %s\n", SDL_JoystickName(0)));
        }
    }
    else
    {
        std::cout(printf("no found joysticks\n"));
    }

    // Init font
    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create instances
    CResourceManager::instance();
#ifdef PLATFORM_DINGOO
    CCommander l_commander(PATH_DEFAULT, PATH_DEFAULT_RIGHT);
#else
    std::string l_path = getenv("HOME");
    l_path += "/Dev/DinguxCommander/test";
    CCommander l_commander(l_path, l_path);
#endif

    // Main loop
    l_commander.execute();

    //Quit
    SDL_utils::hastalavista();

    return 0;
}
