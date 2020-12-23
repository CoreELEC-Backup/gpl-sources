#include "wx/sdljoy.h"
#include "SDL.h"
#include <SDL_joystick.h>
#include <wx/window.h>

DEFINE_EVENT_TYPE(wxEVT_SDLJOY)

struct wxSDLJoyState {
    SDL_Joystick* dev;
    int nax, nhat, nbut;
    short* curval;
    short* initial_val;
    bool is_valid;
    ~wxSDLJoyState()
    {
        if (dev)
            SDL_JoystickClose(dev);
        if (curval)
            delete[] curval;
        if (initial_val)
            delete[] initial_val;
    }
    wxSDLJoyState()
    {
	dev = NULL;
	nax = nhat = nbut = 0;
	curval = NULL;
        initial_val = NULL;
        is_valid = true;
    }
};

wxSDLJoy::wxSDLJoy(bool analog)
    : wxTimer()
    , digital(!analog)
    , joystate(0)
    , evthandler(0)
    , nosticks(true)
{
    // Start up joystick if not already started
    // FIXME: check for errors
    SDL_Init(SDL_INIT_JOYSTICK);
    // but we'll have to manage it manually
    SDL_JoystickEventState(SDL_IGNORE);
    // now query joystick config and open joysticks
    // there is no way to reread this later (e.g. if joystick plugged in),
    // since SDL won't
    njoy = SDL_NumJoysticks();

    if (!njoy)
        return;

    joystate = new wxSDLJoyState[njoy];

    for (int i = 0; i < njoy; i++) {
        SDL_Joystick* dev = joystate[i].dev = SDL_JoystickOpen(i);

        int nctrl = 0, axes = 0, hats = 0, buttons = 0;

        axes    = SDL_JoystickNumAxes(dev);
        hats    = SDL_JoystickNumHats(dev);
        buttons = SDL_JoystickNumButtons(dev);

        if (buttons <= 0 && axes <= 0 && hats <= 0) {
            joystate[i].is_valid = false;

            SDL_JoystickClose(dev);
            joystate[i].dev = NULL;

            continue;
        }

        nctrl += joystate[i].nax  = axes    < 0 ? 0 : axes;
        nctrl += joystate[i].nhat = hats    < 0 ? 0 : hats;
        nctrl += joystate[i].nbut = buttons < 0 ? 0 : buttons;

        joystate[i].curval = new short[nctrl]{};

        // set initial values array
	// see below for 360 trigger handling
#if SDL_VERSION_ATLEAST(2, 0, 6)
        joystate[i].initial_val = new short[nctrl]{};

        for (int j = 0; j < joystate[i].nax; j++) {
            int16_t initial_state = 0;
            SDL_JoystickGetAxisInitialState(dev, j, &initial_state);
            joystate[i].initial_val[j] = initial_state;

            // curval is 0 which is what initial state maps to ATM
        }
#endif
    }
}

wxSDLJoy::~wxSDLJoy()
{
    delete[] joystate;
    // SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

wxEvtHandler* wxSDLJoy::Attach(wxEvtHandler* handler)
{
    wxEvtHandler* prev = evthandler;
    evthandler = handler;
    return prev;
}

void wxSDLJoy::Add(int joy)
{
    if (joy >= njoy || !njoy)
        return;

    if (joy < 0) {
        for (int i = 0; i < njoy; i++)
            Add(i);

        return;
    }

    if (!joystate[joy].dev)
        joystate[joy].dev = SDL_JoystickOpen(joy);

    if (nosticks && joystate[joy].dev) {
        Start(50);
        nosticks = false;
    }
}

void wxSDLJoy::Remove(int joy)
{
    if (joy >= njoy || !njoy)
        return;

    if (joy < 0) {
        for (int i = 0; i < njoy; i++)
            if (joystate[i].dev) {
                SDL_JoystickClose(joystate[i].dev);
                joystate[i].dev = NULL;
            }

        Stop();
        nosticks = true;
        return;
    }

    if (!joystate[joy].dev)
        return;

    SDL_JoystickClose(joystate[joy].dev);
    joystate[joy].dev = NULL;

    for (int i = 0; i < njoy; i++)
        if (joystate[i].dev)
            return;

    Stop();
    nosticks = true;
}

void wxSDLJoy::Notify()
{
    if (nosticks)
        return;

    SDL_JoystickUpdate();
    wxEvtHandler* handler = evthandler ? evthandler : wxWindow::FindFocus();

    for (int i = 0; i < njoy; i++) {
        if (!joystate[i].is_valid) continue;

        SDL_Joystick* dev = joystate[i].dev;

        if (dev) {
            int nax = joystate[i].nax, nhat = joystate[i].nhat,
                nbut = joystate[i].nbut;
            short val;

            for (int j = 0; j < nax; j++) {
                val = SDL_JoystickGetAxis(dev, j);

                if (digital) {
                    if (val > 0x3fff)
                        val = 0x7fff;
                    else if (val <= -0x3fff)
                        val = -0x7fff;
                    else
                        val = 0;
                }

                // This is for the 360 and similar triggers which return a
                // value on initial state of the trigger axis. This hack may be
                // insufficient, we may need to expand the joy event API to
                // support initial values.
                if (joystate[i].initial_val && val == joystate[i].initial_val[j])
                    val = 0;

                if (handler && val != joystate[i].curval[j]) {
                    wxSDLJoyEvent ev(wxEVT_SDLJOY, GetId());
                    ev.joy = i;
                    ev.ctrl_type = WXSDLJOY_AXIS;
                    ev.ctrl_idx = j;
                    ev.ctrl_val = val;
                    ev.prev_val = joystate[i].curval[j];
                    handler->ProcessEvent(ev);
                }

                joystate[i].curval[j] = val;
            }

            for (int j = 0; j < nhat; j++) {
                val = SDL_JoystickGetHat(dev, j);

                if (handler && val != joystate[i].curval[nax + j]) {
                    wxSDLJoyEvent ev(wxEVT_SDLJOY, GetId());
                    ev.joy = i;
                    ev.ctrl_type = WXSDLJOY_HAT;
                    ev.ctrl_idx = j;
                    ev.ctrl_val = val;
                    ev.prev_val = joystate[i].curval[nax + j];
                    handler->ProcessEvent(ev);
                }

                joystate[i].curval[nax + j] = val;
            }

            for (int j = 0; j < nbut; j++) {
                val = SDL_JoystickGetButton(dev, j);

                if (handler && val != joystate[i].curval[nax + nhat + j]) {
                    wxSDLJoyEvent ev(wxEVT_SDLJOY, GetId());
                    ev.joy = i;
                    ev.ctrl_type = WXSDLJOY_BUTTON;
                    ev.ctrl_idx = j;
                    ev.ctrl_val = val;
                    ev.prev_val = joystate[i].curval[nax + nhat + j];
                    handler->ProcessEvent(ev);
                }

                joystate[i].curval[nax + nhat + j] = val;
            }
        }
    }
}
