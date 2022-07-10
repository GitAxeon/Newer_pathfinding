#include "Pathfinder.h"
#include "Color.h"

#include <SDL\SDL.h>
#include <iostream>
#include <optional>

void Pathfinder::InitializeMap(const std::vector<ColorBGR>& map)
{
	std::optional<Vec2i> start_position = std::nullopt;
	std::optional<Vec2i> end_position = std::nullopt;

	ColorBGR wall_color(0, 0, 0);
	ColorBGR start_color(0, 0, 255);
	ColorBGR end_color(255, 0, 0);

	for (size_t i = 0; i < map.size(); i++)
	{
		Vec2i pos(XFromIndex(i), YFromIndex(i));
		
		if (map.at(i) == wall_color)
		{
			m_walls.push_back(pos);
			continue;
		}

		if (map.at(i) == start_color)
		{
			if (!start_position)
			{
				start_position = pos;
				continue;
			}

			throw std::runtime_error("Multiple starting points");
		}

		if (map.at(i) == end_color)
		{
			if (!end_position)
			{
				end_position = pos;
				continue;
			}

			throw std::runtime_error("Multiple ending points");
		}
	}

	if (!start_position || !end_position)
	{
		throw std::runtime_error("Missing one of the following: start position, end position");
	}

	m_start_position = start_position.value();
	m_end_position = end_position.value();
}

std::vector<Vec2i> Pathfinder::Search(DirectionEnum num)
{
	MapNode* current = nullptr;

	std::vector<MapNode*> open_list;
	std::vector<MapNode*> closed_list;

	open_list.push_back(new MapNode(m_start_position));

	while (!open_list.empty())
	{
		std::vector<MapNode*>::iterator current_it = open_list.begin();
		current = *current_it;

		for (auto it = open_list.begin(); it != open_list.end(); it++)
		{
			auto node_ptr = *it;
			if (node_ptr->g + node_ptr->h <= current->g + current->h)
			{
				current = *it;
				current_it = it;
			}
		}

		// Goal reached
		if (current->position == m_end_position)
		{
			break;
		}

		closed_list.push_back(current);
		open_list.erase(current_it);

		std::vector<Vec2i> neighbours = GetNeighbours(current->position, num);
		
		for (size_t i = 0; i < neighbours.size(); i++)
		{
			Vec2i new_position = neighbours[i];
			auto result = std::find_if(closed_list.begin(), closed_list.end(), [&](const auto& val)
				{
				return (val->position == new_position);
				});
			if (result != closed_list.end())
			{
				continue;
			}
			
			int cost = 0;

			// Cursed, needs to be updated
			if (num == DirectionEnum::EIGHT && (new_position + Vec2i(current->position.x * -1, current->position.y * -1) == Vec2i(-1, -1) ||
				new_position + Vec2i(current->position.x * -1, current->position.y * -1) == Vec2i(+1, -1) ||
				new_position + Vec2i(current->position.x * -1, current->position.y * -1) == Vec2i(-1, +1) ||
				new_position + Vec2i(current->position.x * -1, current->position.y * -1) == Vec2i(+1, +1)))
			{
				cost = current->g + 14;
			}
			else
			{
				cost = current->g + 10;
			}

			MapNode* neighbour = nullptr;
			result = std::find_if(open_list.begin(), open_list.end(), [&](const auto& val)
				{
					return (val->position == new_position);
				});
			
			if (result == open_list.end())
			{
				// Newly discovered MapNode!
				neighbour = new MapNode(new_position, current);
				neighbour->g = cost;
				neighbour->h = DistanceToEnd(new_position, m_end_position);
				open_list.push_back(neighbour);
			}
			else
			{
				//  Found a more efficient path to existing node
				neighbour = *result;
				if (cost < neighbour->g + neighbour->h)
				{
					neighbour->parent = current;
					neighbour->g = cost;
				}
			}
		}
	}


	std::vector<Vec2i> path;
	while (current != nullptr)
	{
		path.push_back(current->position);
		current = current->parent;
	}

	for (auto it = open_list.begin(); it != open_list.end();)
	{
		delete (*it);
		it = open_list.erase(it);
	}

	for (auto it = closed_list.begin(); it != closed_list.end();)
	{
		delete (*it);
		it = closed_list.erase(it);
	}

	return (path);
}

std::vector<Vec2i> Pathfinder::GetNeighbours(Vec2i& position, DirectionEnum num)
{
	std::vector<Vec2i> neighbours;

	Vec2i directions[8] = {
		{-1, +0},
		{+1, +0},
		{+0, -1},
		{+0, +1}, // .
		{-1, -1},
		{+1, -1},
		{-1, +1},
		{+1, +1},
	};

	size_t max = (num == DirectionEnum::FOUR ? 4 : 8);
	
	for (size_t i = 0; i < max; i++)
	{
		Vec2i tmp = position + directions[i];
		if (CoordinateInBounds(tmp) && !IsWall(tmp))
		{
			neighbours.push_back(position + directions[i]);
		}
	}

	return (neighbours);
}

bool Pathfinder::IndexInBounds(int index)
{
	return (index < (m_map_width * m_map_height));
}

bool Pathfinder::CoordinateInBounds(const Vec2i& position)
{
	return (position.x > 0 && position.x < m_map_width - 1 &&
		position.y > 0 && position.y < m_map_height - 1);
}

int Pathfinder::IndexFromVec2(const Vec2i& position)
{
	if (!CoordinateInBounds(position))
	{
		throw std::runtime_error("Coordinate out of map bounds");
	}

	return (position.y * m_map_width + position.x);
}

int Pathfinder::XFromIndex(int index)
{
	if (!IndexInBounds(index))
	{
		throw std::runtime_error("Index out of bounds");
	}

	return (index % m_map_width);
}

int Pathfinder::YFromIndex(int index)
{
	if (!IndexInBounds(index))
	{
		throw std::runtime_error("Index out of bounds");
	}

	return (index / m_map_height);
}

bool Pathfinder::IsWall(const Vec2i& position)
{
	auto result = std::find(m_walls.begin(), m_walls.end(), position);

	return (result != m_walls.end());
}

int Pathfinder::DistanceToEnd(const Vec2i& start, const Vec2i& end)
{
	Vec2i tmp = { abs(start.x - end.x), abs(start.y - end.y) };
	return (10 * (tmp.x + tmp.y));
}