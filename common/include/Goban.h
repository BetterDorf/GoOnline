#pragma once
#include <vector>
#include <string>
#include <map>

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

		[[nodiscard]] int CountLiberties(int x, int y) const;
		[[nodiscard]] int CountLiberties(const Group&) const;

		[[nodiscard]] Group GetGroup(int x, int y) const;
		[[nodiscard]] Group GetGroup(Coord) const;
		[[nodiscard]] Group GetNeighbours(int x, int y) const;
		[[nodiscard]] Group GetNeighbours(Coord) const;
		[[nodiscard]] Group GetNeighbours(const Group& group) const;

		void KillGroup(const Group&);
		/**
		 * \brief Kill a group and give point for captures
		 */
		void KillGroupScoring(const Group&);

		void KillStoneScoring(const Coord&);

		[[nodiscard]] Stone StoneAt(int x, int y) const;
		[[nodiscard]] Stone StoneAt(Coord) const;

		void ReadBoardInfo(Board& board, int& bCaps, int& wCaps) const;
		void ReadCaptures(int& bCaps, int& wCaps) const;

		std::string ToString();

		/// <summary>
		/// Score the board as is
		/// </summary>
		/// <returns>Pair of scores. First value is black, second is white</returns>
		[[nodiscard]] std::pair<double, double> ScoreBoard() const;

		void GenerateGroupIds();
		[[nodiscard]] std::map<int, Group> GroupsById() const { return groupsById_; }
	private:
		int x_;
		int y_;
		double komi_ = 7.5;
		int bCaptures_ = 0;
		int wCaptures_ = 0;
		Board stones_;

		Board previousBoardState_;
		Board prviousBoardWithSamePlayer_;

		std::map<int, Group> groupsById_ = {};

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

		[[nodiscard]] std::vector<Group> FindDeadGroups() const;
	};
}