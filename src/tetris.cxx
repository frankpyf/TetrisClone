module;
#include "SDL.h"
export module tetris;

import renderer;
import input;

constexpr uint8_t WIDTH = 10;
constexpr uint8_t HEIGHT = 20;
// height with hidden bars
constexpr uint8_t ACTUAL_HEIGHT = 22;

constexpr uint8_t NUM_COLOR = 7;

// 8 bytes
struct Tetromino {
    uint8_t color       = UINT8_MAX;
    uint8_t type        = UINT8_MAX;
    uint8_t rotation    = 1;
    // row of the top left block of this piece
    uint8_t row         = 1;
    // col of the top left block of this piece
    int8_t  col         = 1;
    char    padding[3];
};

export class Tetris {
    Tetromino curr_tetromino_{};
    Tetromino next_tetromino_{};
    Tetromino hold_tetromino_{};    
    
    // used to calculate time to drop
    float drop_timer_ = 0.0; 
    float merge_timer_ = 0.0;
    float eliminate_timer = 0.0;
    // ring buffer
    uint8_t board_[WIDTH * ACTUAL_HEIGHT]{0};
    uint8_t bottom_row_ = 22;

    uint8_t level_ = 1;
    uint32_t points_ = 0;
    bool can_hold = true;
public:
    void init_debug();
    void init();
    
    bool is_game_end();
    void update_tetromino(float delta_time);

    void render_all(const Renderer&) const;

    void rotate_left();
    void rotate_right();
    void move_left();
    void move_right();
    void move_down();

    void hold_tetromino();

    void hard_drop();
    void soft_drop();
private:
    void generate_tetromino(Tetromino& in_tetromino);
    
    static void render_block(uint8_t row, uint8_t col, const SDL_Color&, const Renderer&);
    static void render_tetromino(const Tetromino&,const Renderer&);

    void set_board(uint8_t row, uint8_t col, uint8_t value);
    bool check_row_full(uint8_t row) const;
    void set_row(uint8_t row, uint8_t value);
    void clear_row(uint8_t row);

    void merge();
    // swap current and next tetromino
    void swap();

    bool check_tetromino_state_valid() const;

    uint8_t get_tallest_underneath();

    uint8_t board_row_col_to_index(uint8_t row, uint8_t col) const;
    static uint8_t tetromino_row_col_to_index(uint8_t local_row, uint8_t local_col, Tetromino, uint8_t n);
};