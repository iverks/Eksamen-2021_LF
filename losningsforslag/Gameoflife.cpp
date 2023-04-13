#include "Gameoflife.h"

#include <algorithm>
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include "AnimationWindow.h"

using namespace TDT4102;

Cell::Cell(Point pos, int size)
    : state(State::Dead), pos{pos}, size{size} {}

// TASK
int Cell::get_value() const {
    // BEGIN: C1
    return static_cast<int>(state);
    // END: C1
}

// TASK
void Cell::kill() {
    // BEGIN: C3
    state = State::Dead;
    // END: C3
}

// TASK
void Cell::resurrect() {
    // BEGIN: C4
    state = State::Live;
    // END: C4
}

// TASK
void Cell::set_state(char c) {
    // BEGIN: C5
    state = char_to_state.at(c);
    // END: C5
}

// TASK
std::istream& operator>>(std::istream& is, Cell& cell) {
    // BEGIN: C6
    char state;
    is >> state;
    cell.set_state(state);
    return is;
    // END: C6
}

// TASK
bool Cell::is_alive() const {
    // BEGIN: C7
    return state == State::Live;
    // END: C7
}

// TASK
char Cell::as_char() const {
    // BEGIN: C8
    return chars[get_value()];
    // END: C8
}

// TASK
Gameoflife::Gameoflife(int x_cells,
                       int y_cells,
                       const std::string& window_title)
    : AnimationWindow{50, 50, x_cells * cell_size + 5 * button_width + 2 * margin,
                      y_cells * cell_size + 2 * margin,
                      window_title},
      x_cells{x_cells},
      y_cells{y_cells},

      step_btn{{x_cells * cell_size + 2 * margin + 3 * button_width, 3 * margin}, button_width, button_height, "Step"},
      steps_input{{x_cells * cell_size + 2 * margin + button_width, 3 * margin}, button_width * 2, button_height, "Number of steps"},
      filename_input{{x_cells * cell_size + 2 * margin + button_width, button_height + 5 * margin}, button_width * 2, button_height, "Filename"},
      load_btn{{x_cells * cell_size + 2 * margin + 3 * button_width, button_height + 5 * margin}, button_width, button_height, "Load"},
      save_btn{{x_cells * cell_size + 2 * margin + 3 * button_width, 2 * button_height + 7 * margin}, button_width, button_height, "Save"},
      play_pause_btn{{x_cells * cell_size + margin + button_width, 2 * button_height + 7 * margin}, button_width * 2, button_height, "Play/Pause"} {
    // Provided (asserts and attach() calls)
    // Asserts are here to make sure any unintended changes to
    // compile-time constants makes the exercise less hard to debug.
    assert(x_cells > 0);
    assert(y_cells > 0);
    assert(button_width > 0);
    assert(margin > 0);

    step_btn.setCallback(std::bind(&Gameoflife::step_pressed, this));
    add(step_btn);
    play_pause_btn.setCallback(std::bind(&Gameoflife::play_pause, this));
    add(play_pause_btn);
    load_btn.setCallback(std::bind(&Gameoflife::load_state, this));
    add(load_btn);
    save_btn.setCallback(std::bind(&Gameoflife::save_state, this));
    add(save_btn);

    add(filename_input);
    add(steps_input);

    // BEGIN: G1

    // END: G1
    for (int i = 0; i < y_cells; i++) {
        get_current_grid().emplace_back();
        for (int j = 0; j < x_cells; j++) {
            int y_pos = i * cell_size + margin;
            int x_pos = j * cell_size + margin;
            // auto c = grid[current_grid][i][j];
            get_current_grid().back().emplace_back(Point{x_pos, y_pos}, cell_size);
        }
    }
    get_scratch_grid() = get_current_grid();
    load("flower.cgol");
}

// TASK
std::istream& operator>>(std::istream& is, Gameoflife& gameoflife) {
    // BEGIN: G2
    for (auto& row : gameoflife.get_current_grid()) {
        for (auto&& cell : row) {
            is >> cell;
        }
    }
    return is;
    // END: G2
}

// TASK
void Gameoflife::load(const std::string& filename) {
    // BEGIN: G3
    std::ifstream ifs{filename};

    if (!ifs) {
        throw std::runtime_error{"Could not load a Game of life state from '" + filename + "'."};
    }
    ifs >> *this;
    step();
    // END: G3
}

// TASK

void Gameoflife::drawState() {
    this->next_frame();
    // get_scratch_grid();

    for (int row = 0; row < get_current_grid().size(); ++row) {
        for (int col = 0; col < get_current_grid().back().size(); ++col) {
            int y_pos = row * cell_size + margin;
            int x_pos = col * cell_size + margin;
            if (get_current_grid()[row][col].is_alive()) {
                draw_rectangle({x_pos, y_pos}, cell_size, cell_size, Color::white);
            } else {
                draw_rectangle({x_pos, y_pos}, cell_size, cell_size, Color::black);
            }
        }
    }
}

