#pragma once
#include "Square.h"
#include <vector>

template<unsigned int X, unsigned int Y, unsigned int SP>
class world {
public:
	world() {
		width = X; 
		height = Y;
		count = (width / SP)*(height / SP);
		columns = width / SP;
		rows = height / SP;

		for (unsigned int i = 0; i < count; i++) {
			auto column = i % columns;
			auto row = (unsigned int)i / columns;

			mSquares[i].setXY(column, row);
		}
		
	}
	~world() = default;

	Square operator[](int i) {
		return mSquares[i];
	}


	bool isTop(Square aSquare) {
		return (aSquare.GetY() == 0);
	}

	bool isBottom(Square aSquare) {
		return (aSquare.GetY() >= height);
	}

	bool isLeft(Square aSquare) {
		return (aSquare.GetX() == 0);
	}

	bool isRight(Square aSquare) {
		return (aSquare.GetX() >= width);
	}

	std::vector<int> activeSquares() {
		return mWorldIdxs;
	}

	Square::neighbors get_neighbors(Square& aSquare) {

		Square::neighbors myNeighbors;

		if (!isTop(aSquare)) {
			myNeighbors.top = aSquare.GetIdx() - columns;

			if (!isLeft(aSquare)) {
				myNeighbors.top_left = aSquare.GetIdx() - columns - 1;
			}
			if (!isRight(aSquare)) {
				myNeighbors.top_right = aSquare.GetIdx() - columns + 1;
			}
		}

		if (!isLeft(aSquare)) {
			myNeighbors.left = aSquare.GetIdx() - 1;
		}
		if (!isRight(aSquare)) {
			myNeighbors.right = aSquare.GetIdx() + 1;
		}

		if (!isBottom(aSquare)) {
			myNeighbors.bottom = aSquare.GetIdx() + columns;

			if (!isLeft(aSquare)) {
				myNeighbors.bottom_left = aSquare.GetIdx() + columns - 1;
			}
			if (!isRight(aSquare)) {
				myNeighbors.bottom_right = aSquare.GetIdx() + columns + 1;
			}
		}

		return myNeighbors;
	}

	int get_active_neighbors(Square::neighbors &neighborhood) {
		int count = 0;
		count += neighborhood.top != -1
			? mSquares[neighborhood.top].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.top_left != -1
			? mSquares[neighborhood.top_left].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.top_right != -1
			? mSquares[neighborhood.top_right].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.bottom != -1
			? mSquares[neighborhood.bottom].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.bottom_left != -1
			? mSquares[neighborhood.bottom_left].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.bottom_right != -1
			? mSquares[neighborhood.bottom_right].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.left != -1
			? mSquares[neighborhood.left].Alive() == true ? 1 : 0
			: 0;
		count += neighborhood.right != -1
			? mSquares[neighborhood.right].Alive() == true ? 1 : 0
			: 0;

		return count;
	}

	std::list<Square> get_dead_neighbors(Square::neighbors neighborhood) {
		std::list<Square> deadNeighbors;

		if ((neighborhood.top != -1) && mSquares[neighborhood.top].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.top]);
		}
		if ((neighborhood.top_left != -1) &&
			mSquares[neighborhood.top_left].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.top_left]);
		}
		if ((neighborhood.top_right != -1) &&
			mSquares[neighborhood.top_right].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.top_right]);
		}
		if ((neighborhood.right != -1) &&
			mSquares[neighborhood.right].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.right]);
		}
		if ((neighborhood.left != -1) &&
			mSquares[neighborhood.left].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.left]);
		}
		if ((neighborhood.bottom != -1) &&
			mSquares[neighborhood.bottom].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.bottom]);
		}
		if ((neighborhood.bottom_left != -1) &&
			mSquares[neighborhood.bottom_left].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.bottom_left]);
		}
		if ((neighborhood.bottom_right != -1) &&
			mSquares[neighborhood.bottom_right].Alive() == false) {
			deadNeighbors.push_back(mSquares[neighborhood.bottom_right]);
		}

		deadNeighbors.sort();
		return deadNeighbors;
	}

	void sqawnSquare(int id) {
		mSquares[id].setAlive(true);
		mWorldIdxs.push_back(id);
	}

	void sqawnSquare(Square aSquare) {
		auto idx = ((aSquare.GetY() * columns) + aSquare.GetX());
		mSquares[idx].setAlive(true);
		mWorldIdxs.push_back(idx);
	}

	void terminateSquare(int id) {
		mSquares[id].setAlive(false);
		mWorldIdxs.erase(std::remove(mWorldIdxs.begin(), mWorldIdxs.end(), id), mWorldIdxs.end());
	}

private:
	int width;
	int height;
	unsigned int count;
	int columns;
	int rows;

	std::array<Square, (X*Y) / (SP*SP)> mSquares;
	std::vector<int> mWorldIdxs;
};