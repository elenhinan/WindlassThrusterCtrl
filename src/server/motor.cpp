#include "motor.h"

Motor::Motor(const char* name, uint8_t pin_positive_out, uint8_t pin_negative_out, uint8_t pin_positive_in, uint8_t pin_negative_in) :
    _pin_positive_out(pin_positive_out),
    _pin_negative_out(pin_negative_out),
    _pin_positive_in(pin_positive_in),
    _pin_negative_in(pin_negative_in),
    _enabled(false),
    _cooldown(0),
    _timeout(0),
    _current_direction(DIR_STOP),
    _cooldown_direction(DIR_STOP),
    _last_input_direction(DIR_STOP),
    _cooldown_timestamp(0),
    _last_timestamp(0)
{
    strcpy(_name, name);
}

bool Motor::begin() {
    // ensure outputs are low
    digitalWrite(_pin_positive_out, LOW);
    digitalWrite(_pin_negative_out, LOW);

    pinMode(_pin_positive_out, OUTPUT);
    pinMode(_pin_negative_out, OUTPUT);

    pinMode(_pin_positive_in, INPUT_PULLUP);
    pinMode(_pin_negative_in, INPUT_PULLUP);

    return true;
}

bool Motor::enable() {
    // sanity check for timeout and cooldown values before enabling
    _enabled = _timeout > 50 && _cooldown > 500;
    return _enabled;
}

void Motor::disable() {
    digitalWrite(_pin_positive_out, LOW);
    digitalWrite(_pin_negative_out, LOW);
   _enabled = false;
}

void Motor::_output(Move new_direction) {
    unsigned long now = millis();

    // if reversal within cooldown, then stop thruster
    if (!_enabled) {
        _current_direction = ERROR_DISABLED;
    }
    else if ((new_direction == -_cooldown_direction) && (now < _cooldown_timestamp + _cooldown)) {
        _current_direction = ERROR_COOL;
    }
    else {
        _current_direction = new_direction;
    }

    // update outputs
    digitalWrite(_pin_positive_out, _current_direction == DIR_POS);
    digitalWrite(_pin_negative_out, _current_direction == DIR_NEG);

    // save direction and timestamp for cooldown period
    if (_current_direction == DIR_POS || _current_direction == DIR_NEG) {
        _cooldown_timestamp = now;
        _cooldown_direction = _current_direction;
    }
    else if (_current_direction == DIR_STOP) {
    }

    // update timestamp
    _last_timestamp = now;
}

void Motor::input(Move direction) {
    // if valid command and no direct input is applied: pass cmd onto output stage
    if (direction >= -1 && direction <= 1 && _hwinput() == DIR_STOP) {
        _output(direction);
    }
}

Move Motor::_hwinput() {
    return (Move)(!digitalRead(_pin_positive_in) - !digitalRead(_pin_negative_in));
}

void Motor::update() {
    unsigned long now = millis();

    // if last action was aborted due to cooldown, but cooldown period passed, reset logic
    if (now > _cooldown_timestamp + _cooldown && _current_direction == ERROR_COOL) {
        _last_input_direction = _current_direction = DIR_STOP;
    }

    // read joystick input
    Move input_direction = _hwinput();
    if (input_direction != _last_input_direction) {
        _output(input_direction);
    }
    _last_input_direction = input_direction;

    // if no new (radio) command recieved within timeout, stop thruster
    //if ((_current_direction != DIR_STOP) && now > _last_timestamp + _timeout) {
    //    if(_debug) Serial.println("Timeout, aborting");
    //    _output(DIR_STOP);
    //}
}