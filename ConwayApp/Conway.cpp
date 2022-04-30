#include "Conway.h"
#include "Square.h"
#include "world.h"

#include <vector>

using std::pair;

static ConwayEngine *instance;

ConwayEngine::ConwayEngine() {
  mWorld.sqawnSquare(340);
  // mWorld.sqawnSquare(340+160);
  // mWorld.sqawnSquare(340+160+160);
  // mWorld.sqawnSquare(340+160+1);
  // mWorld.sqawnSquare(340+160-1);
  instance = this;
  running = true;
}

ConwayEngine *ConwayEngine::getInstance() { return instance; }

void ConwayEngine::addSquares(std::list<Square> &Squares) {
  running = false;
  for (auto aSquare : Squares) {
    mWorld.sqawnSquare(aSquare);
  }
}

std::vector<pair<int, int>> ConwayEngine::GetCoordinates() {
  std::vector<pair<int, int>> activeSquares;
  for (auto idx : mWorld.activeSquares()) {
    activeSquares.push_back({mWorld[idx].GetX(), mWorld[idx].GetY()});
  }

  return activeSquares;
}

void ConwayEngine::Clear() {
	running = false;

	for (auto idx : mWorld.activeSquares()) {
		mWorld.terminateSquare(idx);
	}

}

void ConwayEngine::Run(bool run) { running = run; }

void ConwayEngine::Update() {
  std::list<Square> deadNeighbors;
  std::list<int> killList;
  std::list<int> bornList;

  if (!running) {
    return;
  }

  for (auto idx : mWorld.activeSquares()) {

    Square aSquare = mWorld[idx];
    Square::neighbors neighborSquares = mWorld.get_neighbors(aSquare);
    auto activeNeighbors = mWorld.get_active_neighbors(neighborSquares);

    //!! You cannot alter the list until your done analyzing it! :)
    if (activeNeighbors < 2 || activeNeighbors > 3) {
      killList.push_back(aSquare.GetIdx());
    }

    // Get dead squares around each live square
    std::list<Square> deadSquares = mWorld.get_dead_neighbors(neighborSquares);
    if (deadNeighbors.size() == 0) {
      deadNeighbors = deadSquares;
    } else {
      deadNeighbors.merge(deadSquares);
    }
  }

  deadNeighbors.sort();
  deadNeighbors.unique();

  for (Square deadSquare : deadNeighbors) {
    Square::neighbors neighborSquares = mWorld.get_neighbors(deadSquare);
    auto activeNeighbors = mWorld.get_active_neighbors(neighborSquares);

    if (activeNeighbors == 3) {
      bornList.push_back(deadSquare.GetIdx());
    }
  }

  for (auto idx : killList) {
    mWorld.terminateSquare(idx);
  }

  for (auto idx : bornList) {
    mWorld.sqawnSquare(idx);
  }
}
