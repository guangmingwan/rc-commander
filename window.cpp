#include <iostream>
#include "window.h"
#include "def.h"
#include "sdlutils.h"

#define KEYHOLD_TIMER_FIRST 6
#define KEYHOLD_TIMER 2

extern SDL_Surface *ScreenSurface;

CWindow::CWindow(void) : m_timer(0),
                         m_lastPressed(SDLK_0),
                         m_retVal(0)
{
    // Add window to the lists for render
    Globals::g_windows.push_back(this);
}

CWindow::~CWindow(void)
{
    // Remove last window
    Globals::g_windows.pop_back();
}

const int CWindow::execute(void)
{
    m_retVal = 0;
    Uint32 l_time(0);
    SDL_Event l_event;
    bool l_loop(true);
    bool l_render(true);
    // Main loop
    while (l_loop)
    {
        l_time = SDL_GetTicks();
        // Handle key press
        while (SDL_PollEvent(&l_event))
        {
            if (l_event.type == SDL_KEYDOWN ||
                l_event.type == SDL_JOYAXISMOTION ||
                l_event.type == SDL_JOYHATMOTION ||
                l_event.type == SDL_JOYBUTTONDOWN)
            {
                l_render = this->keyPress(l_event);
                if (m_retVal)
                    l_loop = false;
            }
        }
        // Handle key hold
        if (l_loop)
            l_render = this->keyHold() || l_render;
        // Render if necessary
        if (l_render && l_loop)
        {
            SDL_utils::renderAll();
            // Flip twice to avoid graphical glitch on Dingoo
            //SDL_Flip(Globals::g_screen);
            //SDL_Flip(Globals::g_screen);
            SDL_SoftStretch(Globals::g_screen, NULL, ScreenSurface, NULL);
            SDL_Flip(ScreenSurface);
            l_render = false;
            INHIBIT(std::cout << "Render time: " << SDL_GetTicks() - l_time << "ms" << std::endl;)
        }
        // Cap the framerate
        l_time = MS_PER_FRAME - (SDL_GetTicks() - l_time);
        if (l_time <= MS_PER_FRAME)
            SDL_Delay(l_time);
    }
    return m_retVal;
}

const bool CWindow::keyPress(const SDL_Event &p_event)
{
    // Reset timer if running
    if (m_timer)
        m_timer = 0;
    SDL_Event *event = (SDL_Event *)&p_event;
    switch (event->type)
    {
    case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
        if ((event->jaxis.value < -3200) || (event->jaxis.value > 3200))
        {
            /* code goes here */
            if (event->jaxis.axis == 0)
            {
                /* Left-right movement code goes here */
                printf(" Left-right\n");
                if (event->jaxis.value < -3200)
                {
                    event->key.keysym.sym = SDLK_LEFT;
                }
                else if (event->jaxis.value > 3200)
                {
                    event->key.keysym.sym = SDLK_RIGHT;
                }
            }

            if (event->jaxis.axis == 1)
            {
                /* Up-Down movement code goes here */
                printf("Up-Down\n");
                if (event->jaxis.value < -3200)
                {
                    event->key.keysym.sym = SDLK_UP;
                }
                else if (event->jaxis.value > 3200)
                {
                    event->key.keysym.sym = SDLK_DOWN;
                }
            }
        }
        else
        {
            //event->key.keysym.sym = -1;
        }
        break;

    case SDL_JOYHATMOTION:
        if (event->jhat.value == 0)
        {
            //event->key.keysym.sym = -1;
        }
        if (event->jhat.value & SDL_HAT_UP)
        {
            /* Do up stuff here */
            event->key.keysym.sym = SDLK_UP;
        }

        else if (event->jhat.value & SDL_HAT_LEFT)
        {
            /* Do left stuff here */
            event->key.keysym.sym = SDLK_LEFT;
        }

        else if (event->jhat.value & SDL_HAT_RIGHT)
        {
            /* Do right and down together stuff here */
            event->key.keysym.sym = SDLK_RIGHT;
        }
        else if (event->jhat.value & SDL_HAT_DOWN)
        {
            /* Do right and down together stuff here */
            event->key.keysym.sym = SDLK_DOWN;
        }
        else if (event->jhat.value & SDL_HAT_CENTERED)
        {
            //event->key.keysym.sym = -1;
        }

        break;
    case SDL_JOYBUTTONDOWN:             /* Handle Joystick Button Presses */
        if (event->jbutton.button == 0) //x
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_a;
        }
        if (event->jbutton.button == 1) //a
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_RETURN;
        }
        if (event->jbutton.button == 2) //b
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_BACKSPACE;
        }
        if (event->jbutton.button == 3) //y
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_q;
        }
        if (event->jbutton.button == 4) //select
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_INSERT;
        }
        if (event->jbutton.button == 5) //start
        {
            /* code goes here */
            event->key.keysym.sym = SDLK_w;
        }
        printf("event->jbutton.button %d", event->jbutton.button);
        break;
    case SDL_JOYBUTTONUP:
        //event->key.keysym.sym = -1;
        break;
    case SDL_MOUSEMOTION:
        break;
    default:
        break;
    }

    m_lastPressed = event->key.keysym.sym;
    return false;
}

const bool CWindow::keyHold(void)
{
    // Default behavior
    return false;
}

const bool CWindow::tick(const Uint8 p_held)
{
    bool l_ret(false);
    if (p_held)
    {
        if (m_timer)
        {
            --m_timer;
            if (!m_timer)
            {
                // Trigger!
                l_ret = true;
                // Timer continues
                m_timer = KEYHOLD_TIMER;
            }
        }
        else
        {
            // Start timer
            m_timer = KEYHOLD_TIMER_FIRST;
        }
    }
    else
    {
        // Stop timer if running
        if (m_timer)
            m_timer = 0;
    }
    return l_ret;
}

const int CWindow::getReturnValue(void) const
{
    return m_retVal;
}

const bool CWindow::isFullScreen(void) const
{
    // Default behavior
    return false;
}
