#include "Karen.hpp"
#include "ConsolePlay.hpp"
#include <iostream>

int main(int, char**)
{
	using namespace karen11;
#if 1
	ConsolePlay play;
	play.random.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	play();
#elif 0
	Engine karen(Board::standard(), Color::WHITE);
	
	// Move moves[] = {
	// 	makeMove(Square::D2, Square::D4),
	// 	makeMove(Square::E7, Square::E6),
	// 	makeMove(Square::H2, Square::H3),
	// 	makeMove(Square::G8, Square::F6),
	// 	makeMove(Square::H3, Square::H4),
	// 	makeMove(Square::F8, Square::B4),
	// 	makeMove(Square::B1, Square::D2),
	// 		makeMove(Square::H8, Square::E8),
	// };
	[[maybe_unused]] auto printBoard = [&]()
	{
		Square s = Square::A1;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
			{
				std::cout << to_string_view(karen.getBoard()[s]) << " \n"[j == 7];
				++s;
			}
	};
	[[maybe_unused]] auto printMoves = [&]()
	{
		for (auto move : karen.availableMoves())
			std::cout << to_string_view(getOrig(move)) << ' ' << to_string_view(getDest(move)) << '\n';			
	};
	[[maybe_unused]] auto printLists = [&]()
	{
		for (auto node = karen.getWhiteList(); node; node = node->pNext)
			std::cout << to_string_view(node->pos) << ':' << to_string_view(karen.getBoard()[node->pos]) << '\n';
		std::cout << std::endl;
		for (auto node = karen.getBlackList(); node; node = node->pNext)
			std::cout << to_string_view(node->pos) << ':' << to_string_view(karen.getBoard()[node->pos]) << '\n';
	};

	// for (auto move : moves)
	// {
	// 	std::cout << to_string_view(getOrig(move)) << ' ' << to_string_view(getDest(move)) << '\n';
	// 	karen.doMove(move);
	// 	printBoard();
	// 	std::cout << '\n';
	// }

// auto av = karen.availableMoves(false);
	// for (auto move : av)
	// {
	// 	std::cout << to_string_view(getOrig(move)) << ' ' << to_string_view(getDest(move)) << '\n';
	// }

	karen.doMove(makeMove(Square::E2, Square::E4));
	karen.doMove(makeMove(Square::D7, Square::D5));
	karen.doMove(makeMove(Square::D2, Square::D3));
	
	auto move = karen.think(7);
	std::cout << to_string_view(getOrig(move)) << ' ' << to_string_view(getDest(move)) << '\n';
	printBoard();
	// auto moves = karen.availableMoves();
	// auto inf = karen.doMove(moves[moves.size() - 1]);
	// printBoard();
	// karen.undoMove(inf);
	// printBoard();
	// printMoves();
	// printLists();
#else
	Board b;
	for (auto& p : b)
		p = Piece::EMPTY;
	b[Square::A7] = Piece::WHITE_KING;
	b[Square::B5] = Piece::BLACK_KING;
	b[Square::G2] = Piece::MOVED_BLACK_PAWN;
	Engine karen(b, Color::BLACK);

	[[maybe_unused]] auto printBoard = [&]()
	{
		Square s = Square::A1;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
			{
				std::cout << to_string_view(karen.getBoard()[s]) << " \n"[j == 7];
				++s;
			}
	};
	printBoard();

	karen.doMove(makeMove(Square::G2, Square::G1));
	printBoard();
	
#endif
	return 0;
}


