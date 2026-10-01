#include <exception>
#include <tetris.hpp>

tetris::Piece::Piece(std::span<const Vec2> blocks, BlockColor color,
                     Parity parity)
    : rotation(Rotation::None), color(color), blocks(blocks),
      pos{Board::WIDTH / 2, Board::HEIGHT + 2}, parity(parity) {}

void tetris::Piece::rotate_counter_clockwise() {
  switch (this->rotation) {
  case Rotation::None:
    this->rotation = Rotation::Left;
    break;
  case Rotation::Left:
    this->rotation = Rotation::Flipped;
    break;
  case Rotation::Flipped:
    this->rotation = Rotation::Right;
    break;
  case Rotation::Right:
    this->rotation = Rotation::None;
    break;
  }
}

void tetris::Piece::rotate_clockwise() {
  switch (this->rotation) {
  case Rotation::None:
    this->rotation = Rotation::Right;
    break;
  case Rotation::Right:
    this->rotation = Rotation::Flipped;
    break;
  case Rotation::Flipped:
    this->rotation = Rotation::Left;
    break;
  case Rotation::Left:
    this->rotation = Rotation::None;
    break;
  }
}

// TODO: impl afine rotation for even pieces
Vec2 tetris::Piece::apply_rotation(Vec2 point) {
  switch (this->rotation) {
  case Rotation::None:
    return Vec2{point.x, point.y};
  case Rotation::Left:
    return Vec2{-point.y, point.x};
  case Rotation::Flipped:
    return Vec2{-point.x, -point.y};
  case Rotation::Right:
    return Vec2{point.y, -point.x};
  }
  std::terminate();
}

Vec2 tetris::Piece::rotation_correction() {
  if (this->parity == Parity::Odd)
    return Vec2{0, 0};
  switch (this->rotation) {
  case Rotation::None:
    return Vec2{0, 0};
  case Rotation::Left:
    return Vec2{1, 0};
  case Rotation::Flipped:
    return Vec2{1, 1};
  case Rotation::Right:
    return Vec2{0, 1};
  }
  std::terminate();
}

Vec2 tetris::Piece::get_block(size_t i) {
  auto rotated = this->apply_rotation(this->blocks[i]);
  return rotated + this->pos + this->rotation_correction();
}

int tetris::Piece::size() { return this->blocks.size(); }
