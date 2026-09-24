#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_scancode.h"
#include <chrono>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "tetris.hpp"
#include <memory>

using namespace std::string_literals;

namespace sdl {
using Window = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using Renderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
} // namespace sdl

constexpr size_t BLOCK_SIZE = 20;

namespace chrono {
using namespace std::chrono;
}

class App {
  tetris::Game game;
  sdl::Window window;
  sdl::Renderer renderer;
  chrono::milliseconds tick_lenght;
  chrono::time_point<std::chrono::high_resolution_clock> last_tick_date;

public:
  App()
      : window(nullptr, &SDL_DestroyWindow),
        renderer(nullptr, &SDL_DestroyRenderer),
        tick_lenght(chrono::milliseconds(400)),
        last_tick_date(chrono::high_resolution_clock::now()) {
    this->window.reset(
        SDL_CreateWindow("tetris", tetris::Board::WIDTH * BLOCK_SIZE,
                         (tetris::Board::HEIGHT + 1) * BLOCK_SIZE, 0));

    if (this->window == nullptr)
      throw std::runtime_error("unable to open a Window:"s + SDL_GetError());
    this->renderer.reset(SDL_CreateRenderer(this->window.get(), nullptr));
    if (this->renderer == nullptr)
      throw std::runtime_error("unable to create a renderer"s + SDL_GetError());
    // TODO: add Vsync support.
    /*if (SDL_SetRenderVSync(this->renderer.get(), SDL_RENDERER_VSYNC_ADAPTIVE))
      throw std::runtime_error("failed to turn on VSYNC"s + SDL_GetError());*/
  }

  void set_draw_color(tetris::BlockColor color) {
    switch (color) {
    case tetris::BlockColor::Empty:
      SDL_SetRenderDrawColor(this->renderer.get(), 0, 0, 0, 255);
      break;
    case tetris::BlockColor::Cyan:
      SDL_SetRenderDrawColor(this->renderer.get(), 0, 255, 255, 255);
      break;
    case tetris::BlockColor::Blue:
      SDL_SetRenderDrawColor(this->renderer.get(), 0, 0, 255, 255);
      break;
    case tetris::BlockColor::Orange:
      SDL_SetRenderDrawColor(this->renderer.get(), 255, 165, 0, 255);
      break;
    case tetris::BlockColor::Yellow:
      SDL_SetRenderDrawColor(this->renderer.get(), 225, 255, 0, 255);
      break;
    case tetris::BlockColor::Green:
      SDL_SetRenderDrawColor(this->renderer.get(), 0, 255, 0, 255);
      break;
    case tetris::BlockColor::Purple:
      SDL_SetRenderDrawColor(this->renderer.get(), 128, 0, 128, 255);
      break;
    case tetris::BlockColor::Red:
      SDL_SetRenderDrawColor(this->renderer.get(), 255, 0, 0, 255);
      break;
    }
  }

  void draw_board() {
    for (auto y = 0; y < tetris::Board::HEIGHT; y++) {
      for (auto x = 0; x < tetris::Board::WIDTH; x++) {
        auto block = this->game.board.get({x, y});
        if (block.is_empty())
          continue;
        float screen_x = x * BLOCK_SIZE;
        float screen_y = (tetris::Board::HEIGHT - y) * BLOCK_SIZE;
        SDL_FRect rect = {screen_x, screen_y, BLOCK_SIZE, BLOCK_SIZE};
        this->set_draw_color(block.color);
        SDL_RenderFillRect(this->renderer.get(), &rect);
      }
    }
  }

  void draw_piece() {
    this->set_draw_color(this->game.piece.color);
    auto block_count = this->game.piece.size();
    for (auto i = 0; i < block_count; i++) {
      auto pos = this->game.piece.get_block(i);
      float screen_x = pos.x * BLOCK_SIZE;
      float screen_y = (tetris::Board::HEIGHT - pos.y) * BLOCK_SIZE;
      SDL_FRect rect = {screen_x, screen_y, BLOCK_SIZE, BLOCK_SIZE};
      SDL_RenderFillRect(this->renderer.get(), &rect);
    }
  }

  void draw_fast_placement() {
    SDL_SetRenderDrawColor(this->renderer.get(), 60, 60, 60, 255);
    auto block_count = this->game.fast_placemet.size();
    for (auto i = 0; i < block_count; i++) {
      auto pos = this->game.fast_placemet.get_block(i);
      float screen_x = pos.x * BLOCK_SIZE;
      float screen_y = (tetris::Board::HEIGHT - pos.y) * BLOCK_SIZE;
      SDL_FRect rect = {screen_x, screen_y, BLOCK_SIZE, BLOCK_SIZE};
      SDL_RenderFillRect(this->renderer.get(), &rect);
    }
  }

  SDL_AppResult on_iterate() {
    auto now = chrono::high_resolution_clock::now();
    auto elpased = now - this->last_tick_date;
    if (elpased > this->tick_lenght) {
      this->last_tick_date = now;
      if (this->game.tick()) {
        this->game = tetris::Game();
      }
    }

    SDL_SetRenderDrawColor(this->renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(this->renderer.get());
    this->draw_fast_placement();
    this->draw_piece();
    this->draw_board();
    SDL_RenderPresent(this->renderer.get());
    return SDL_APP_CONTINUE;
  };

  void key_pressed(SDL_Scancode code) {
    switch (code) {
    case SDL_SCANCODE_A:
      this->game.left();
      break;
    case SDL_SCANCODE_D:
      this->game.right();
      break;
    case SDL_SCANCODE_Q:
      this->game.rotate_clockwise();
      break;
    case SDL_SCANCODE_E:
      this->game.rotate_counter_clockwise();
      break;
    case SDL_SCANCODE_S:
      if (this->game.accept_fast_placement()) {
        this->game = tetris::Game();
      }
      break;
    default:
      break;
    }
  }

  SDL_AppResult on_event(SDL_Event &event) {
    switch (event.type) {
    case SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      return SDL_APP_SUCCESS;
    case SDL_EventType::SDL_EVENT_KEY_DOWN:
      this->key_pressed(event.key.scancode);
      break;
    default:
      break;
    }
    return SDL_APP_CONTINUE;
  }
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  try {
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO))
      throw std::runtime_error(std::string{"failed to init SDL: "} +
                               SDL_GetError());

    auto app = std::make_unique<App>();
    *appstate = app.release();

  } catch (const std::exception &error) {
    std::cerr << "error while starting, " << error.what() << std::endl;
    return SDL_APP_FAILURE;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto app = static_cast<App *>(appstate);
  return app->on_iterate();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto app = static_cast<App *>(appstate);
  return app->on_event(*event);
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  std::unique_ptr<App> app{static_cast<App *>(appstate)};
}
