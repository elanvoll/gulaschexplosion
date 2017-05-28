
#ifndef _INC_DEFINES_H

#define _INC_DEFINES_H

#define TIMEOUT_PROP_DELAY_MS 500


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define APP_NAME "Gulaschbombe"
#define APP_VERSION "0.2"

#define GAME_TCP_PORT 8262

#define PLAYERS 2

#define STATE_HOST_MESSAGE "Share Game\nvia IR Sender"
#define STATE_JOIN_MESSAGE "Revice Game\ninfo via IR\nSensor from\nHost"

enum game_state_t {
  GAME_STATE_UNUSED = 0,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_SHARING_ACCESS,
  GAME_STATE_RECEIVING_ACCESS,
  GAME_STATE_CONNECTING_WIFI,
  GAME_STATE_HOST_AWAIT_START,
  GAME_STATE_CLIENT_AWAIT_START,
  GAME_STATE_RUNNING,
  GAME_STATE_FINISH
};

#define PLAYER_ID_HOST 0

#define HOST_IP IPAddress(10, 0, 0, 1)

#define STICK_DIR_UP 1
#define STICK_DIR_LEFT 2
#define STICK_DIR_DOWN 3
#define STICK_DIR_RIGHT 4

#endif