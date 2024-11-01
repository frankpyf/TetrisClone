// Game Loop and stuff
export module engine;

import window;
import renderer;
import tetris;
import input;
import <memory>;
import <vector>;
import <cstdint>;

export class Game
{
    int32_t points_ = 0;
    int32_t level_ = 1;

    // rows eliminated at current level
    uint32_t rows_eliminated_ = 0;
public:
    void add_score(uint8_t rows_eliminated, bool b_tspin);
};


export class Engine {
public:
    InputSystem input_;
    Game game_;
private:
    std::vector<std::unique_ptr<Object>> game_objs_{};
    // tick frequency per millisecond
    uint32_t frame_interval_ = 1000 / 60;
    bool should_close_ = false;
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    virtual ~Engine();
    void init();
    void run();

    void set_fps_(float new_fps) { frame_interval_= 1000.f / new_fps; }

    static Engine& get() { static Engine g_engine; return g_engine; }

    template<class T>
    T* get_object_by_class()
    {
        for(auto& obj : game_objs_)
        {
            if(T* casted_obj = dynamic_cast<T*>(obj.get()); casted_obj != nullptr)
            {
                return casted_obj;
            }
        }
        return nullptr;
    }
private:
    Engine();
    void process_input();
};