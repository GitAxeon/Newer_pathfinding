#pragma once
#include "MapNode.h"
#include "Color.h"

#include <optional>
#include <vector>

enum class DirectionEnum { FOUR = 0, EIGHT = 1};

class Pathfinder
{
public:
	Pathfinder() = default;
	~Pathfinder() = default;

	void InitializeMap(const std::vector<ColorBGR>& map);
	std::vector<Vec2i> Search(DirectionEnum num = DirectionEnum::FOUR);

	int IndexFromVec2(const Vec2i& position);
private:
	std::vector<Vec2i> GetNeighbours(Vec2i& position, DirectionEnum num = DirectionEnum::FOUR);
	bool IndexInBounds(int index);
	bool CoordinateInBounds(const Vec2i& position);
	int XFromIndex(int index);
	int YFromIndex(int index);
	bool IsWall(const Vec2i& position);
	int DistanceToEnd(const Vec2i& start, const Vec2i& end);

public:
	int m_map_width = 0;
	int m_map_height = 0;
	Vec2i m_start_position = { 0, 0 };
	Vec2i m_end_position = { 0, 0 };
	std::vector<Vec2i> m_walls;
};

