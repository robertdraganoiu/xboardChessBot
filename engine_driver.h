#ifndef ENGINE_DRIVER_H
#define ENGINE_DRIVER_H

#include "lot_bot.h"
#define BUFFER_SIZE 64


class EngineDriver {
private:
    LotBot* bot = nullptr;
    char inBuf[BUFFER_SIZE];
    char outBuf[BUFFER_SIZE];

public:
    ~EngineDriver();
    void run();
    void base_setup();
    void game_loop();

    void send_resign();
    void send_quit();
};

#endif