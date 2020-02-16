#include "interface.h"

InterfaceClass::InterfaceClass() :
    _pin_btn_left(0),
    _pin_btn_right(0),
    _pin_btn_up(0),
    _pin_btn_down(0),
    _pin_btn_ok(0),
	_buttons(0),
    _buttons_prev(0),
    _buttons_time(0),
    _display(0),
    _last_btn_timestamp(0),
	_state(THRUSTER)
{
}

void InterfaceClass::setPins(uint8_t left, uint8_t right, uint8_t up, uint8_t down, uint8_t ok) {
    _pin_btn_left = left;
    _pin_btn_right = right;
    _pin_btn_up = up;
    _pin_btn_down = down;
    _pin_btn_ok = ok;
}

bool InterfaceClass::begin() {
    if(!_pin_btn_left || !_pin_btn_right || !_pin_btn_up || _pin_btn_down || _pin_btn_ok)
        return false;

    // setup hw pushbuttons
    pinMode(_pin_btn_left, INPUT_PULLUP);
    pinMode(_pin_btn_right, INPUT_PULLUP);
    pinMode(_pin_btn_up, INPUT_PULLUP);
    pinMode(_pin_btn_down, INPUT_PULLUP);
    pinMode(_pin_btn_ok, INPUT_PULLUP);

	return true;
}

void InterfaceClass::update() {
	_buttons = 0;
	_buttons |= digitalRead(_pin_btn_left) ? 0 : BTN_LEFT;
	_buttons |= digitalRead(_pin_btn_right) ? 0 : BTN_RIGHT;
	_buttons |= digitalRead(_pin_btn_up) ? 0 : BTN_UP;
	_buttons |= digitalRead(_pin_btn_down) ? 0 : BTN_DOWN;
	_buttons |= digitalRead(_pin_btn_ok) ? 0 : BTN_OK;

	// if any keypress registered
	if (_buttons != BTN_NONE) {
		_last_btn_timestamp = millis();
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		digitalWrite(LED_BUILTIN, LOW);
	}

	// check for change, and reset timestamp if there is
	if (_buttons != _buttons_prev) {
		_buttons_time = millis();
		_buttons_prev = _buttons;
	}
}

bool InterfaceClass::_checkBtn(uint8_t btn_combo, long unsigned time) {
	if ((btn_combo == _buttons) && (millis() - _buttons_time > time)) {
		_buttons_time = millis(); // reset timer if button registered
		return true;
	} else {
		return false;
	}
}

void InterfaceClass::_updateState() {
	if (_checkBtn(BTN_OK, BUTTON_PRESS*4)) _menu_state = !_menu_state;
	switch(_state) {
		case STATUS:
			if (_checkBtn(BTN_LEFT, BUTTON_PRESS)) _state = TURN_OFF;
			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = THRUSTER;
			break;
		case THRUSTER:
			if (_checkBtn(BTN_LEFT, BUTTON_PRESS)) _state = STATUS;
			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = WINDLASS;
			break;
		case WINDLASS:
			if (_checkBtn(BTN_LEFT, BUTTON_PRESS)) _state = THRUSTER;
			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = WINDLASS_CALIBRATE;
			break;
		case WINDLASS_CALIBRATE:
			if (_checkBtn(BTN_LEFT, BUTTON_PRESS)) _state = WINDLASS;
			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = TURN_OFF;
			break;
		case TURN_OFF:
			if (_checkBtn(BTN_LEFT, BUTTON_PRESS)) _state = WINDLASS_CALIBRATE;
			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = STATUS;
			break;
		default:
			_menu_state = true;
			_state = STATUS;
			break;
	}

	//remote.setWindlass(DIR_STOP);
	//remote.setThruster(DIR_STOP);
	//remote.setDepth(0);
	//remote.transmit();
}

void InterfaceClass::_dispStatus() {
	_display->setFont(u8g2_font_7x13B_tr);
    _display->clearBuffer();
    _display->setCursor(0,16);
    _display->print("Thruster:");
    _display->setCursor(0,32);
    _display->print("Windlass:");
    _display->setCursor(0,48);
    _display->print("Depth:");
    _display->setCursor(0,64);
    _display->print("Signal:");
    _display->setCursor(80,16);
    _display->print(remote.getThruster());
    _display->setCursor(80,32);
    _display->print(remote.getWindlass());
    _display->setCursor(80,48);
    _display->print(remote.getDepth());
    _display->setCursor(80,64);
    _display->print(remote.getSignal());
    _display->sendBuffer();
}

void InterfaceClass::_dispThruster() {
	Move dir = remote.getThruster();

	_display->clearBuffer();
	_display->drawXBMP(ICON_X-propeller_width/2, ICON_Y-propeller_height/2, propeller_width, propeller_height, propeller_bits);
	if (dir == DIR_NEG) {
		_display->drawXBMP(ARROW_LEFT_X-ARROW_WIDTH/2, ARROW_LEFT_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_solid_left_bits);
	} else {
		_display->drawXBMP(ARROW_LEFT_X-ARROW_WIDTH/2, ARROW_LEFT_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_outline_left_bits);
	}
	if (dir == DIR_POS) {
		_display->drawXBMP(ARROW_RIGHT_X-ARROW_WIDTH/2, ARROW_RIGHT_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_solid_right_bits);
	} else {
		_display->drawXBMP(ARROW_RIGHT_X-ARROW_WIDTH/2, ARROW_RIGHT_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_outline_right_bits);
	}
	if (dir == ERROR_LIMIT || dir == ERROR_COOL || dir == ERROR_DISABLED) {
		_display->drawXBMP(ARROW_X-denied_width/2, ARROW_Y-denied_height/2, denied_width, denied_height, denied_bits);
	}
	_display->updateDisplay();
}

void InterfaceClass::_dispWindlass() {
	Move dir = remote.getWindlass();

	_display->clearBuffer();
	// draw icon
	_display->drawXBMP(WINDLASS_ICON_X-anchor_width/2, ICON_Y-anchor_height/2, anchor_width, anchor_height, anchor_bits);
	
	// draw arrows
	if (dir == DIR_POS) {
		_display->drawXBMP(ARROW_UP_X-ARROW_WIDTH/2, ARROW_UP_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_solid_up_bits);
	} else {
		_display->drawXBMP(ARROW_UP_X-ARROW_WIDTH/2, ARROW_UP_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_outline_up_bits);
	}
	if (dir == DIR_NEG) {
		_display->drawXBMP(ARROW_DOWN_X-ARROW_WIDTH/2, ARROW_DOWN_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_solid_down_bits);
	} else {
		_display->drawXBMP(ARROW_DOWN_X-ARROW_WIDTH/2, ARROW_DOWN_Y-ARROW_HEIGHT/2, ARROW_WIDTH, ARROW_HEIGHT, arrow_outline_down_bits);
	}
	if (dir == ERROR_LIMIT || dir == ERROR_COOL || dir == ERROR_DISABLED) {
		_display->drawXBMP(WINDLASS_ARROW_X-denied_width/2, ARROW_Y-denied_height/2, denied_width, denied_height, denied_bits);
	}

	// print depth
	_display->setFontPosCenter();
	_display->setFont(u8g2_font_fub25_tn);
	_display->setCursor(WINDLASS_DEPTH_X,CENTER_Y);
    _display->print(remote.getDepth(),1);

	// display
	_display->updateDisplay();
}

void InterfaceClass::_dispWindlassZero() {

}

void InterfaceClass::_dispWindlassCalibrate() {

}

InterfaceClass interface;