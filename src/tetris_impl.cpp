module;
#include <iostream>
#include <random>
#include "SDL.h"
module tetris;

import renderer;

static constexpr SDL_Color s_colors[] = {
    {0, 0, 0, 255},     // Empty block
    {255, 0, 0, 255}, 
    {0, 255, 255,255},
    {128, 0,  128, 255},
    {255, 255, 0, 255}, 
    {0,  255, 0, 255}, 
    {0, 0, 255, 255},
    {255, 127, 0, 122},
    {248, 248, 248, 255}  // Block to be eliminated
};

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
    1, 1, 1, 
    0, 1, 0,
    0, 0, 0
};
static constexpr uint8_t L[] = { 3,
    0, 0, 1, 
    1, 1, 1,
    0, 0, 0
};
// Inverse L
static constexpr uint8_t IL[] = { 3,
    1, 0, 0, 
    1, 1, 1,
    0, 0, 0
};

static constexpr const uint8_t* s_tetrominoes[7] = {I, O, S, Z, T, L, IL};

static constexpr const float s_time_to_drop[] = {
    60.0 / 60.0,
    30.0 / 60.0,
    10.0 / 60.0,
    1.0 / 60.0f
};

static constexpr float s_time_to_merge[] = {
    s_time_to_drop[0] * (22 + 10),
    s_time_to_drop[1] * (22 + 10),
    s_time_to_drop[2] * (22 + 10),
    s_time_to_drop[3] * (22 + 10)
};

void Tetris::init_debug()
{
    std::cout << "size of Tetromino" << sizeof(Tetromino) <<std::endl;
    std::cout << "size of Tetris" << sizeof(Tetris) <<std::endl;

    board_[board_row_col_to_index(22, 10)] = 1;
    board_[board_row_col_to_index(22, 9)] = 1;
    board_[board_row_col_to_index(22, 8)] = 1;

    generate_tetromino(curr_tetromino_);
    generate_tetromino(next_tetromino_);
    curr_tetromino_.col = (WIDTH - s_tetrominoes[curr_tetromino_.type][0]) / 2.0 + 1;
    curr_tetromino_.row = 1;
}

void Tetris::update_tetromino(float delta_time)
{
    if(!rows_to_eliminated_.empty())
    {
        for(const auto& row : rows_to_eliminated_)
        {
            set_row(row, NUM_COLOR + 1);
        }
    }
    drop_timer_ += delta_time;
    merge_timer_ += delta_time;
    eliminate_timer += delta_time;

    // Render Prediciton
    pred_tetromino_ = curr_tetromino_;
    while(check_tetromino_state_valid(pred_tetromino_))
    {
        ++pred_tetromino_.row;
    }
    --pred_tetromino_.row;
    pred_tetromino_.color = NUM_COLOR + 1;


    if(drop_timer_ > s_time_to_drop[level_ - 1])
    {
        drop_timer_ = 0.0f;
        soft_drop();
    }
    
    if(eliminate_timer > 1.5)
    {
        for(uint8_t row = 1; row <= ACTUAL_HEIGHT; ++row)
        {
            if(board_[board_row_col_to_index(row, 1)] == (NUM_COLOR + 1))
                clear_row(row);
        }
        eliminate_timer = 0.0;
    }

    if(merge_timer_ <= s_time_to_merge[level_ - 1])
        return;
    merge_timer_ = 0.0f;
    merge();
    curr_tetromino_ = next_tetromino_;
    generate_tetromino(next_tetromino_);
    curr_tetromino_.col = (WIDTH - s_tetrominoes[curr_tetromino_.type][0]) / 2.0 + 1;
    curr_tetromino_.row = 1;
}

void Tetris::rotate_left()
{
    curr_tetromino_.rotation = (curr_tetromino_.rotation + 3) % 4;
    while(!check_tetromino_state_valid(curr_tetromino_))
    {
        curr_tetromino_.row--;
    }
}

void Tetris::rotate_right()
{
    curr_tetromino_.rotation = (curr_tetromino_.rotation + 1) % 4;
    while(!check_tetromino_state_valid(curr_tetromino_))
    {
        curr_tetromino_.row--;
    }
}

void Tetris::move_left()
{
    --curr_tetromino_.col;
    if(!check_tetromino_state_valid(curr_tetromino_))
    {
        ++curr_tetromino_.col;
    }
}

