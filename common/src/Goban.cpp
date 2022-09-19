#include  "Goban.h"

#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

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

		int captures = 0;

		// Play the move on the board without considering KO
		NaivePlayStone(x, y, team, captures);

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

		// Update capture scores
		if (captures != 0)
		{
			if (team == black)
			{
				bCaptures_ += captures;
			}
			else
			{
				wCaptures_ += captures;
			}
		}

		return true;
	}

	bool Goban::NaivePlayStone(const int x, const int y, const Stone team, int& capturedCount)
	{
		if (stones_.at(x).at(y) != empty)
			return  false;

		// Add the stone
		stones_.at(x).at(y) = team;

		// Check if other stones died as a result
		for (const auto neighbours = GetNeighbours(x, y); const auto& neighbour : neighbours)
		{
			if (const Stone stone = StoneAt(neighbour); stone != team && stone != empty)
			{
				if (auto group = GetGroup(neighbour); CountLiberties(group) == 0)
				{
					capturedCount = group.size();
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

	int Goban::CountLiberties(const int x, const int y) const
	{
		return CountLiberties(GetGroup(x, y));
	}

	int Goban::CountLiberties(const Group& group) const
	{
		int liberties = 0;

		for (const auto& neighbourCoord : GetNeighbours(group))
		{
			if (StoneAt(neighbourCoord) == empty)
			{
				liberties += 1;
			}
		}

		return liberties;
	}

	Group Goban::GetGroup(int x, int y) const
	{
		return GetGroup(Coord (x, y));
	}

	Group Goban::GetGroup(Coord pos) const
	{
		Group group;

		const int team = StoneAt(pos);

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
						// Add the stone if it's not already in our group
						if (std::ranges::find(group, neighbourPos) == group.end())
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

	Group Goban::GetNeighbours(const Group& group) const
	{
		Group groupNeighbours{};

		for (auto& stone : group)
		{
			auto neighbours = GetNeighbours(stone);
			for (auto& neighbour : neighbours)
			{
				// Add the coord if it's not part of our group and not yet in our neighbours
				if (std::ranges::find(groupNeighbours, neighbour) == groupNeighbours.end() &&
					std::ranges::find(group, neighbour) == group.end())
				{
					groupNeighbours.emplace_back(neighbour);
				}
			}
		}

		return groupNeighbours;
	}

	Group Goban::GetNeighbours(const Coord pos) const
	{
		return GetNeighbours(pos.first, pos.second);
	}

	Stone Goban::StoneAt(const int x, const int y) const
	{
		return stones_.at(x).at(y);
	}

	Stone Goban::StoneAt(const Coord pos) const
	{
		return StoneAt(pos.first, pos.second);
	}

	std::string Goban::ToString()
	{
		std::string s;

		for (auto& it : std::ranges::reverse_view(stones_))
		{
			for (const auto& stone : it)
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
			stones_.at(stone.first).at(stone.second) = empty;
		}
	}

	void Goban::ReadBoardInfo(Board& board, int& bCaps, int& wCaps) const
	{
		board = Board(stones_);
		bCaps = bCaptures_;
		wCaps = wCaptures_;
	}

	std::pair<double, double> Goban::ScoreBoard() const
	{
		std::vector<Coord> checkedCoords;
		std::pair<double, double> points(static_cast<double>(bCaptures_), static_cast<double>(wCaptures_) + komi_);

		// loop over every intersection
		for (int x = 0; x < x_ ; x++)
		{
			for (int y = 0 ; y < y_ ; y++)
			{
				Coord currentCoord(x,y);

				// Check if it's empty
				if (StoneAt(currentCoord) != empty)
				{
					continue;
				}

				// Check if we didn't process it already
				if (std::ranges::find(checkedCoords, currentCoord) != checkedCoords.end())
				{
					continue;
				}

				// obtain all empty intersections connected to it
				Group territory = GetGroup(currentCoord);

				// Add all territory coords in checked coords
				for (auto& coord : territory)
				{
					checkedCoords.emplace_back(coord);
				}

				// Check if there is one zero or two colours touching it
				Stone owner = empty;
				bool scorePoints = true;
				for (const auto& neighbourCoord : GetNeighbours(territory))
				{
					const Stone stone = StoneAt(neighbourCoord);

					// Check if the stone is different than the current owner
					if (stone != empty && stone != owner)
					{
						// Score no points if this territory already had an owner
						if (owner != empty)
						{
							scorePoints = false;
							continue;
						}
						else
						{
							owner = stone;
						}
					}
				}

				// Give points according to that
				if (scorePoints && owner != empty)
				{
					if (owner == black)
					{
						points.first += territory.size();
					}
					else
					{
						points.second += territory.size();
					}
				}
			}
		}

		return points;
	}

	std::vector<Group> Goban::FindDeadGroups() const
	{
		// Find all groups

		// Test if they can be killed

		return {};
	}
}