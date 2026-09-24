#include "../tetris.hpp"
#include <array>
#include <random>

namespace pieces {
static std::array I = {Vec2{-1, 0}, Vec2{0, 0}, Vec2{1, 0}, Vec2{2, 0}};
static std::array O = {Vec2{0, 0}, Vec2{0, 1}, Vec2{1, 0}, Vec2{1, 1}};
static std::array T = {Vec2{-1, 0}, Vec2{0, 0}, Vec2{1, 0}, Vec2{0, 1}};
static std::array L = {Vec2{-1, 0}, Vec2{0, 0}, Vec2{1, 0}, Vec2{1, 1}};
static std::array J = {Vec2{-1, 0}, Vec2{0, 0}, Vec2{1, 0}, Vec2{-1, 1}};
static std::array S = {Vec2{-1, 0}, Vec2{0, 0}, Vec2{0, 1}, Vec2{1, 1}};
static std::array Z = {Vec2{-1, 1}, Vec2{0, 1}, Vec2{0, 0}, Vec2{1, 0}};
}; // namespace pieces

using Parity = tetris::Piece::Parity;
static std::array PIECES{
    tetris::Piece(pieces::I, tetris::BlockColor::Cyan, Parity::Even),
    tetris::Piece(pieces::O, tetris::BlockColor::Yellow, Parity::Even),
    tetris::Piece(pieces::T, tetris::BlockColor::Purple, Parity::Odd),
    tetris::Piece(pieces::L, tetris::BlockColor::Orange, Parity::Odd),
    tetris::Piece(pieces::J, tetris::BlockColor::Blue, Parity::Odd),
    tetris::Piece(pieces::S, tetris::BlockColor::Green, Parity::Odd),
    tetris::Piece(pieces::Z, tetris::BlockColor::Red, Parity::Odd),
};

tetris::Game::Game() : piece(PIECES[0]), board(), fast_placemet(PIECES[0]) {
  std::random_device rd;
  this->mt19937 = std::mt19937(rd());
  this->distrib = std::uniform_int_distribution<>(0, PIECES.size() - 1);
  this->piece = this->get_a_new_piece();
  this->replace_fast_piece();
}

tetris::Piece tetris::Game::get_a_new_piece() {
  return PIECES[this->distrib(this->mt19937)];
  this->replace_fast_piece();
}

void tetris::Game::rotate_clockwise() {
  Piece copy = this->piece;
  copy.rotate_counter_clockwise();
  if (!this->board.collide(copy)) {
    this->piece = copy;
  }
  this->replace_fast_piece();
}

void tetris::Game::rotate_counter_clockwise() {
  Piece copy = this->piece;
  copy.rotate_clockwise();
  if (!this->board.collide(copy)) {
    this->piece = copy;
  }
  this->replace_fast_piece();
}

void tetris::Game::left() {
  Piece copy = this->piece;
  copy.pos += Vec2{-1, 0};
  if (!this->board.collide(copy)) {
    this->piece = copy;
  }
  this->replace_fast_piece();
}

void tetris::Game::right() {
  Piece copy = this->piece;
  copy.pos += Vec2{1, 0};
  if (!this->board.collide(copy)) {
    this->piece = copy;
  }
  this->replace_fast_piece();
}

void tetris::Game::replace_fast_piece() {
  auto copy = this->piece;
  do {
    copy.pos.y--;
  } while (!this->board.collide(copy));
  copy.pos.y++;
  this->fast_placemet = copy;
}

bool tetris::Game::accept_fast_placement() {
  if (this->board.place(this->fast_placemet))
    return true; // we overfilled the board
  this->board.clear_complete_raw();
  this->get_a_new_piece();
  this->piece = this->get_a_new_piece();
  this->replace_fast_piece();
  return false;
}

// move the piece down, if it colide with the grid, place the piece in the grid
bool tetris::Game::tick() {
  Piece copy = this->piece;
  copy.pos += Vec2{0, -1};
  if (!this->board.collide(copy)) {
    this->piece = copy;
    return false;
  }
  if (this->board.place(this->piece))
    return true; // we overfilled the board

  this->board.clear_complete_raw();
  this->piece = this->get_a_new_piece();
  this->replace_fast_piece();
  return false;
}
