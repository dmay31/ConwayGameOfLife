#pragma once
#include <array>
#include <list>
#include <vector>

#include "config.h"

class Square {

public:
  Square();
  Square(int Col, int Row, bool proto = false);
  Square(const Square &aSquare);
  ~Square();

  static void sqawnSquare(int id);
  static void terminateSquare(int id);


  typedef struct neighbors {
    int top;
    int top_left;
    int top_right;
    int left;
    int right;
    int bottom;
    int bottom_left;
    int bottom_right;

    neighbors() {
      top = -1;
      top_left = -1;
      top_right = -1;
      left = -1;
      right = -1;
      bottom = -1;
      bottom_left = -1;
      bottom_right = -1;
    }
  } neighbors;

  int GetX();
  int GetY();
  int GetIdx();
  bool Alive();
  bool isTop();

  void setXY(unsigned int x, unsigned int y);
  void setAlive(bool alive);

  neighbors get_neighbors();
  std::list<Square> get_dead_neighbors(Square::neighbors neighborhood);
  int get_active_neighbors(Square::neighbors &neighborhood);

  bool operator==(const Square &val) {
	  bool equal = true;
	  equal &= x == val.x;
	  equal &= y == val.y;

	  return equal;
  }

  bool operator<(Square &val) { return (mWorldIdx < val.mWorldIdx); };

private:
  int x, y;
  int listIdx, mWorldIdx;
  bool isAlive;
  bool hasNeighbor;

  void addToWorld();
};
