#ifndef VEC2_HPP
#define VEC2_HPP

struct Vec2 {
  Vec2();
  Vec2(int x, int y);

  int x;
  int y;

  friend Vec2 operator+(Vec2 lhs, Vec2 rhs);
  void operator+=(Vec2 other);
};

#endif
