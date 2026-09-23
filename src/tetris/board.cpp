#include "../tetris.hpp"
#include <array>
#include <vector>

tetris::Block::Block() : color(BlockColor::Empty) {}

tetris::Block::Block(tetris::BlockColor color) : color(color) {}

tetris::Board::Board() : grid() {
  for (int i = 0; i < Board::HEIGHT; i++) {
    std::array<Block, Board::WIDTH> row;
    row.fill(tetris::Block());
    this->grid.push_back(row);
  }
}

tetris::Block tetris::Board::get(Vec2 pos) { return this->grid[pos.y][pos.x]; }

bool tetris::Board::within_corridor(Vec2 pos) {
  // Height isn't check
  return 0 <= pos.x && pos.x < WIDTH && 0 <= pos.y;
}

bool tetris::Board::within_borders(Vec2 pos) {
  return 0 <= pos.x && pos.x < WIDTH && 0 <= pos.y && pos.y < HEIGHT;
}

bool tetris::Board::collide(tetris::Piece &piece) {
  for (int i = 0; i < piece.size(); i++) {
    Vec2 block = piece.get_block(i);
    if (!this->within_corridor(block))
      return true;
    if (this->within_borders(block) && !this->get(block).is_empty())
      return true;
  }
  return false;
}

bool tetris::Board::place(Piece piece) {
  for (int i = 0; i < piece.size(); i++) {
    Vec2 block = piece.get_block(i);
    if (!this->within_borders(block))
      return true;
    this->grid[block.y][block.x] = tetris::Block(piece.color);
  }
  return false;
}

bool is_raw_filled(std::array<tetris::Block, tetris::Board::WIDTH> &row) {
  for (auto block : row) {
    if (block.is_empty())
      return false;
  }
  return true;
}

void tetris::Board::clear_complete_raw() {
  for (auto it = this->grid.begin(); it != grid.end();) {
    if (is_raw_filled(*it)) {
      it = this->grid.erase(it);
    } else {
      it++;
    }
  }
  for (auto i = this->grid.size(); i < Board::HEIGHT; i++) {
    std::array<Block, Board::WIDTH> row;
    row.fill(tetris::Block());
    this->grid.push_back(row);
  }
}
