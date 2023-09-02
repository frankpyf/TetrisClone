module;
#include <iostream>
module tetris;

static constexpr SDL_Color s_colors[] = {{0, 0, 0, 255}, {255, 0, 0, 255}, {255, 165, 0, 255}, {0, 255, 0, 255}, {0, 0, 255, 255}};

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

void Tetris::init_debug()
{
    std::cout << "size of Tetromino" << sizeof(Tetromino) <<std::endl;
    std::cout << "size of Tetris" << sizeof(Tetris) <<std::endl;
    // board_[218] = 1;
    // board_[219] = 1;
    // board_[216] = 3;
    // board_[217] = 4;

    // board_[210] = 1;
    // board_[209] = 1;
    // board_[208] = 1;
    board_[board_row_col_to_index(22, 10)] = 1;
    board_[board_row_col_to_index(22, 9)] = 1;
    board_[board_row_col_to_index(22, 8)] = 1;

    generate_tetromino(curr_tetromino_);
    generate_tetromino(next_tetromino_);
}

void Tetris::update_tetromino(float delta_time)
{
    time_passed_ += delta_time;
    // std::cout << delta_time << std::endl;
    if(time_passed_ <= s_time_to_drop[level_ - 1])
        return;
    time_passed_ = 0.0f;
    soft_drop();
    
}

void Tetris::rotate_left()
{
    curr_tetromino_.rotation = (curr_tetromino_.rotation + 3) % 4;
}

void Tetris::rotate_right()
{
    curr_tetromino_.rotation = (curr_tetromino_.rotation + 1) % 4;
}

void Tetris::hold_tetromino()
{
    Tetromino temp = curr_tetromino_;
    curr_tetromino_ = hold_tetromino_;
    hold_tetromino_ = temp;
}

void Tetris::hard_drop()
{

}

void Tetris::soft_drop()
{
    curr_tetromino_.row++;
}

void Tetris::render_all(const Renderer& renderer) const
{
    // top left:(1, 1)
    // bottom right: (10, 22)

    // render board
    uint8_t row = 3, col = 1;
    for(int i = 0; i < WIDTH * ACTUAL_HEIGHT; ++i)
    {
        if(col == WIDTH + 1)
        {
            col = 1;
            ++row;
        }
        render_block(row, col++, s_colors[board_[board_row_col_to_index(row, col)]], renderer);
    }

    // render current tetromino
    render_tetromino(renderer);

    // render upcoming tetromino
    //render_
}

void Tetris::render_tetromino(const Renderer& renderer) const
{
    const auto& tetromino = s_tetrominoes[curr_tetromino_.type];
    SDL_Color color = s_colors[curr_tetromino_.color];

    int local_row = 0, local_col = 0;
    int n = tetromino[0];

    int row = curr_tetromino_.row, col = curr_tetromino_.col;
    while(local_row < n)
    {
        uint8_t data = tetromino[tetromino_row_col_to_index(local_row, local_col, curr_tetromino_, n)];
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

void Tetris::render_block(uint8_t row, uint8_t col, const SDL_Color& color, const Renderer& renderer) const
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

void Tetris::clear_row(uint8_t row)
{
    for(int i = 1; i <= 10; ++i)
    {
        set_board(row, i, 0);
    }
    if(row == ACTUAL_HEIGHT)
    {
        bottom_row_ = (bottom_row_ + 1) % 22;
    }
}

uint8_t Tetris::board_row_col_to_index(uint8_t row, uint8_t col) const
{

    return ((bottom_row_ * WIDTH) + (row - 1) * WIDTH + col - 1) % (WIDTH * ACTUAL_HEIGHT);
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
    std::srand(std::time(nullptr));
    // color
    int color = std::rand() % NUM_COLOR + 1;
    // type
    int type = std::rand() % 7;

    int col = (WIDTH - s_tetrominoes[type][0]) / 2;
    
    in_tetromino.color = static_cast<uint8_t>(color);
    in_tetromino.type = type;
    in_tetromino.rotation = 1;
    in_tetromino.row = 1;
    in_tetromino.col = static_cast<uint8_t>(col + 1);
}