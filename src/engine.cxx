// Game Loop and stuff
export module engine;

import window;
import renderer;
import tetris;
import input;

export class Engine {
    Tetris tetris_{};
    InputSystem input_{};
    bool should_close_ = false;
public:
    Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    virtual ~Engine();

    void run();
private:
    void process_input();


};