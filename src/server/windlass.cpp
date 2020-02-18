#include "windlass.h"

Windlass::Windlass(const char* name, uint8_t pin_up_out, uint8_t pin_down_out, uint8_t pin_up_in, uint8_t pin_down_in, uint8_t pin_chain_encA, uint8_t pin_chain_encB) :
    Motor(name, pin_up_out, pin_down_out, pin_up_in, pin_down_in),
    _pin_chain_encA(pin_chain_encA),
    _pin_chain_encB(pin_chain_encB),
    _chain_counter(0),
    _chain_length(50),
    _link_length(0.1),
    _force_move(false)
{
}

bool Windlass::begin() {
    Motor::begin();

    pinMode(_pin_chain_encA, INPUT_PULLUP);
    pinMode(_pin_chain_encB, INPUT_PULLUP);

    _chain_encA_prev = !digitalRead(_pin_chain_encA);
    _chain_encB_prev = !digitalRead(_pin_chain_encB);

    return true;
}

void Windlass::_decodeQuadrature() {
    // read encoder outputs
    bool chain_encA = !digitalRead(_pin_chain_encA);
    bool chain_encB = !digitalRead(_pin_chain_encB);

    // decode direction of turn
    int16_t decoded = 0;
    if (chain_encA != _chain_encA_prev)
        decoded += (chain_encA == chain_encB) ? 1 : -1;
    if (chain_encB != _chain_encB_prev)
        decoded += (chain_encA == chain_encB) ? -1 : 1;

    // change counter if necessary and update history
    _chain_counter += decoded;
    _chain_encA_prev = chain_encA;
    _chain_encB_prev = chain_encB;

    _checkLimits(_current_direction);
}

bool Windlass::_checkLimits(Move direction) {
    // abort movement if out of chain
    if (_force_move) {
        return true;
    }
    bool pos_endstop = _chain_counter <= 0 && direction == DIR_POS;
    bool neg_endstop = _chain_counter >= _chain_length && direction == DIR_NEG;
    if (pos_endstop || neg_endstop) {
        Motor::_output(ERROR_LIMIT);
        return false;
    } else {
        return true;
    }
}

Move Windlass::_hwinput() {
    // Hardware inputs overrule limits
    return (Move)(!digitalRead(_pin_positive_in)*2 - !digitalRead(_pin_negative_in)*2);
}

void Windlass::_output(Move direction) {
    if (direction == DIR_FORCE_POS) {
        direction = DIR_POS;
        _force_move = true;
    }
    else if (direction == DIR_FORCE_NEG) {
        direction = DIR_NEG;
        _force_move = true;
    } else {
        _force_move = false;
    }
    // do not allow to run past endstops
    if (_checkLimits(direction)) {
        Motor::_output(direction);
    }
}

void Windlass::update() {
    _decodeQuadrature();
    Motor::update();
}