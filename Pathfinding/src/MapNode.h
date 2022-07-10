#pragma once
struct Vec2i
{
	int x, y;

	bool operator==(const Vec2i& other)
	{
		return (x == other.x && y == other.y);
	}


	Vec2i operator+(const Vec2i& other)
	{
		return { x + other.x, y + other.y };
	}
};

struct MapNode
{
	MapNode(Vec2i position_)
	{
		position = position_;
		g = 0;
		h = 0;
		parent = nullptr;
	}

	MapNode(Vec2i position_, MapNode* parent_)
	{
		position = position_;
		parent = parent_;
		g = 0;
		h = 0;
	}

	bool operator==(const MapNode& other)
	{
		return (position == other.position);
	}

	Vec2i position;
	float g;
	float h;
	MapNode* parent;
};

