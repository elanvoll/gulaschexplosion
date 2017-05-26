
#ifndef _INC_DEFINES_H

#define _INC_DEFINES_H

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define APP_NAME "Gulaschexplosion v0.1"


enum game_state_t {
  GAME_STATE_UNUSED = 0,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_SHARING_ACCESS,
  GAME_STATE_RECEIVING_ACCESS
};



#endif