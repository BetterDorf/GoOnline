#include  "Goban.h"

#include <algorithm>
#include <string>

Goban::Goban(const int x, const int y) : x_(x), y_(y)
{
	stones_ = std::vector<std::vector<Stone>>(x);
	for (auto& column : stones_)
	{
		column = std::vector<Stone>(y);
		for (auto& stone : column)
		{
			stone = empty;
		}
	}
}

bool Goban::PlayStone(const int x, const int y, const Stone team)
{
	// Check if the play is legal
	// Check other stone present
	if (stones_.at(x).at(y) != empty)
		return  false;

	// TODO Check KO

	// Add the stone
	stones_.at(x).at(y) = team;

	// Check if other stones died as a result
	const auto neighbours = GetNeighbours(x, y);
	for (const auto& neighbour : neighbours)
	{
		Stone stone = StoneAt(neighbour);
		if (stone != team && stone != empty)
		{
			auto group = GetGroup(neighbour);
			if (CountLiberties(group) == 0)
			{
				KillGroup(group);
			}
		}
	}

	// Check self-capture
	if (CountLiberties(x, y) == 0)
	{
		stones_.at(x).at(y) = empty;
		return  false;
	}

	return true;
}

int Goban::CountLiberties(int x, int y)
{
	return CountLiberties(GetGroup(x, y));
}

int Goban::CountLiberties(const std::vector<std::pair<int, int>>& group)
{
	std::vector<std::pair<int, int>> liberties;
	
	for (auto& stone : group)
	{
		auto neighbours = GetNeighbours(stone);
		for(auto& neighbour : neighbours)
		{
			if (StoneAt(neighbour) == empty) 
			{
				liberties.emplace_back(neighbour);
			}
		}
	}

	return liberties.size();
}

std::vector<std::pair<int, int>> Goban::GetGroup(int x, int y)
{
	return GetGroup(std::pair<int, int> (x, y));
}

std::vector<std::pair<int, int>> Goban::GetGroup(const std::pair<int, int> pos)
{
	std::vector<std::pair<int, int>> group;

	const int team = StoneAt(pos);

	if (team == 0)
		return  group;

	group.emplace_back(pos);

	auto stonesAdded = std::vector<std::pair<int, int>>();
	auto stonesToCheck = std::vector<std::pair<int, int>>();
	stonesToCheck.emplace_back(pos);

	// propagate from the stones that have just been added to the group
	while (stonesToCheck.empty() == false)
	{
		for (const auto& addedStone : stonesToCheck)
		{
			for (auto& neighbourPos : GetNeighbours(addedStone))
			{
				if (StoneAt(neighbourPos) == team)
				{
					if (std::find(group.begin(), group.end(), neighbourPos) != group.end())
					{
						group.emplace_back(neighbourPos);
						stonesAdded.emplace_back(neighbourPos);
					}
				}
			}
		}

		stonesToCheck = std::vector<std::pair<int, int>>(stonesAdded);
		stonesAdded.clear();
	}

	return group;
}

std::vector<std::pair<int, int>> Goban::GetNeighbours(int x, int y) const
{
	std::vector<std::pair<int, int>> neighbours;

	if (x + 1 < x_)
	{
		neighbours.emplace_back(std::pair<int, int>(x + 1, y));
	}

	if (x - 1 >= 0)
	{
		neighbours.emplace_back(std::pair<int, int>(x - 1, y));
	}

	if (y - 1 >= 0)
	{
		neighbours.emplace_back(std::pair<int, int>(x, y - 1));
	}

	if (y + 1 < y_)
	{
		neighbours.emplace_back(std::pair<int, int>(x, y + 1));
	}

	return  neighbours;
}

std::vector<std::pair<int, int>> Goban::GetNeighbours(const std::pair<int, int> pos) const
{
	return GetNeighbours(pos.first, pos.second);
}

Stone Goban::StoneAt(const int x, const int y)
{
	return stones_.at(x).at(y);
}

Stone Goban::StoneAt(const std::pair<int, int> pos)
{
	return StoneAt(pos.first, pos.second);
}

std::string Goban::ToString()
{
	std::string s;

	for (auto it = stones_.rbegin(); it != stones_.rend(); ++it)
	{
		for (const auto& stone : *it)
		{
			s += std::to_string(stone);
		}

		s += "\n";
	}

	s += "White captures : ";
	s += std::to_string(wCaptures_);
	s += "\t";
	s += "Black captures : ";
	s += std::to_string(bCaptures_);

	return s;
}

void Goban::KillGroup(const std::vector<std::pair<int, int>>& group)
{
	for (auto& stone : group)
	{
		if (StoneAt(stone) == 1)
		{
			wCaptures_++;
		}
		else
		{
			bCaptures_++;
		}

		stones_.at(stone.first).at(stone.second) = empty;
	}
}
