// Game Loop and stuff
export module engine;

import window;
import renderer;
import tetris;

class Context {

};

// RAII Engine class
export class Engine {
public:
    Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    virtual ~Engine();

    void run();
private:
    bool should_close_ = false;
    Tetris tetris_{};
};