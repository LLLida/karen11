/**
 * This file is part of Karen11.
 *
 * Karen11 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Karen11 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Karen11.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "ConsolePlay.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <tuple>
#include <array>
#include <algorithm>

namespace karen11
{

using std::cout;
using std::cin;
using namespace std::literals;

#if defined(KAREN_ALWAYS_COLORED) || defined(__linux__) || defined(__linux) || defined(_WIN32)
bool ConsolePlay::colored = true;
#else
	bool ConsolePlay::colored = false;
#endif

#if defined(KAREN_ALWAYS_COLORED) || defined(__linux__) || defined(__linux) || defined(_WIN32)
bool ConsolePlay::clearScreen = true;
#else
bool ConsolePlay::clearScreen = false;
#endif

#if defined(KAREN_ALWAYS_COLORED) || defined(__linux__) || defined(__linux)
bool ConsolePlay::useUnicode = true;
#else
bool ConsolePlay::useUnicode = false;
#endif

/* \033[0m - resets terminal mode(std::ostream manipulator) */
static std::ostream& reset(std::ostream& out) noexcept
{
	if (ConsolePlay::colored)
		out << "\033[0m";
	return out;
}

/* foreground terminal color std::ostream manipulators */
namespace fg
{
	/* \033[31m - prints red colored text */
	static std::ostream& red(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[31m";
		return out;
	}
	/* \033[32m - prints green colored text */
	static std::ostream& green(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[32m";
		return out;
	}
	/* \033[33m - prints yellow colored text */
	static std::ostream& yellow(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[33m";
		return out;
	}
	/* \033[32m - prints blue colored text */
	static std::ostream& blue(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[34m";
		return out;
	}
	/* \033[32m - prints magenta colored text */
	static std::ostream& magenta(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[35m";
		return out;
	}
	/* \033[32m - prints cyan colored text */
	static std::ostream& cyan(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[36m";
		return out;
	}
}
/* background terminal color std::ostream manipulators */
namespace bg
{
	/* \033[40m - prints text with black background */
	static std::ostream& black(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
			out << "\033[40m";
		return out;
	}
	/* \033[47m - prints text with white background */
	static std::ostream& white(std::ostream& out) noexcept
	{
		if (ConsolePlay::colored)
				out << "\033[47m";
			return out;
	}
}

ConsolePlay::ConsolePlay()
	: Play(promptSide()) {}

ConsolePlay::~ConsolePlay() noexcept
{
	cout << reset << std::endl;
}

bool ConsolePlay::renderBoard(Color)
{
	clearTerminal();
	fillMessageBuffer();
	
	const Board& board = engine().getBoard();
	const bool w = playerSide == Color::WHITE;
	
	const auto perLine = [](bool w)
	{
		std::array<std::tuple<Square, std::string_view, std::string_view>, 8> result;
		if (w)
		{
			result[0] = {Square::A8, "8| "sv, "|8"sv};
			result[1] = {Square::A7, "7| "sv, "|7"sv};
			result[2] = {Square::A6, "6| "sv, "|6"sv};
			result[3] = {Square::A5, "5| "sv, "|5"sv};
			result[4] = {Square::A4, "4| "sv, "|4"sv};
			result[5] = {Square::A3, "3| "sv, "|3"sv};
			result[6] = {Square::A2, "2| "sv, "|2"sv};
			result[7] = {Square::A1, "1| "sv, "|1"sv};
		}
		else
		{
			result[0] = {Square::H1, "1| "sv, "|1"sv};
			result[1] = {Square::H2, "2| "sv, "|2"sv};
			result[2] = {Square::H3, "3| "sv, "|3"sv};
			result[3] = {Square::H4, "4| "sv, "|4"sv};
			result[4] = {Square::H5, "5| "sv, "|5"sv};
			result[5] = {Square::H6, "6| "sv, "|6"sv};
			result[6] = {Square::H7, "7| "sv, "|7"sv};
			result[7] = {Square::H8, "8| "sv, "|8"sv};
		}
		return result;
	}(w);
	if (useUnicode)
	{
		const std::string_view letters = (w ?
										  "   A B C D E F G H\n"sv :
										  "   H G F E D C B A\n"sv);
		cout << reset
			 << letters
			 << " +-----------------+\n";
		for (byte i = 0; i < 8; i++)
		{
			auto [square, beginLine, endLine] = perLine[i];
			
			cout << reset << beginLine;
			for (byte j = 0; j < 8; j++)
			{
				if ((i + j) % 2) cout << bg::white;
				else cout << bg::black;

				if (board[square] == Piece::EMPTY)
					cout << fg::magenta << "_";
				else if (isWhite(board[square]))
					cout << fg::blue << to_string_view(board[square], true);
				else
					cout << fg::red << to_string_view(board[square], true);

				cout << reset << ' ';
				if (w) ++square;
				else --square;
			}

			cout << reset << endLine;
			if (unsigned(i) * messageBufferWidth <= messageBuffer.size())
			{
				std::cout << "     "
						  << messageBuffer.substr(unsigned(i) * messageBufferWidth, messageBufferWidth);
			}
			cout << '\n';
		}
		cout << reset
			 << " +-----------------+\n"
			 << letters;
	}
	else
	{
		const std::string_view letters = (w ?
										  "   A  B  C  D  E  F  G  H\n"sv :
										  "   H  G  F  E  D  C  B  A\n"sv);
		
		cout << reset
			 << letters
			 << " +-------------------------+\n";
		for (byte i = 0; i < 8; i++)
		{
			auto [square, beginLine, endLine] = perLine[i];
			
			cout << reset << beginLine;
			for (byte j = 0; j < 8; j++)
			{
				if ((i + j) % 2) cout << bg::white;
				else cout << bg::black;

				if (board[square] == Piece::EMPTY)
					cout << fg::magenta << "__";
				else if (isWhite(board[square]))
					cout << fg::blue << to_string_view(board[square], false);
				else
					cout << fg::red << to_string_view(board[square], false);

				cout << reset << ' ';
				if (w) ++square;
				else --square;
			}

			cout << reset << endLine;
			if (unsigned(i) * messageBufferWidth <= messageBuffer.size())
			{
				std::cout << "     "
						  << messageBuffer.substr(unsigned(i) * messageBufferWidth, messageBufferWidth);
			}
			cout << '\n';
		}
		cout << reset
			 << " +-------------------------+\n"
			 << letters;
	}	
	return false;
}

bool ConsolePlay::inputMove(Move& move)
{
	moved = true;
	auto moves = engine().availableMoves(true);
	while (true)
	{
		std::string s = {};
		while (s.size() == 0)
		{
			cout << fg::cyan << "Input your move:" << fg::green << ' ';
			std::getline(cin, s);
		}

		std::transform(s.begin(), s.end(), s.begin(), ::tolower); /* lower case */
		std::remove(s.begin(), s.end(), ' '); /* remove spaces */
		std::remove(s.begin(), s.end(), '.'); /* remove dots */
		std::remove(s.begin(), s.end(), '-'); /* remove slashes */
		std::remove(s.begin(), s.end(), ':'); /* remove colons */
		if (s == "exit")
		{
			static const std::string_view messages[] = {
				"Dont' let me alone!\n"sv,
				"I won :D\n"sv,
				"Are you're afraid of me?\n"sv,
			};
			cout << fg::magenta << messages[random() % std::size(messages)];
			return true;
		}
		else if (s == "color")
		{
			colored = !colored;
			cout << fg::green << "Colored output is now <" << (colored ? "ON" : "OFF") << ">\n";
		}
		else if (s == "clearscreen")
		{
			clearScreen = !clearScreen;
			cout << fg::green << "Clearing screen is now <" << (clearScreen ? "ON" : "OFF") << ">\n";
		}
		else if (s == "unicode")
		{
			useUnicode = !useUnicode;
			cout << fg::green << "Unicode output is now <" << (useUnicode ? "ON" : "OFF") << ">\n";
		}
		else if (s == "help")
		{
			printHelp();
		}
		else if (s == "version")
		{
			printVersion();
		}
		else if (s == "history")
		{
		    printHistory(std::cout);
		}
		else if (s == "save")
		{
			static std::ofstream fout("karen-history.txt");
			if (fout)
			{
				using namespace std::chrono;
				bool temp = colored;
				colored = false;
				std::time_t now = system_clock::to_time_t(system_clock::now());
				fout << "Karen history for " << std::ctime(&now) << '\n';
				printHistory(fout);
				colored = temp;
				std::cout << fg::green << "Successfully wrote history to 'karen-history.txt'.\n";
			}
			else cout << fg::red << "Failed to open file for writing history :(\n";
		}
		else if (tryParse(s, move))
		{
			for (auto elem : moves)
				if (elem == move)
					return false;
			std::cout << fg::red << "Move isn't available. Type 'help' for help.\n";
		}
		else std::cout << fg::red << "Unrecognized command. Type 'help' for help.\n";
	}
	return false; /* avoid compiler warning */
}
	
void ConsolePlay::win()
{
	renderBoard(playerSide);
	cout << fg::green << R"(
  _   _    ___    _   _
 | \_/ |  / _ \  | | | |
 \     / / / \ \ | | | |
  \   /  | | | | | | | |
   | |   | | | | | |_| |
   | |   \ \_/ / |     |
   |_|    \___/  \_____|
 _  _  _    ___    _____
| || || |  / _ \  |     \
| || || | / / \ \ |  _  |
| || || | | | | | | | | |
| || || | | | | | | | | |
\       / \ \_/ / | | | |
 \_____/   \___/  |_| |_|
)";
}
	
void ConsolePlay::gameOver()
{
	renderBoard(playerSide);
	cout << fg::red << R"(
  ____    _____     _____     _____
 /  __/  /  _  \   /     \   /  _  \
/  /     | |_| |  /       \ /  | |  \
| /   _  |  _  |  | || || | |  _____|
| \  / \ | | | |  | || || | |  \____
\  \_/ | | | | |  | || || | \       \
 \_____/ |_| |_|  |_||_||_|  \______/
    ___    _   _    _____    _____ 
   / _ \  | | | |  /  _  \  /  _  \
  / / \ \ | | | | /  | |  \ | |_| |
  | | | | | | | | |  _____| |  _  /
  | | | | | \_/ | |  \____  | | \ \
  \ \_/ / \     / \       \ | | | |
   \___/   \___/   \______/ |_| |_|
)";
}

void ConsolePlay::draw()
{
	renderBoard(playerSide);
	cout << fg::yellow << R"(
 ____     _____   _____   _  _  _ 
|    \   /  _  \ /  _  \ | || || |
|  _  \  | |_| | | |_| | | || || |
| | \  | |  _  / |  _  | | || || |
| |_/  | | | \ \ | | | | | || || |
|      | | | | | | | | | \       /
|_____/  |_| |_| |_| |_|  \_____/ 
)";
}

void ConsolePlay::fillMessageBuffer()
{
	if (moved)
	{
		static std::string praises[] = {
			"Good move."s,
			"My grandma is even better."s,
			"Whoah!"s,
			"How about this move?"s,
			"You play awesome."s,
		};
		auto& info = engine().getState();
	
		auto move = history().back();
	
		messageBuffer =
			praises[random() % std::size(praises)] +
			" I moved "s + std::to_string(move) +
			", it took "s + std::to_string(info.time.count()) + "ms for me."s +
			" I transfered "s + std::to_string(info.positionsTransfered) +
			" and evaluated "s + std::to_string(info.positionsEvaluated) + " positions."s;
	}
	else
	{
		static std::string emoticons[] = {
			"Good luck!"s,
			"(/0_0)/"s,
			"Just blink."s,
			"..."s,
			"/\\/\\/\\ <----(0_-)"s,
		};
		messageBuffer = emoticons[random() % std::size(emoticons)];
	}
}
	
Color ConsolePlay::promptSide() noexcept
{
	static const std::string_view images[] = {
		R"(
    __        __
  /~ .~\    /~  ~\
 '      `\/'      *
(                .*)
|                .*|
 \            . *./
  `\ .      . .*/'
    `\ * .*. */'
      `\ * */'
        `\/'
)",
		R"(
  _   _
 |*\_/*|_______
 |_/-\_|______ \
| |           | |
| |  /\     | | |
| |-/  | /-/\-| |
| |    \/     | |
| |___________| |
 \_____________/
      |    |
      |   .|
      |  .*|
      | .**|
   ___|____|___
  / ********** \
/  ************  \
-------------------
)"
	};
	
	random.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	std::cout << fg::magenta << images[random() % std::size(images)];
	
	while(true)
	{
		std::cout << fg::yellow
				  << "Input your side[W/b]: " << fg::green;
		char c;
		std::cin >> c;
		if (c == 'w' || c == 'W')
			return Color::WHITE;
		if (c == 'b' || c == 'B')
			return Color::BLACK;

		std::cout << fg::red
				  << "Invalid input. Please, write 'w' for white or 'b' for black.\n";
	}
	return Color::WHITE;
}

void ConsolePlay::clearTerminal() noexcept
{
	if (clearScreen)
	{
#if defined(_WIN32)
		system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
		if (system("clear") < 0)
			cout << "Failed to clear screen!\n";
#elif defined (__APPLE__)
		system("clear");
#else
#       warning "Platform not supported. Clearing terminal won't work."
#endif
	}
}
bool ConsolePlay::tryParse(std::string& s, Move& move) noexcept
{
	using namespace std::string_view_literals;
	
	if (s == "oo"sv)
	{
		move = makeMove(Square::A1, Square::A1, MoveType::SHORT_CASTLING);
		return true;
	}
	if (s == "ooo"sv)
	{
		move = makeMove(Square::A1, Square::A1, MoveType::LONG_CASTLING);
		return true;
	}
	MoveType type = MoveType::NORMAL;
	if (s.find(';') != s.npos)
	{
		std::remove(s.begin(), s.end(), ';');
		type = MoveType::ENPASSANT;
	}
	byte x1 = s[0] - 'a';
	byte y1 = s[1] - '1';
	byte x2 = s[2] - 'a';
	byte y2 = s[3] - '1';
	if (x1 < 8 && y1 < 8 &&
		x2 < 8 && y2 < 8)
	{
		Square from = makeSquare(x1, y1), to = makeSquare(x2, y2);
		move = makeMove(from, to, type);
		return true;
	}
	return false;
}


void ConsolePlay::printHistory(std::ostream& stream)
{
	const auto& hist = history();
	const unsigned n = hist.size();
	stream << fg::magenta << "Move history(" << n / 2 << " moves):\n";
	for (unsigned i = 0; i < n; i += 2)
	{
		stream << fg::blue << "\t" << i / 2 + 1 << ". "
			   << fg::yellow << to_string(hist[i]) << ' ' << (to_string(hist[i + 1]))
				  << '\n';
	}
	stream << fg::magenta << "End.\n";
}


bool ConsolePlay::parseOptions(int argc, char** argv) noexcept
{
	for (int i = 1; i < argc; i++)
	{
		std::string option = argv[i];
		if (parseOption(option))
			return true;
	}
	return false;
}

bool ConsolePlay::parseOption(const std::string& s) noexcept
{
	if (s == "--version")
	{
		printVersion();
		return true;
	}
	if (s == "--help")
	{
		printHelp();
		return true;
	}
	if (s.find("--color") != s.npos)
	{
		if (s.find("OFF") != s.npos) colored = false;
		else colored = true;
		return false;
	}
	if (s.find("--unicode") != s.npos)
	{
		if (s.find("OFF") != s.npos) useUnicode = false;
		else useUnicode = true;
		return false;
	}
	if (s.find("--clearscreen") != s.npos)
	{
		if (s.find("OFF") != s.npos) clearScreen = false;
		else clearScreen = true;
		return false;
	}
	std::cout << fg::red << "Unrecognized option '" << s << "'.\n" << reset;
    return true;
}

void ConsolePlay::printVersion() noexcept
{
	std::cout << "Karen version is "
			  << Engine::version.major << '.' << Engine::version.minor
			  << ". Built in " << __DATE__ << ".\n"
			  << R"(
Copyright (C) 2021  Adil Mokhammad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
)";
}

void ConsolePlay::printHelp() noexcept
{
	std::cout << R"(
Karen is free open source lightweight chess engine.
License: GPLv3.

options:
    --version                Prints karen's version(program will be finished immediately).
    --help                   Prints this message(program will be finished immediately).
    --color={ON|OFF}         Enables colored output via ANSII escape sequences.
    --clearscreen={ON|OFF}   Enables clearing terminal after every move.
    --unicode={ON|OFF}       Enables unicode symbols output.

commands(type them when Karen asks you to input move):
    version                  Prints karen's version.
    help                     Prints this message.
    color                    Toggles colored output via ANSII escape sequences.
    clearscreen              Toggles clearing terminal after every move.
    unicode                  Toggles unicode symbols output.
    history                  Prints move history.
    save                     Writes move history to file 'karen-history.txt'.
    <move>                   Makes a move. If you want to do quiet move or capture simply type
                             source and destination squares, for example D2D4 or g8:f6.
                             If you want to do castling type OO or OOO(for long castling).
                             If you want to do en passant then type source square, ';' and
                             destination square, for example D5;C6.
                             Karen automatically checks whether move is available if you see
                             'Move isn't available' check if your move doesn't cause check for
                             your side.
    exit                     Exits the program.
)";
}

}

/* ConsolePlay.cpp ends here */

