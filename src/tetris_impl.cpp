module;
#include <iostream>
#include <random>
#include <functional>
#include "SDL.h"
module tetris;

import engine;

static constexpr const uint8_t s_points_for_row[] = {40, 100, 300, 1200};

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

void Object::do_tick(float delta_time)
{
    if(!b_should_tick_) return;
    float update_interval = 1000.f / update_frequency_;
    interval_ += delta_time;
    if(interval_ < update_interval) return;
    interval_ = 0;
    tick(delta_time);
}

void Object::do_render(const Renderer& in_renderer) const
{
    if(!b_should_render_) return;
    render(in_renderer);
}

Tetromino::Tetromino(Board& owning_board, const uint8_t row, const uint8_t col, const Info info)
    : owning_board_(owning_board), info_(info), row_(row), col_(col)
{
    render_priority_ = 1;
    update_predict();
}

void Tetromino::move_right(const int8_t input_value)
{
    int8_t original_col = col_;
    col_ += input_value;
    if(!check_state_valid())
    {
        col_ = original_col;
    }
    update_predict();
}
void Tetromino::move_down(const int8_t input_value)
{
    int8_t original_row = row_;
    row_ += input_value;
    if(!check_state_valid())
    {
        row_ = original_row;
    }
}

void Tetromino::rotate()
{
    uint8_t original_rotation = rotation_;
    rotation_ = (rotation_ + 1) % 4;
    if(!check_state_valid())
    {
        rotation_ = original_rotation;
    }
    update_predict();
}

void Tetromino::hard_drop()
{
    row_ = predict_row_;
    owning_board_.merge(*this);
    to_next();
}

Tetromino &Tetromino::reset_pos(const uint8_t row, const uint8_t col)
{
    row_ = row;
    col_ = col;
    update_predict();
    return *this;
}
void Tetromino::reset(Info new_tetromino)
{
    info_ = new_tetromino;
    rotation_ = 0;
}

void Tetromino::to_next()
{
    reset(owning_board_.pop_next());
    reset_pos();
    grounded_tick_ = 0;
}

void Tetromino::hold()
{
    owning_board_.swap_hold(*this);
}

uint8_t Tetromino::get(uint8_t local_row, uint8_t local_col) const
{
    const uint8_t* const cur_tetromino = s_tetrominoes[info_.type];
    uint8_t n = cur_tetromino[0];
    uint8_t idx = 0;
    switch (rotation_)
    {
    case 0:
        idx = local_row * n + local_col + 1;
        break;
    case 1:
        idx = (n - 1 - local_col) * n + local_row + 1;
        break;
    case 2:
        idx = (n - 1 - local_row) * n + (n - 1 - local_col) + 1;
        break;
    case 3:
        idx = local_col * n + n - local_row;
        break;
    }

    return info_.color * cur_tetromino[idx];
}

void Tetromino::init()
{
    update_frequency_ = 1;   
    auto& g_engine = Engine::get();
    auto& input_system = g_engine.input_;

    input_system.bind_action(SDL_SCANCODE_A, std::bind(&Tetromino::move_right, this, -1));
    input_system.bind_action(SDL_SCANCODE_D, std::bind(&Tetromino::move_right, this, 1));
    input_system.bind_action(SDL_SCANCODE_S, std::bind(&Tetromino::move_down, this, 1));
    input_system.bind_action(SDL_SCANCODE_R, std::bind(&Tetromino::rotate, this));
    input_system.bind_action(SDL_SCANCODE_SPACE, std::bind(&Tetromino::hard_drop, this));
    input_system.bind_action(SDL_SCANCODE_Q, std::bind(&Tetromino::hold, this));
}

void Tetromino::tick(float delta_time)
{
    ++row_;
    if(!check_state_valid())
    {
        --row_;
        ++grounded_tick_;
        if(grounded_tick_ > 5)
        {
            owning_board_.merge(*this);
            to_next();
        }
    }
    else
    {
        grounded_tick_ = 0;
    }
}

