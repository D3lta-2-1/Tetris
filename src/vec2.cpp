#include "tetris.hpp"

Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(int x, int y) : x(x), y(y) {}

Vec2 operator+(Vec2 lhs, Vec2 rhs) {
  return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

void Vec2::operator+=(Vec2 other) { *this = *this + other; }
