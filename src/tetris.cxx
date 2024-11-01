module;
#include "SDL.h"
export module tetris;

import renderer;
import input;
import <limits>;
import <vector>;

constexpr uint8_t WIDTH = 10u;
constexpr uint8_t HEIGHT = 20u;
// height with hidden bars
constexpr uint8_t ACTUAL_HEIGHT = 22u;

constexpr uint8_t NUM_COLOR = 7u;

export class Object
{
    friend class Engine;
protected:
    // object with higher value will be render on top
    uint8_t render_priority_ = 0;
    uint8_t update_frequency_ = 60;
    bool b_should_render_ = true;
    bool b_should_tick_ = true;

    float interval_ = 0.f;
public:
    virtual void init() {};
    virtual void tick(float delta_time) {};
    virtual void render(const Renderer& in_renderer) const = 0;
    
    void set_update_frequency(uint8_t new_frequency) {update_frequency_ = new_frequency; }
    void do_tick(float delta_time);
    void do_render(const Renderer& in_renderer) const;
};

class Board;
export class Tetromino : public Object
{
    friend class Board;

    Board&  owning_board_;
    struct Info
    {
        uint8_t color;
        uint8_t type;

        Info(uint8_t in_color = std::rand() % NUM_COLOR + 1u, uint8_t in_type = std::rand() % 7)
            : color(in_color), type(in_type) {}

        friend void swap(Info& a, Info& b)
        {
            std::swap(a.color, b.color);
            std::swap(a.type, b.type);
        }
    } info_;
    uint8_t rotation_ = 0;
    // row of the top left block of this piece
    int8_t row_;
    // col of the top left block of this piece
    int8_t col_;
    
    int8_t predict_row_ = 0;
    uint8_t grounded_tick_ = 0;

public:
    Tetromino(Board&, const uint8_t row = 1, const uint8_t col = 1, const Info = Info());
    Tetromino(const Tetromino&) = default;
    Tetromino& operator=(const Tetromino&) = default;
    virtual ~Tetromino() = default;

    void move_right(const int8_t input_value);
    void move_down(const int8_t input_value);
    void rotate();
    void hard_drop();

    Tetromino& reset_pos(const uint8_t row = 1, const uint8_t col = 1);
    void reset(Info = Info());
    void to_next();
    void hold();

    // return color_idx of the coord given
    uint8_t get(uint8_t local_row, uint8_t local_col) const;
    
    void init() override;
    void tick(float delta_time) override;
    void render(const Renderer&) const override;

    bool check_state_valid() const;
    void update_predict();

    static constexpr uint8_t I[] = { 4,
        0, 0, 0, 0,
        1, 1, 1, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0 
    };
    static constexpr uint8_t O[] = { 2,
        1, 1, 
        1, 1 
    };
    static constexpr uint8_t Z[] = { 3,
        1, 1, 0, 
        0, 1, 1,
        0, 0, 0
    };
    static constexpr uint8_t S[] = { 3,
        0, 1, 1, 
        1, 1, 0,
        0, 0, 0
    };
    static constexpr uint8_t T[] = { 3,
        0, 0, 0,
        1, 1, 1, 
        0, 1, 0,
    };
    static constexpr uint8_t L[] = { 3,
        0, 0, 0,
        0, 0, 1, 
        1, 1, 1,
    };
    // Inverse L
    static constexpr uint8_t IL[] = { 3,
        0, 0, 0,
        1, 0, 0, 
        1, 1, 1,
    };

    static constexpr const uint8_t* s_tetrominoes[7] = {I, O, S, Z, T, L, IL};
};

export class Board : public Object
{
    Tetromino::Info hold_tetromino_{0};
    Tetromino::Info next_tetromino_;
    uint32_t width_;
    uint32_t height_;
    uint32_t unit_length_;
    std::vector<uint8_t> data_;
public:
    Board(uint32_t width, uint32_t height, uint32_t unit_length = 30);
    virtual ~Board() = default;

    void merge(const Tetromino&);
    // return color_idx of the coord given
    uint8_t get(uint8_t global_row, uint8_t global_col) const;
    uint32_t get_width() const { return width_; }
    uint32_t get_height() const { return height_; }
    uint32_t get_unit_length() const { return unit_length_; }
    Tetromino::Info pop_next();
    void swap_hold(Tetromino& to_swap);

    void init() override;
    void render(const Renderer&) const override;
};
