module;
#include "SDL.h"
export module tetris;

import <random>;

import renderer;

constexpr uint8_t WIDTH = 10;
constexpr uint8_t HEIGHT = 20;
// height with hidden bars
constexpr uint8_t ACTUAL_HEIGHT = 22;

constexpr uint8_t NUM_COLOR = 4;

// 8 bytes
struct Tetromino {
    uint8_t color       = 0;
    uint8_t type        = 0;
    uint8_t rotation    = 1;
    // row of the top left block of this piece
    uint8_t row         = 1;
    // col of the top left block of this piece
    uint8_t col         = 1;
    char    padding[3];
};

export class Tetris {
    Tetromino curr_tetromino_{};
    Tetromino next_tetromino_{};
    Tetromino hold_tetromino_{};    
    
    // used to calculate time to drop
    float time_passed_ = 0.0; 
    // ring buffer
    uint8_t board_[WIDTH * ACTUAL_HEIGHT]{0};
    uint8_t bottom_row_ = 0;

    uint8_t level_ = 1;
public:
    void init_debug();
    void init();
    
    bool is_game_end();
    void update_tetromino(float delta_time);

    void render_all(const Renderer&) const;

    void rotate_left();
    void rotate_right();

    void hold_tetromino();

    void hard_drop();
    void soft_drop();
private:
    void generate_tetromino(Tetromino& in_tetromino);
    
    void render_block(uint8_t row, uint8_t col, const SDL_Color&, const Renderer&) const;
    void render_tetromino(const Renderer&) const;

    void set_board(uint8_t row, uint8_t col, uint8_t value);
    void clear_row(uint8_t row);

    void merge();

    uint8_t board_row_col_to_index(uint8_t row, uint8_t col) const;
    static uint8_t tetromino_row_col_to_index(uint8_t local_row, uint8_t local_col, Tetromino, uint8_t n);
};