#include  "Goban.h"

#include <algorithm>
#include <string>

namespace golc
{

	Goban::Goban(const int x, const int y) : x_(x), y_(y)
	{
		stones_ = Board(x);
		for (auto& column : stones_)
		{
			column = std::vector<Stone>(y);
			for (auto& stone : column)
			{
				stone = empty;
			}
		}

		previousBoardState_ = Board(stones_);
		prviousBoardWithSamePlayer_ = Board(previousBoardState_);
	}

	Goban::Goban(int x, int y, double komi) : Goban(x,y)
	{
		komi_ = komi;
	}

	bool Goban::PlayStone(const int x, const int y, const Stone team)
	{
		// Check if the play is legal
		// Check other stone present
		if (stones_.at(x).at(y) != empty)
			return  false;

		previousBoardState_ = Board(stones_);

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

		// Check for KO
		bool identical = true;
		for (int x = 0; x < x_; x++)
		{
			for (int y = 0; y < y_; y++)
			{
				if (stones_.at(x).at(y) != prviousBoardWithSamePlayer_.at(x).at(y))
				{
					identical = false;
					break;
				}
			}

			if (identical == false)
				break;
		}

		if (identical)
		{
			// Revert to before the move
			stones_ = Board(previousBoardState_);

			return false;
		}

		// Update the previous boards
		prviousBoardWithSamePlayer_ = Board(previousBoardState_);

		return true;
	}

	int Goban::CountLiberties(int x, int y)
	{
		return CountLiberties(GetGroup(x, y));
	}

	int Goban::CountLiberties(const Group& group)
	{
		Group liberties;
	
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

	Group Goban::GetGroup(int x, int y)
	{
		return GetGroup(Coord (x, y));
	}

	Group Goban::GetGroup(const Coord pos)
	{
		Group group;

		const int team = StoneAt(pos);

		if (team == 0)
			return group;

		group.emplace_back(pos);

		auto stonesAdded = Group();
		auto stonesToCheck = Group();
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
						if (std::find(group.begin(), group.end(), neighbourPos) == group.end())
						{
							group.emplace_back(neighbourPos);
							stonesAdded.emplace_back(neighbourPos);
						}
					}
				}
			}

			stonesToCheck = Group(stonesAdded);
			stonesAdded.clear();
		}

		return group;
	}

	Group Goban::GetNeighbours(int x, int y) const
	{
		Group neighbours;

		if (x + 1 < x_)
		{
			neighbours.emplace_back(Coord(x + 1, y));
		}

		if (x - 1 >= 0)
		{
			neighbours.emplace_back(Coord(x - 1, y));
		}

		if (y - 1 >= 0)
		{
			neighbours.emplace_back(Coord(x, y - 1));
		}

		if (y + 1 < y_)
		{
			neighbours.emplace_back(Coord(x, y + 1));
		}

		return  neighbours;
	}

	Group Goban::GetNeighbours(const Coord pos) const
	{
		return GetNeighbours(pos.first, pos.second);
	}

	Stone Goban::StoneAt(const int x, const int y)
	{
		return stones_.at(x).at(y);
	}

	Stone Goban::StoneAt(const Coord pos)
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
				if (stone == empty)
					s += "+";
				else
				{
					s += std::to_string(stone);
				}
				s += " ";
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

	void Goban::KillGroup(const Group& group)
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

	void Goban::ReadBoardInfo(Board& board, int& bCaps, int& wCaps) const
	{
		board = Board(stones_);
		bCaps = bCaptures_;
		wCaps = wCaptures_;
	}

	std::pair<int, int> Goban::ScoreBoard() const
	{
		// TODO

		return std::pair<int, int>();
	}
}