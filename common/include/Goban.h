#pragma once
#include <unordered_set>
#include <vector>

#include "Stone.h"

namespace golc
{
	using Coord = std::pair<int, int>;
	using Group = std::vector<Coord>;
	using Board = std::vector<std::vector<Stone>>;

	class Goban
	{
	public:
		Goban(int x, int y);

		bool PlayStone(int x, int y, Stone team);

		int CountLiberties(int x, int y);
		int CountLiberties(const Group&);

		Group GetGroup(int x, int y);
		Group GetGroup(Coord);
		Group GetNeighbours(int x, int y) const;
		Group GetNeighbours(Coord) const;

		void KillGroup(const Group&);

		Stone StoneAt(int x, int y);
		Stone StoneAt(Coord);

		void const ReadBoardInfo(Board& board, int& bCaps, int& wCaps);

		std::string ToString();
	private:
		int x_;
		int y_;
		int bCaptures_ = 0;
		int wCaptures_ = 0;
		Board stones_;

		Board previousBoardState_;
		Board prviousBoardWithSamePlayer_;
	};
}