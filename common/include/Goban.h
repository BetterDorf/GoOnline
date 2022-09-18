#pragma once
#include <vector>
#include <string>

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
		Goban(int x, int y, double komi);

		bool PlayStone(int x, int y, Stone team);

		int CountLiberties(int x, int y) const;
		int CountLiberties(const Group&) const;

		Group GetGroup(int x, int y) const;
		Group GetGroup(Coord) const;
		Group GetNeighbours(int x, int y) const;
		Group GetNeighbours(Coord) const;
		Group GetNeighbours(const Group& group) const;

		void KillGroup(Group&);

		Stone StoneAt(int x, int y) const;
		Stone StoneAt(Coord) const;

		void ReadBoardInfo(Board& board, int& bCaps, int& wCaps) const;

		std::string ToString();

		/// <summary>
		/// Score the board as is
		/// </summary>
		/// <returns>Pair of scores. First value is black, second is white</returns>
		std::pair<double, double> ScoreBoard() const;
	private:
		int x_;
		int y_;
		double komi_ = 7.5;
		int bCaptures_ = 0;
		int wCaptures_ = 0;
		Board stones_;

		Board previousBoardState_;
		Board prviousBoardWithSamePlayer_;

		/// <summary>
		/// Play a stone ignoring ko and without writing to variables like previous board state and wCaptures_
		/// But does write to stones_
		/// </summary>
		/// <param name="x">the x coordinate of the play</param>
		/// <param name="y">the y coordinate of the play</param>
		/// <param name="team">the colour of the stone</param>
		/// <param name="capturedCount">where to write the amount of stones that would be captured by that move</param>
		/// <returns>wether the move is allowed or not</returns>
		bool NaivePlayStone(int x, int y, Stone team, int& capturedCount);
	};
}