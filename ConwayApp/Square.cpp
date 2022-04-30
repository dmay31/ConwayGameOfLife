#include "Square.h"
#include <list>

// Default Constructor
Square::Square() {
  x = 0;
  y = 0;
  isAlive = false;
  hasNeighbor = false;
  addToWorld();
}

// Copy Constructor
Square::Square(const Square &aSquare) {
  x = aSquare.x;
  y = aSquare.y;
  isAlive = aSquare.isAlive;
  mWorldIdx = aSquare.mWorldIdx;
}

Square::Square(int Col, int Row, bool proto) {
  x = Col;
  y = Row;

  isAlive = proto;
  addToWorld();
}

Square::~Square() {}

int Square::GetX() { return x; }

int Square::GetY() { return y; }

int Square::GetIdx() { return mWorldIdx; }

bool Square::Alive() { return isAlive; }

void Square::setXY(unsigned int x, unsigned int y) {
  this->x = x;
  this->y = y;
  addToWorld();
}

void Square::setAlive(bool alive) { isAlive = alive; }

void Square::addToWorld() { mWorldIdx = ((y * ROW_SQ) + x); }
