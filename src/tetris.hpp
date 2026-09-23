#pragma once
#include <array>
#include <cstddef>
#include <random>
#include <span>
#include <vector>

struct Vec2 {
  Vec2();
  Vec2(int x, int y);

  int x;
  int y;

  friend Vec2 operator+(Vec2 lhs, Vec2 rhs);
  void operator+=(Vec2 other);
};

namespace tetris {
enum class BlockColor {
  Empty,
  Cyan,
  Blue,
  Orange,
  Yellow,
  Green,
  Purple,
  Red,
};

class Block {
public:
  Block();
  Block(BlockColor color);
  BlockColor color;
  bool is_empty() { return this->color == BlockColor::Empty; }
};

// constexpr std::array<Vec2, 2>

struct Piece {
  enum class Rotation { None, QuarterOfATurn, HalfATurn, ThreeQuarterOfATurn };
  enum class Parity { Odd, Even };
  Rotation rotation;
  Parity parity;
  Vec2 pos;
  std::span<const Vec2> blocks;
  Vec2 apply_rotation(Vec2 point);
  Vec2 rotation_correction();

public:
  BlockColor color;
  Piece(std::span<const Vec2> blocks, BlockColor color, Parity partity);
  void rotate_counter_clockwise();
  void rotate_clockwise();
  Vec2 get_block(size_t i);
  int size();
};

class Board {
public:
  static constexpr size_t WIDTH = 10;
  static constexpr size_t HEIGHT = 20;

  Board();
  Block get(Vec2 point);
  bool collide(Piece &piece);
  bool within_corridor(Vec2 pos);
  bool within_borders(Vec2 pos);
  bool place(Piece piece);
  void clear_complete_raw();

private:
  std::vector<std::array<Block, WIDTH>> grid;
};

struct Game {
  std::mt19937 mt19937;
  std::uniform_int_distribution<> distrib;
  tetris::Board board;
  tetris::Piece piece;

  Piece get_a_new_piece();
  Game();
  void rotate_clockwise();
  void rotate_counter_clockwise();
  void left();
  void right();
  void bottom();
  /// return true if the game is over
  bool tick();
};
} // namespace tetris