void Gameoflife::step() {
    // BEGIN: G4

    // get_scratch_grid();
    // auto start = std::chrono::high_resolution_clock::now();

    for (int row = 0; row < get_current_grid().size(); ++row) {
        for (int col = 0; col < get_current_grid().back().size(); ++col) {
            // How many neighbours are currently alive?
            int live_neighbours = 0;
            // We go through all neighbouring cells
            for (int y = -1; y <= 1; ++y) {
                for (int x = -1; x <= 1; ++x) {
                    // Except the current cell
                    if (!(x == 0 && y == 0)) {
                        // And add the integer value of the cell's state
                        int lookup_y = ((row + y) + y_cells) % y_cells;
                        int lookup_x = ((col + x) + x_cells) % x_cells;
                        live_neighbours +=
                            get_current_grid()[lookup_y][lookup_x].get_value();
                    }
                }
            }
            int y_pos = row * cell_size + margin;
            int x_pos = col * cell_size + margin;

            if (get_current_grid()[row][col].is_alive() &&
                (live_neighbours == 2 || live_neighbours == 3)) {
                // 1. Any live cell with two or three live neighbours
                // survives.
                get_scratch_grid()[row][col].resurrect();
                // draw_rectangle({x_pos, y_pos}, cell_size, cell_size, Color::white);
                //  Pass - keep alive
            } else if (!get_current_grid()[row][col].is_alive() &&
                       live_neighbours == 3) {
                // 2. Any dead cell with three live neighbours becomes a
                // live cell.
                get_scratch_grid()[row][col].resurrect();
                // draw_rectangle({x_pos, y_pos}, cell_size, cell_size, Color::white);
            } else {
                // 3. All other live cells die in the next generation.
                // Similarly, all other dead cells stay dead.
                get_scratch_grid()[row][col].kill();
                // draw_rectangle({x_pos, y_pos}, cell_size, cell_size, Color::black);
            }
        }
    }

    std::swap(current_grid, scratch_grid);

    // END: G4
}

// TASK
void Gameoflife::step(int steps) {
    // BEGIN: G5
    for (int i = 0; i < steps; i++) {
        step();
    }
    // END: G5
}

// TASK
Cell* Gameoflife::cell_at_pos(Point pos) {
    // BEGIN: E1
    int row = (pos.y - margin) / cell_size;
    int col = (pos.x - margin) / cell_size;
    std::cout << row << ", " << col << '\n';

    if (row >= y_cells || col >= x_cells || pos.y - margin < 0 ||
        pos.x - margin < 0)
        return nullptr;

    return &get_current_grid()[row][col];
    // END: E1
}

// TASK
void Cell::toggle() {
    // BEGIN: E2
    if (state == State::Live) {
        kill();
    } else {
        resurrect();
    }
    // END: E2
}

// TASK
bool Gameoflife::toggle_cell(Point pos) {
    // BEGIN: E3
    if (auto cell = cell_at_pos(pos)) {
        cell->toggle();
        if (cell->get_value() == 0) {
            draw_rectangle(cell->get_pos(), cell_size, cell_size, Color::black);
        } else {
            draw_rectangle(cell->get_pos(), cell_size, cell_size, Color::white);
        }
        return true;
    }

    return false;
    // END: E3
}

// Provided
std::ostream& operator<<(std::ostream& os, const Cell& cell) {
    return os << cell.as_char();
}

// Provided
void Gameoflife::save(const std::string& filename) {
    constexpr std::string_view ext = ".cgol";
    if (filename.size() < ext.size() ||
        filename.substr(filename.size() - 5, 5) != ext) {
        throw std::runtime_error{"'" + filename +
                                 "' does not have the correct file extension: "
                                 "'.cgol' or is too short."};
    }

    std::ofstream ofs{filename};
    if (!ofs) {
        throw std::runtime_error{"Could not save the CGoL state to '" +
                                 filename + "'."};
    }

    ofs << *this;
}

// Provided
std::ostream& operator<<(std::ostream& os, const Gameoflife& gameoflife) {
    for (auto& row : gameoflife.get_current_grid()) {
        for (auto&& cell : row) {
            os << cell;
        }
        os << '\n';
    }

    return os;
}

// Provided
void Gameoflife::step_pressed() {
    int steps;
    try {
        steps = stoi(steps_input.getText());
    } catch (std::invalid_argument) {
        steps = 1;
    }
    steps = std::clamp(steps, min_steps, max_steps);
    step(steps);
}

// Provided
void Gameoflife::load_state() {
    const std::string& filename = filename_input.getText();
    try {
        load(filename);
    } catch (const std::runtime_error& e) {
        alert(e.what());
    }
}

// Provided
void Gameoflife::save_state() {
    const std::string& filename = filename_input.getText();
    try {
        save(filename);
    } catch (const std::runtime_error& e) {
        alert(e.what());
    }
}

Gameoflife::Grid& Gameoflife::get_current_grid() { return grid[current_grid]; }
Gameoflife::Grid& Gameoflife::get_scratch_grid() { return grid[scratch_grid]; }
const Gameoflife::Grid& Gameoflife::get_current_grid() const {
    return grid[current_grid];
}
const Gameoflife::Grid& Gameoflife::get_scratch_grid() const {
    return grid[scratch_grid];
}

void Gameoflife::alert(const std::string& message) {
    AnimationWindow::show_info_dialog(message);
}

int Gameoflife::click_handler(Point pos) { return toggle_cell(pos); }

void Gameoflife::play_pause() {
    playing = !playing;
}

void Gameoflife::run() {
    Uint64 last_updated = 0;
    while (!should_close()) {
        if (left_mouse_clicked()) {
            click_handler(get_mouse_coordinates());
        }
        drawState();
        if (playing) {
            Uint64 current = SDL_GetTicks64();
            if (current - last_updated > animation_interval) {
                step(1);
                last_updated = current;
            }
        }
    }
}

bool Gameoflife::left_mouse_clicked() {
    static bool was_down = false;
    bool is_down = is_left_mouse_button_down();
    if (is_down && was_down) {
        return false;
    } else if (is_down) {
        was_down = true;
        return true;
    } else {
        was_down = false;
        return false;
    }
}

Gameoflife::~Gameoflife() {
    playing = false;
}