void Tetromino::render(const Renderer& renderer) const
{
    int length_with_space = owning_board_.get_unit_length() + 1;
    int offset_w = -(owning_board_.get_width() / 2) * length_with_space;
    int offset_h = -(owning_board_.get_height() / 2) * length_with_space;

    const uint8_t* const tetromino = s_tetrominoes[info_.type];
    uint8_t n = tetromino[0];
    for(int row = 0; row < n; ++row)
    {
        for(int col = 0; col < n; ++col)
        {
            if(!get(row, col))
                continue;
            int board_row = row + row_;
            int board_col = col + col_;
            renderer.draw_rect_fill(
                {offset_w + board_col * length_with_space, offset_h + board_row * length_with_space, static_cast<int>(owning_board_.get_unit_length()), static_cast<int>(owning_board_.get_unit_length())},
                s_colors[get(row, col)]);
        }
    }
    if(predict_row_ - row_ < n) return;
    // render predict when far away
    for(int row = 0; row < n; ++row)
    {
        for(int col = 0; col < n; ++col)
        {
            if(!get(row, col))
                continue;
            int board_row = row + predict_row_;
            int board_col = col + col_;
            renderer.draw_rect_outline(
                {offset_w + board_col * length_with_space, offset_h + board_row * length_with_space, static_cast<int>(owning_board_.get_unit_length()), static_cast<int>(owning_board_.get_unit_length())},
                4,
                s_colors[get(row, col)]);
        }
    }
}

bool Tetromino::check_state_valid() const
{
    const uint8_t* const cur_tetromino = s_tetrominoes[info_.type];
    uint8_t n = cur_tetromino[0];
    for(int local_row = 0; local_row < n; ++local_row)
    {
        for(int local_col = 0; local_col < n; ++local_col)
        {
            if(!get(local_row, local_col)) continue;
            int8_t global_row = row_ + local_row;
            int8_t global_col = col_ + local_col;
            
            if(global_row < 0 || global_row >= owning_board_.get_height()
            || global_col < 0 || global_col >= owning_board_.get_width()
            || owning_board_.get(global_row, global_col))
            {
                return false;
            }
        }
    }
    return true;
}

void Tetromino::update_predict()
{
    predict_row_ = row_;
    while(check_state_valid())
    {
        ++row_;
    }
    --row_;
    std::swap(predict_row_, row_);
}

Board::Board(uint32_t width, uint32_t height, uint32_t unit_length)
    : width_(width), height_(height), unit_length_(unit_length), data_(width_ * height_, 0)
{
}

void Board::merge(const Tetromino& in_tetromino)
{
    bool b_tspin = false;
    auto tetromino = Tetromino::s_tetrominoes[in_tetromino.info_.type];
    if(tetromino == Tetromino::T)
    {
        // check whether three of the four corners are occupied
        // if so, it is a t-spin
        int num_occupied = 0;
        if(get(in_tetromino.row_, in_tetromino.col_)) ++num_occupied;
        if(get(in_tetromino.row_ + 2, in_tetromino.col_)) ++num_occupied;
        if(get(in_tetromino.row_, in_tetromino.col_ + 2)) ++num_occupied;
        if(get(in_tetromino.row_ + 2, in_tetromino.col_ + 2)) ++num_occupied;
        
        if(num_occupied == 3)
        {
            b_tspin = true;
            std::cout << "T-spin\n";
        }
    }
    uint8_t n = tetromino[0];
    uint8_t eliminated_rows = 0;
    for(int local_row = 0; local_row < n; ++local_row)
    {
        int global_row = in_tetromino.row_ + local_row;
        for(int col = 0; col < n; ++col)
        {
            uint8_t block = in_tetromino.get(local_row, col);
            if(!block) continue;
            uint8_t idx = global_row * width_ + in_tetromino.col_ + col;
            data_[idx] = block;
        }
        bool b_eliminated = true;
        for(int col = 0; col < width_; ++col)
        {
            if(!data_[global_row * width_ + col])
            {
                b_eliminated = false;
                break;
            }
        }
        if(b_eliminated)
        {
            ++eliminated_rows;
            for(int g_row = global_row ; g_row > 0; --g_row)
            {
                ::memcpy(
                    &data_[g_row * width_],
                    &data_[(g_row - 1) * width_],
                    width_ * sizeof(data_[0])
                );
            }
            std::fill(data_.begin(), data_.begin() + width_, 0);
        }
    }
    auto& cur_game = Engine::get().game_;
    cur_game.add_score(eliminated_rows, b_tspin);
}

