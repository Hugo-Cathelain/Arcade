///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/shared/WiiMote.hpp"
#include <string>
#include <iostream>
#include <cassert>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
wiimote** WiiMote::mHandle;
std::array<WiiMote::Item, WiiMote::maxWiiMotes> WiiMote::mWiiMotes;

///////////////////////////////////////////////////////////////////////////////
void WiiMote::Initialize(void)
{
    freopen("/dev/null", "w", stdout);

    if (mHandle) {
        wiiuse_cleanup(mHandle, WiiMote::maxWiiMotes);
    }

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        mWiiMotes[i].connected = false;
        mWiiMotes[i].accelerometer = Accelerometer();
        mWiiMotes[i].buttons.fill(false);
        mWiiMotes[i].justButtons.fill(false);
    }

    mHandle = wiiuse_init(WiiMote::maxWiiMotes);
    freopen("/dev/tty", "w", stdout);

    if (!mHandle) {
        std::cerr << "Failed to initialize WiiMote" << std::endl;
        return;
    }

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        wiiuse_motion_sensing(mHandle[i], 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
void WiiMote::Cleanup(void)
{
    freopen("/dev/null", "w", stderr);
    if (mHandle) {
        wiiuse_cleanup(mHandle, WiiMote::maxWiiMotes);
    }
    freopen("/dev/tty", "w", stderr);

    mHandle = nullptr;

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        mWiiMotes[i].connected = false;
        mWiiMotes[i].accelerometer = Accelerometer();
        mWiiMotes[i].buttons.fill(false);
        mWiiMotes[i].justButtons.fill(false);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool WiiMote::Find(void)
{
    assert(mHandle && "WiiMote not initialized");
    bool found = wiiuse_find(mHandle, WiiMote::maxWiiMotes, 5);

    if (!found) {
        std::cerr << "Failed to find WiiMote" << std::endl;
    }
    return (found);
}

///////////////////////////////////////////////////////////////////////////////
bool WiiMote::Connect(void)
{
    assert(mHandle && "WiiMote not initialized");
    bool connected = wiiuse_connect(mHandle, WiiMote::maxWiiMotes);

    if (!connected) {
        std::cerr << "Failed to connect to any wiimote." << std::endl;
        return (false);
    }

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        mWiiMotes[i].connected = WIIMOTE_IS_CONNECTED(mHandle[i]);
    }

    wiiuse_set_leds(mHandle[0], WIIMOTE_LED_1);
    wiiuse_set_leds(mHandle[1], WIIMOTE_LED_2);
    wiiuse_set_leds(mHandle[2], WIIMOTE_LED_3);
    wiiuse_set_leds(mHandle[3], WIIMOTE_LED_4);

    wiiuse_rumble(mHandle[0], 1);
    wiiuse_rumble(mHandle[1], 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    wiiuse_rumble(mHandle[0], 0);
    wiiuse_rumble(mHandle[1], 0);

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void WiiMote::Update(void)
{
    assert(mHandle && "WiiMote not initialized");
    bool atLeastOneConnected = false;

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        mWiiMotes[i].connected = WIIMOTE_IS_CONNECTED(mHandle[i]);

        if (mWiiMotes[i].connected) {
            atLeastOneConnected = true;
        }
    }

    if (!atLeastOneConnected) {
        return;
    }

    if (!wiiuse_poll(mHandle, WiiMote::maxWiiMotes)) {
        return;
    }

    for (unsigned int i = 0; i < WiiMote::maxWiiMotes; i++) {
        wiiuse_motion_sensing(mHandle[i], 1);

        switch (mHandle[i]->event) {
            case WIIUSE_EVENT:
            {
                struct wiimote_t* wm = mHandle[i];

                mWiiMotes[i].buttons[Button::A]     = IS_PRESSED(wm, WIIMOTE_BUTTON_A);
                mWiiMotes[i].buttons[Button::B]     = IS_PRESSED(wm, WIIMOTE_BUTTON_B);
                mWiiMotes[i].buttons[Button::Up]    = IS_PRESSED(wm, WIIMOTE_BUTTON_UP);
                mWiiMotes[i].buttons[Button::Down]  = IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN);
                mWiiMotes[i].buttons[Button::Left]  = IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT);
                mWiiMotes[i].buttons[Button::Right] = IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT);
                mWiiMotes[i].buttons[Button::Minus] = IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS);
                mWiiMotes[i].buttons[Button::Plus]  = IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS);
                mWiiMotes[i].buttons[Button::One]   = IS_PRESSED(wm, WIIMOTE_BUTTON_ONE);
                mWiiMotes[i].buttons[Button::Two]   = IS_PRESSED(wm, WIIMOTE_BUTTON_TWO);
                mWiiMotes[i].buttons[Button::Home]  = IS_PRESSED(wm, WIIMOTE_BUTTON_HOME);

                mWiiMotes[i].justButtons[Button::A]     = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_A);
                mWiiMotes[i].justButtons[Button::B]     = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B);
                mWiiMotes[i].justButtons[Button::Up]    = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP);
                mWiiMotes[i].justButtons[Button::Down]  = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN);
                mWiiMotes[i].justButtons[Button::Left]  = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_LEFT);
                mWiiMotes[i].justButtons[Button::Right] = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_RIGHT);
                mWiiMotes[i].justButtons[Button::Minus] = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS);
                mWiiMotes[i].justButtons[Button::Plus]  = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS);
                mWiiMotes[i].justButtons[Button::One]   = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_ONE);
                mWiiMotes[i].justButtons[Button::Two]   = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_TWO);
                mWiiMotes[i].justButtons[Button::Home]  = IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_HOME);

                if (WIIUSE_USING_ACC(wm)) {
                    mWiiMotes[i].accelerometer.roll  = wm->orient.roll;
                    mWiiMotes[i].accelerometer.pitch = wm->orient.pitch;
                    mWiiMotes[i].accelerometer.yaw   = wm->orient.yaw;
                }

                break;
            }
            case WIIUSE_DISCONNECT:
            case WIIUSE_UNEXPECTED_DISCONNECT:
            {
                mWiiMotes[i].connected = false;
                break;
            }
            default:
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void WiiMote::Rumble(unsigned int index, bool on)
{
    assert(index < WiiMote::maxWiiMotes && "WiiMote index out of range");
    assert(mHandle && "WiiMote not initialized");
    wiiuse_rumble(mHandle[index], on);
}

