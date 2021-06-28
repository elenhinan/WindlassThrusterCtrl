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
	_last_refresh(0),
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
    // setup hw pushbuttons
    pinMode(_pin_btn_left, INPUT_PULLUP);
    pinMode(_pin_btn_right, INPUT_PULLUP);
    pinMode(_pin_btn_up, INPUT_PULLUP);
    pinMode(_pin_btn_down, INPUT_PULLUP);
    pinMode(_pin_btn_ok, INPUT_PULLUP);

	resetIdle();

	return true;
}

void InterfaceClass::update() {
	unsigned long now = millis();

	_buttons = 0;
	_buttons |= digitalRead(_pin_btn_left)  ? 0 : BTN_LEFT;
	_buttons |= digitalRead(_pin_btn_right) ? 0 : BTN_RIGHT;
	_buttons |= digitalRead(_pin_btn_up)    ? 0 : BTN_UP;
	_buttons |= digitalRead(_pin_btn_down)  ? 0 : BTN_DOWN;
	_buttons |= digitalRead(_pin_btn_ok)    ? 0 : BTN_OK;
	
	// if any keypress registered
	if (_buttons != BTN_NONE) {
		_last_btn_timestamp = now;
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		digitalWrite(LED_BUILTIN, LOW);
	}

	// check for change, and reset timestamp if there is
	if (_buttons != _buttons_prev) {
		_buttons_time = now;
		_buttons_prev = _buttons;
	}

	//
	_updateState();
}

bool InterfaceClass::_checkBtn(uint8_t btn_combo, long unsigned time, bool hold) {
	unsigned long now = millis();
	if ((btn_combo == _buttons) && (now - _buttons_time > time)) {
		if (!hold) {
			_buttons_time = now; // reset timer if button registered
		}
		return true;
	} else {
		return false;
	}
}

void InterfaceClass::_updateState() {

	// ok + up = force_up
	// ok + down = force_down
	// ok hold 3 sec = switch
	// ok hold 6 sec = menu
	//		- zero (reset counter)
	// 		- set max length (default current)
	//		- calibrate length (enter measured)
	//		- set cooldown thruster (0.1 seconds)
	//		- set cooldown windlass (0.1 seconds)
	//		- save settings
	//		- debug display

	switch(_state) {
		case STATUS:
			if (_checkBtn(BTN_OK, BUTTON_LONGPRESS)) _state = THRUSTER;
			break;
		case THRUSTER:
			if (_checkBtn(BTN_OK, BUTTON_LONGPRESS)) _state = WINDLASS;
			else if (_checkBtn(BTN_RIGHT, BUTTON_SHORTPRESS, true)) radio.setThruster(DIR_POS);
			else if (_checkBtn(BTN_LEFT, BUTTON_SHORTPRESS, true)) radio.setThruster(DIR_NEG);
			else radio.setThruster(DIR_STOP);
			break;
		case WINDLASS:
			if (_checkBtn(BTN_OK, BUTTON_LONGPRESS)) _state = STATUS;
			else if (_checkBtn(BTN_UP, BUTTON_SHORTPRESS, true)) radio.setWindlass(DIR_POS);
			else if (_checkBtn(BTN_DOWN, BUTTON_SHORTPRESS, true)) radio.setWindlass(DIR_NEG);
			else if (_checkBtn(BTN_UP | BTN_OK, BUTTON_SHORTPRESS, true)) radio.setWindlass(DIR_FORCE_POS);
			else if (_checkBtn(BTN_DOWN | BTN_OK, BUTTON_SHORTPRESS, true)) radio.setWindlass(DIR_FORCE_NEG);
			else radio.setWindlass(DIR_STOP);
			break;
//		case WINDLASS_CALIBRATE:
//			if (_checkBtn(BTN_LEFT, BUTTON_LONGPRESS)) _state = WINDLASS;
//			else if (_checkBtn(BTN_RIGHT, BUTTON_PRESS)) _state = STATUS;
//			break;
		default:
			_state = STATUS;
			break;
	}
}

void InterfaceClass::display(bool force) {
	unsigned long now = millis();
  	if (now - _last_refresh > REFRESH_INTERVAL || force) {
		_last_refresh = now;

		switch(_state) {
			case STATUS: _dispStatus(); break;
			case THRUSTER: _dispThruster(); break;
			case WINDLASS: _dispWindlass(); break;
			case WINDLASS_CALIBRATE: _dispWindlassCalibrate(); break;
			default: break;
		}
	}
}

