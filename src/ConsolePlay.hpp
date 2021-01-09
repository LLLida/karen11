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
#include "Karen.hpp"

#include <random>
#include <ostream>

namespace karen11
{

/**
 * Interacts with via text user interface.
 */
class ConsolePlay final : public Play
{
public:
	static bool colored;
	static bool clearScreen;
	static bool useUnicode;

	ConsolePlay();
	~ConsolePlay() noexcept;

	byte messageBufferWidth = 32;
	std::string messageBuffer;
	std::mt19937 random;

	/**
	 * Print help to stdout.
	 */
	static void printHelp() noexcept;
	/**
	 * Print version to stdout.
	 */
	static void printVersion() noexcept;
	static bool parseOptions(int argc, char** argv) noexcept;
	/**
	 * Print move history to `stream`.
	 */
	void printHistory(std::ostream& stream);

private:
	bool renderBoard(Color side) override;	
	bool inputMove(Move& move) override;
	void win() override;
	void gameOver() override;
	void draw() override;

	void fillMessageBuffer();
	static Color promptSide() noexcept;
	static void clearTerminal() noexcept;
	bool tryParse(std::string& s, Move& move) noexcept;
	[[nodiscard]]
	static bool parseOption(const std::string& s) noexcept;

	bool moved = false;
};

} /* namespace karen11 */

