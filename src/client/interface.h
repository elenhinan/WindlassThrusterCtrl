#pragma once
#include <Arduino.h>
#include <u8g2lib.h>
#include "config.h"
#include "remote.h"
#include "icons.h"
#include "battery.h"

enum InterfaceState : uint8_t {STATUS, THRUSTER, WINDLASS, WINDLASS_CALIBRATE, nInterfaceState};

enum Buttons : uint8_t {
    BTN_NONE = 0,
    BTN_LEFT = 1<<0,
    BTN_RIGHT = 1<<1,
    BTN_UP = 1<<2,
    BTN_DOWN = 1<<3,
    BTN_OK = 1<<4
};

class InterfaceClass {
    private:
        uint8_t _pin_btn_left;
        uint8_t _pin_btn_right;
        uint8_t _pin_btn_up;
        uint8_t _pin_btn_down;
        uint8_t _pin_btn_ok;
        uint8_t _buttons;
        uint8_t _buttons_prev;
        long signed _buttons_time;
        U8G2* _display;
        unsigned long _last_btn_timestamp;
        InterfaceState _state;
        void _drawButtons();
        void _drawBattery();
        void _drawSignal();
        void _dispStatus();
        void _dispThruster();
        void _dispWindlass();
        void _dispWindlassCalibrate();    
        bool _checkBtn(uint8_t btn_combo, long unsigned time);
        void _updateState();
        void _printCommand(Move cmd);

    public:
        InterfaceClass();
        void setPins(uint8_t left, uint8_t right, uint8_t up, uint8_t down, uint8_t ok);
        void setDisplay(U8G2 *display) { _display = display; }
        void setState(InterfaceState newstate) { _state = newstate; }
        bool begin();
        void update();
        void display();
        unsigned long getIdle() { return millis() - _last_btn_timestamp; }
        void resetIdle() { _last_btn_timestamp = millis(); }
};

extern InterfaceClass interface;