void InterfaceClass::_dispStatus() {
	_display->clearBuffer();
	_display->setFontPosTop();
	_display->setFont(u8g2_font_6x10_tr);

    _display->setCursor(0,0);
	_display->print(F("Thruster: "));
    _printCommand(radio.getThruster());

    _display->setCursor(0,10);
	_display->print(F("Windlass: "));
	_printCommand(radio.getWindlass());

    _display->setCursor(0,20);
	_display->print(F("Depth:    "));
	_display->print(radio.getDepth(),1);
	_display->print(F(" m"));

    _display->setCursor(0,30);
	_display->print(F("Signal:   "));
	_display->print(radio.getSignal(),0);
	_display->print(F(" dBm"));
    
    _display->setCursor(0,40);
	_display->print(F("Battery:  "));
	_display->print(battery.getVoltage(),2);
	_display->print(F(" V"));

    _display->setCursor(0,50);
	_display->print(F("Buttons:  "));
	_display->setFont(u8g2_font_open_iconic_all_1x_t);
	if (_buttons & BTN_LEFT)  _display->write(74);
	if (_buttons & BTN_RIGHT) _display->write(75);
	if (_buttons & BTN_UP)    _display->write(76);
	if (_buttons & BTN_DOWN)  _display->write(73);
	if (_buttons & BTN_OK)    _display->write(115);

    _display->sendBuffer();
}

void InterfaceClass::_drawButtons() {
	const uint8_t x = BUTTON_X;
	const uint8_t y = BUTTON_Y;
	if(_buttons & BTN_LEFT) 	_display->drawBox(x+0, y+3, 2, 2);
	if(_buttons & BTN_RIGHT)	_display->drawBox(x+6, y+3, 2, 2);
	if(_buttons & BTN_UP) 		_display->drawBox(x+3, y+0, 2, 2);
	if(_buttons & BTN_DOWN) 	_display->drawBox(x+3, y+6, 2, 2);
	if(_buttons & BTN_OK)   	_display->drawBox(x+3, y+3, 2, 2);
}

void InterfaceClass::_drawBattery() {
	const uint8_t x = BATTERY_X;
	const uint8_t y = BATTERY_Y;
	const uint8_t w = BATTERY_WIDTH - 1;
	const uint8_t h = BATTERY_HEIGHT;

	const uint8_t bar_max = w-4;
	uint8_t bar = battery.getPercentage() * (bar_max+1);

	_display->drawFrame(x, y, w, h);
	_display->drawVLine(x+w, y+2, y+h-4);
	_display->drawBox(x+2,y+2,min(bar, bar_max),h-4);
}

void InterfaceClass::_drawSignal() {
	const uint8_t x = SIGNAL_X;
	const uint8_t y = SIGNAL_Y;
	const uint8_t bar_y = y + antenna_height;
	const uint8_t bar_x = x + antenna_width + 2;
	const uint8_t n_bars = 4;

	float snr = radio.getSNR();
	
	if (snr == NAN) // do not draw antenna if no signal
		return;

	// else draw bars
	uint8_t bar = constrain(snr * n_bars+1,0,n_bars);
	_display->drawXBMP(x, y, antenna_width, antenna_height, antenna_bits);
	for (uint8_t i=0; i<bar; i++) {
		uint8_t bar_h = (i+1)*2;
		_display->drawBox(bar_x + 3*i, bar_y-bar_h, 2, bar_h);
	}
}

void InterfaceClass::_printCommand(Move cmd) {
	switch(cmd) {
		case DIR_STOP:		_display->print(F("stop")); break;
		case DIR_POS:
		case DIR_FORCE_POS: _display->print(F("run +")); break;
		case DIR_NEG:
		case DIR_FORCE_NEG:	_display->print(F("run -")); break;
		case NOOP:			_display->print(F("idle")); break;
		case ERROR_DISABLED:_display->print(F("disabled")); break;
		case ERROR_COOL:	_display->print(F("cooldown")); break;
		case ERROR_LIMIT:	_display->print(F("limit")); break;
		case UNKNOWN:
		default:       		_display->print(F("--")); break;
	}
}

void InterfaceClass::_dispThruster() {
	Move dir = radio.getThruster();

	_display->clearBuffer();
	_drawBattery();
	_drawSignal();
	//_drawButtons();
	
	// draw icon
	_display->drawXBMP(ICON_X-propeller_width/2, ICON_Y-propeller_height/2, propeller_width, propeller_height, propeller_bits);

	// draw arrows
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
		_display->drawXBMP(THRUSTER_ARROW_X-denied_width/2, ARROW_Y-denied_height/2, denied_width, denied_height, denied_bits);
	}

	// display
	_display->updateDisplay();
}

void InterfaceClass::_dispWindlass() {
	Move dir = radio.getWindlass();

	_display->clearBuffer();
	_drawBattery();
	_drawSignal();
	//_drawButtons();

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
	_display->setCursor(WINDLASS_DEPTH_X,WINDLASS_DEPTH_Y);
	float depth = radio.getDepth();
    _display->print(depth, (depth<100 && depth>-10) ? 1:0);

	// display
	_display->updateDisplay(); 
}

void InterfaceClass::_dispWindlassCalibrate() {
	_display->clearBuffer();
	_drawBattery();
	_drawSignal();
	_drawButtons();
	_display->updateDisplay();
}

InterfaceClass interface;