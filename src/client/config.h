// settings
#define INTERVAL                100
#define AUTO_OFF                300000
#define BUTTON_PRESS            200 // ms a button must be pressed to register
#define BUTTON_WAKE             2000 // ms to keep button pressed to wake from sleep

// display
#define OLED_WIDTH 128 // OLED display width, in pixels
#define OLED_HEIGHT 64 // OLED display height, in pixels

// interface layout
#define CENTER_Y        36
#define ICON_X          32
#define ICON_Y          CENTER_Y
#define THRUSTER_ICON_X     32
#define THRUSTER_ARROW_X    100
#define WINDLASS_ICON_X     16
#define WINDLASS_ARROW_X    44
#define WINDLASS_DEPTH_X    60
#define WINDLASS_DEPTH_Y    CENTER_Y +4
#define ARROW_Y         CENTER_Y
#define ARROW_LEFT_X    THRUSTER_ARROW_X - 18
#define ARROW_LEFT_Y    ARROW_Y
#define ARROW_RIGHT_X   THRUSTER_ARROW_X + 17
#define ARROW_RIGHT_Y   ARROW_Y
#define ARROW_UP_X      WINDLASS_ARROW_X
#define ARROW_UP_Y      ARROW_Y - 18
#define ARROW_DOWN_X    WINDLASS_ARROW_X
#define ARROW_DOWN_Y    ARROW_Y + 17

#define STATUS_Y        0
#define BATTERY_X       84
#define BATTERY_Y       STATUS_Y
#define BATTERY_WIDTH   16
#define BATTERY_HEIGHT  8
#define SIGNAL_X        104
#define SIGNAL_Y        STATUS_Y
#define SIGNAL_BARS     5
#define BUTTON_X        72
#define BUTTON_Y        STATUS_Y
