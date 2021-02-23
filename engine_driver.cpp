#include <iostream>
#include <ios>
#include <string>
#include "lot_bot.h"
#include "engine_driver.h"

EngineDriver::~EngineDriver() {
    if(this->bot) {
        delete this->bot;
    }
}

void EngineDriver::run() {
    // set up game features
    base_setup();
    std::cerr << "[INFO] Game setup complete.\n";

    // loop game commands
    game_loop();
    
    std::cerr << "[INFO] Game ended.\n";
    
}

void EngineDriver::base_setup() {
        // receives xboard and protover messages
        if(!fgets(this->inBuf, BUFFER_SIZE, stdin) 
            || !fgets(this->inBuf, BUFFER_SIZE, stdin)){
            std::cerr << ("[ERROR] Did not receive handshake.\n");
            exit(-1);
        }

        // checks second message to be protover
        std::string command(inBuf);
        if(command.find("protover") == std::string::npos) {
            std::cerr << ("[ERROR] Did not receive protover.\n");
             std::cout << "quit";
        }

        // sends setup features to engine
        printf("feature usermove=1 debug=1 name=LOTBOT sigint=0 sigterm=0 san=0 colors=0");
        printf("feature done=1\n");
        fflush(stdout); 

        for(int i = 0; i < 7; ++i) { // num features accept/reject answers
            if(!fgets(this->inBuf, BUFFER_SIZE, stdin)) {
                std::cerr << ("[ERROR] Expected feature accept/reject and did not receive. Terminated.\n");
                exit(-1);
            }
        }
}

void EngineDriver::game_loop() {
    int turn = 0;
    bot = new LotBot(FORCE_MODE);
    Sides side_to_move = UNDEFINED;
    while(true) {
        // move if bot turn
        if(side_to_move == bot->get_side()) {
            cout << bot->botmove();
            side_to_move == WHITE ? side_to_move = BLACK : side_to_move = WHITE;
            std::cout.flush();
        }

        // read input and construct command
        if(!fgets(this->inBuf, BUFFER_SIZE, stdin)) {
            std::cerr << ("[ERROR] Expected command and did not receive. Terminated.\n");
            exit(-1);
        }
        std::string command(inBuf);

        // manage command
        if (command.find("accepted") != std::string::npos) {
            continue;
        } 

        if (command.find("usermove") != std::string::npos) {
            turn++;
            bot->usermove(command);
            side_to_move == WHITE ? side_to_move = BLACK : side_to_move = WHITE; // WHITE -> 0, BLACK -> 1
            continue;
        } 

        if(command.find("new") != std::string::npos) {
            cerr << "TURN: " << turn << endl;
            turn = 0;
            delete bot;
            bot = new LotBot(BLACK);
            side_to_move = WHITE;
            continue;
        }

        if(command.find("go") != std::string::npos) {
            cerr << "TURN: " << turn << endl;
            turn = 0;
            bot->set_side(side_to_move);
            continue;
        }

        if(command.find("force") != std::string::npos) {
            bot->set_side(FORCE_MODE);
            continue;
        } 
 
        if(command.find("quit") != std::string::npos) {
            cerr << "Quit received. Stopping engine.\n";
            exit(-1);
        }
    }
}

void EngineDriver::send_resign() {
    std::cout<< "resign\n";    
}

void EngineDriver::send_quit() {
    std::cerr << "[INFO] trying to quit\n";
    std::cout<< "quit\n"; 
}