///////////////////////////////////////////////////////////////////////////////
bool WiiMote::IsConnected(unsigned int index)
{
    assert(index < WiiMote::maxWiiMotes && "WiiMote index out of range");
    assert(mHandle && "WiiMote not initialized");
    return (mWiiMotes[index].connected);
}

///////////////////////////////////////////////////////////////////////////////
bool WiiMote::IsButtonPressed(unsigned int index, Button button)
{
    assert(index < WiiMote::maxWiiMotes && "WiiMote index out of range");
    assert(mHandle && "WiiMote not initialized");
    return (mWiiMotes[index].buttons[button]);
}

///////////////////////////////////////////////////////////////////////////////
bool WiiMote::IsButtonJustPressed(unsigned int index, Button button)
{
    assert(index < WiiMote::maxWiiMotes && "WiiMote index out of range");
    assert(mHandle && "WiiMote not initialized");
    return (mWiiMotes[index].justButtons[button]);
}

///////////////////////////////////////////////////////////////////////////////
const WiiMote::Accelerometer& WiiMote::GetAccelerometer(unsigned int index)
{
    assert(index < WiiMote::maxWiiMotes && "WiiMote index out of range");
    assert(mHandle && "WiiMote not initialized");
    return (mWiiMotes[index].accelerometer);
}

} // namespace Arc