void Tetris::move_right()
{
    ++curr_tetromino_.col;
    if(!check_tetromino_state_valid(curr_tetromino_))
    {
        --curr_tetromino_.col;
    }
}

void Tetris::hold_tetromino()
{
    if(!can_hold)
        return;
    Tetromino temp = curr_tetromino_;
    if(hold_tetromino_.type != UINT8_MAX)
    {
        curr_tetromino_ = hold_tetromino_;
        curr_tetromino_.col = (WIDTH - s_tetrominoes[curr_tetromino_.type][0]) / 2.0 + 1;
        curr_tetromino_.row = 1;
    }
    else   
    {
        curr_tetromino_ = next_tetromino_;
        generate_tetromino(next_tetromino_);
        curr_tetromino_.col = (WIDTH - s_tetrominoes[curr_tetromino_.type][0]) / 2.0 + 1;
        curr_tetromino_.row = 1;
    }
    hold_tetromino_ = temp;
    hold_tetromino_.row = 12 + (4 - s_tetrominoes[curr_tetromino_.type][0]) / 2.0;
    hold_tetromino_.col = 16 + (4 - s_tetrominoes[curr_tetromino_.type][0]) / 2.0;
    can_hold = false;
}

void Tetris::hard_drop()
{
    while(check_tetromino_state_valid(curr_tetromino_))
    {
        ++curr_tetromino_.row;
    }
    --curr_tetromino_.row;    
    merge();
    drop_timer_ = 0.0f;
    merge_timer_ = 0.0f;
    curr_tetromino_ = next_tetromino_;
    curr_tetromino_.row = 1;
    curr_tetromino_.col = (WIDTH - s_tetrominoes[curr_tetromino_.type][0]) / 2.0 + 1;
    generate_tetromino(next_tetromino_);
}

void Tetris::soft_drop()
{
    ++curr_tetromino_.row;
    if(!check_tetromino_state_valid(curr_tetromino_))
    {
        --curr_tetromino_.row;
    }
}

void Tetris::render_all(const Renderer& renderer) const
{
    // top left:(1, 1)
    // bottom right: (10, 22)

    // render board
    uint8_t row = 3, col = 0;
    for(int i = 0; i < WIDTH * ACTUAL_HEIGHT; ++i)
    {
        if(++col > WIDTH)
        {
            col = 1;
            ++row;
        }
        render_block(row, col, s_colors[board_[board_row_col_to_index(row, col)]], renderer);
    }

    // render current tetromino
    render_tetromino(curr_tetromino_, renderer);

    render_tetromino(pred_tetromino_, renderer);

    // canvas for upcoming tetromino
    row = 5, col = 15;
    for(int i = 0; i < 16; ++i)
    {
        if(++col > 19)
        {
            col = 16;
            ++row;
        }
        render_block(row, col, s_colors[0], renderer);
    }
    // render next tetromino preview
    render_tetromino(next_tetromino_, renderer);

    // canvas for upcoming tetromino
    row = 12, col = 15;
    for(int i = 0; i < 16; ++i)
    {
        if(++col > 19)
        {
            col = 16;
            ++row;
        }
        render_block(row, col, s_colors[0], renderer);
    }
    if(hold_tetromino_.type != UINT8_MAX)
        render_tetromino(hold_tetromino_, renderer);
}

void Tetris::render_tetromino(const Tetromino& in_piece, const Renderer& renderer)
{
    const auto& tetromino = s_tetrominoes[in_piece.type];
    SDL_Color color = s_colors[in_piece.color];

    int local_row = 0, local_col = 0;
    int n = tetromino[0];

    int row = in_piece.row, col = in_piece.col;
    while(local_row < n)
    {
        uint8_t data = tetromino[tetromino_row_col_to_index(local_row, local_col, in_piece, n)];
        if(data != 0)
            render_block(row + local_row, col + local_col, color, renderer);
        local_col++;
        if(local_col == n)
        {
            local_row++;
            local_col = 0;
        }
    }
}

void Tetris::render_block(uint8_t row, uint8_t col, const SDL_Color& color, const Renderer& renderer)
{
    constexpr int w = 30, h = 30;
    // TODO: eliminate hard coding: 490, 720...
    SDL_Rect rect{490 + (col - 1) * h, (720 - h * ACTUAL_HEIGHT) + (row - 1) * w , w, h};
    renderer.draw_rect_fill(rect, color);
    renderer.draw_rect_outline(rect, {125, 125, 125, 255});
}

