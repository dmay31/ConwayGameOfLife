#pragma once
#include "config.h"
#include "Square.h"
#include "world.h"
#include <array>
#include <vector>

class ConwayEngine
{
public:
	ConwayEngine();
	~ConwayEngine() = default;

	static ConwayEngine * getInstance();
	void addSquares(std::list<Square>& Squares);

	
	std::vector<std::pair<int, int>> GetCoordinates();
	void Update();
	void Run(bool run);
	void Clear();

private:
    world<800, 800, 5> mWorld;

	bool running;
};