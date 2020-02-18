#pragma once
#include "motor.h"

class Windlass : public Motor{
    private:
        uint8_t _pin_chain_encA;
        uint8_t _pin_chain_encB;
        int16_t _chain_counter;
        int16_t _chain_length;
        float _link_length;
        bool _chain_encA_prev;
        bool _chain_encB_prev;
        bool _force_move;
        bool _checkLimits(Move direction);
        void _output(Move direction);
        void _decodeQuadrature();
        Move _hwinput();

    public:
        Windlass(const char* name, uint8_t pin_up_out, uint8_t pin_down_out, uint8_t pin_up_in, uint8_t pin_down_in, uint8_t pin_chain_encA, uint8_t pin_chain_encB);
        bool begin();
        void update();
        float getDepth() { return float(_chain_counter) * _link_length; }
        int16_t getChain() { return _chain_counter; }
        void setChain(int16_t counts) { _chain_counter = counts; }
        float getLinkLength() { return _link_length; }
        void setLinkLength(float length) { _link_length = length; }
        void setChainLength(float length) { _chain_length = length / _link_length; }
};