void Tetris::set_board(uint8_t row, uint8_t col, uint8_t value)
{
    board_[board_row_col_to_index(row, col)] = value;
}

bool Tetris::check_row_full(uint8_t row) const
{
    for(uint8_t col = 1; col <= WIDTH; ++col)
    {
        if(board_[board_row_col_to_index(row, col)] == 0)
            return false;
    }
    return true;
}

void Tetris::clear_row(uint8_t row)
{
    if(row == ACTUAL_HEIGHT)
    {
        for(int i = 1; i <= 10; ++i)
        {
            set_board(row, i, 0);
        }
        bottom_row_ = (bottom_row_ - 1) == 0 ? 22 : --bottom_row_;
    }
    else
    {
        for(int line = row; line > 1; --line)
        {
            memcpy(&board_[board_row_col_to_index(line, 1)], &board_[board_row_col_to_index(line - 1, 1)], WIDTH);
        }
    }
    points_ += 10;
}

void Tetris::set_row(uint8_t row, uint8_t value)
{
    for(uint8_t col = 1; col <= WIDTH; ++col)
    {
        board_[board_row_col_to_index(row, col)] = value;
    }
}

uint8_t Tetris::board_row_col_to_index(uint8_t row, uint8_t col) const
{

    return ((bottom_row_ * (WIDTH)) + (row - 1) * WIDTH + col - 1) % (WIDTH * ACTUAL_HEIGHT);
}

uint8_t Tetris::tetromino_row_col_to_index(uint8_t local_row, uint8_t local_col, Tetromino tetromino, uint8_t n) 
{
    switch(tetromino.rotation)
    {
        case 0:
            return local_col * n + n - local_row;
        case 1:
            return local_row * n + local_col + 1;
        case 2:
            return (n - 1 - local_col) * n + local_row + 1;
        case 3:
            return (n - 1 - local_row) * n + (n - 1 - local_col) + 1;
    }

    return 0;
}

void Tetris::generate_tetromino(Tetromino& in_tetromino)
{
    // color
    int color = std::rand() % NUM_COLOR + 1;
    // type
    int type = std::rand() % 7;
    
    in_tetromino.color = static_cast<uint8_t>(color);
    in_tetromino.type = type;
    in_tetromino.rotation = 1;
    in_tetromino.row = 5 + (4 - s_tetrominoes[curr_tetromino_.type][0]) / 2.0;
    in_tetromino.col = 16 + (4 - s_tetrominoes[curr_tetromino_.type][0]) / 2.0;
}

void Tetris::merge()
{
    const auto& tetromino = s_tetrominoes[curr_tetromino_.type];

    uint8_t local_row = 0, local_col = 0;
    uint8_t n = tetromino[0];
    uint8_t color = curr_tetromino_.color;
    uint8_t row = curr_tetromino_.row, col = curr_tetromino_.col;
    while(local_row < n)
    {
        uint8_t data = tetromino[tetromino_row_col_to_index(local_row, local_col, curr_tetromino_, n)];
        if(data != 0)
            board_[board_row_col_to_index(row + local_row, col + local_col)] = data * color;
        if(check_row_full(row + local_row))
        {
            set_row(row + local_row, NUM_COLOR + 1);
        }
        local_col++;
        if(local_col == n)
        {
            local_row++;
            local_col = 0;
        }
    }
    can_hold = true;
}

bool Tetris::check_tetromino_state_valid(const Tetromino& in_tetromino) const
{
    const auto& tetromino = s_tetrominoes[in_tetromino.type];
    int local_row = 0, local_col = 0;
    int n = tetromino[0];

    int row = in_tetromino.row, col = in_tetromino.col;
    while(local_row < n)
    {
        uint8_t data = tetromino[tetromino_row_col_to_index(local_row, local_col, in_tetromino, n)];
        if(data != 0)
        {
            if(board_[board_row_col_to_index(row + local_row, col + local_col)] !=0 ||
            row + local_row > ACTUAL_HEIGHT || row + local_row <= 0 ||
            col + local_col > WIDTH || col + local_col <= 0)
                return false;
        }
        local_col++;
        if(local_col == n)
        {
            local_row++;
            local_col = 0;
        }
    }
    return true;
}