uint8_t Board::get(uint8_t global_row, uint8_t global_col) const
{
    return data_[global_row * width_ + global_col];
}

Tetromino::Info Board::pop_next()
{
    Tetromino::Info next = next_tetromino_;
    next_tetromino_ = Tetromino::Info();
    return next;
}

void Board::swap_hold(Tetromino &to_swap)
{
    if(hold_tetromino_.color != 0)
    {
        swap(hold_tetromino_, to_swap.info_);
        to_swap.reset_pos();
    }
    else
    {
        hold_tetromino_ = to_swap.info_;
        to_swap.to_next();
    }
}

void Board::init()
{
    b_should_tick_ = false;
}

void Board::render(const Renderer& renderer) const
{
    int length_with_space = unit_length_ + 1;
    int offset_w = -(width_ / 2) * length_with_space;
    int offset_h = -(height_ / 2) * length_with_space;
    for(int row = 0; row < height_; ++row)
    {
        for(int col = 0; col < width_; ++col)
        {
            renderer.draw_rect_fill(
                {offset_w + col * length_with_space, offset_h + row * length_with_space, static_cast<int>(unit_length_), static_cast<int>(unit_length_)},
                s_colors[data_[row * width_ + col]]);
        }
    }
    // render next tetromino
    {
        length_with_space *= 0.5;
        renderer.draw_rect_fill(
            {-20 * length_with_space, 5 * length_with_space, length_with_space * 6, length_with_space * 6},
            {0, 0, 0, 255},
            AnchorPoint::TopRight);
        
        auto next_tetromino_data = Tetromino::s_tetrominoes[next_tetromino_.type];
        uint8_t n = next_tetromino_data[0];
        for(uint8_t row = 0; row < n; ++row)
        {
            for(uint8_t col = 0; col < n; ++col)
            {
                uint8_t idx = row * n + col + 1;
                if(!next_tetromino_data[idx]) continue;

                renderer.draw_rect_fill(
                    {(col - 19) * length_with_space, (row + 6) * length_with_space, static_cast<int>(unit_length_ * 0.5), static_cast<int>(unit_length_ * 0.5)},
                    s_colors[next_tetromino_data[idx] * next_tetromino_.color],
                    AnchorPoint::TopRight);
            }
        }
    }

    // render hold
    {
        renderer.draw_rect_fill(
            {20 * length_with_space, 5 * length_with_space, length_with_space * 6, length_with_space * 6},
            {0, 0, 0, 255},
            AnchorPoint::TopLeft);
        
        auto hold_tetromino_data = Tetromino::s_tetrominoes[hold_tetromino_.type];
        uint8_t n = hold_tetromino_data[0];
        for(uint8_t row = 0; row < n; ++row)
        {
            for(uint8_t col = 0; col < n; ++col)
            {
                uint8_t idx = row * n + col + 1;
                if(!hold_tetromino_data[idx]) continue;

                renderer.draw_rect_fill(
                    {(col + 21) * length_with_space, (row + 6) * length_with_space, static_cast<int>(unit_length_ * 0.5), static_cast<int>(unit_length_ * 0.5)},
                    s_colors[hold_tetromino_data[idx] * hold_tetromino_.color],
                    AnchorPoint::TopLeft);
            }
        }
    }
}
