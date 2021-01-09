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
#pragma once

#include <cstdint>
#include <cstring>
#include <chrono>
#include <string_view>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <vector>

#ifndef NDEBUG
# define KAREN_DEBUG
#endif

#ifndef KAREN_ASSERT
	
# ifndef NDEBUG /* enable assertions when debug */
#  define KAREN_ENABLE_ASSERTIONS
# endif
	
#endif
	
#ifdef KAREN_ENABLE_ASSERTIONS
#define KAREN_ASSERT(cond, message)										\
	if (!(cond)) { karen11::detail::reportAssertion(message, __LINE__, __FUNCTION__); }
#else
#define KAREN_ASSERT(cond, message)
#endif

#define KAREN_OVERLOAD_ENUM_BIN_OPERATOR(op, type)						\
	[[nodiscard]] inline constexpr type									\
	operator op(type lhs, type rhs) noexcept							\
	{																	\
		return static_cast<type>(										\
			static_cast<std::underlying_type_t<type>>(lhs)	op			\
			static_cast<std::underlying_type_t<type>>(rhs));			\
	}
#define KAREN_OVERLOAD_ENUM_UN_OPERATOR(op, type)	\
	[[nodiscard]] inline constexpr type				\
	operator op(type value) noexcept									\
	{																	\
		return static_cast<type>(										\
			op static_cast<std::underlying_type_t<type>>(value));		\
	}

#define KAREN_OVERLOAD_ENUM_CR_OPERATOR(op, type)	\
	inline void	 operator op##op(type& value) noexcept				\
	{																\
		value = static_cast<type>(									\
			static_cast<std::underlying_type_t<type>>(value) op 1);	\
	}

#define KAREN_DEFINE_PIECE_CHECKER(colorName, pieceName)				\
	[[nodiscard]] inline constexpr bool									\
	is##colorName##pieceName(Piece p) noexcept							\
	{																	\
		return is##pieceName (p) && is##colorName (p);					\
	}

namespace karen11
{
#ifdef KAREN_ENABLE_ASSERTIONS
namespace detail
{
	inline void reportAssertion(const std::string& message, int line, const char* func)
	{
		throw std::runtime_error(std::string(func) + ": " + std::to_string(line) + ": " + message);
	}
}
#endif

#ifdef KAREN_DEBUG
inline constexpr bool is_debug = true;
#else
inline constexpr bool is_debug = false;
#endif

using namespace std::string_literals;
	
using byte = uint8_t;
using sbyte = int8_t;
/**
 * Converts `value` to byte.
 */
template<typename T>
[[nodiscard]]
inline constexpr byte toByte(T value) noexcept
{
	return static_cast<byte>(value);
}

/**
 * Represents piece's code
 */
enum class Code : byte
{
	PAWN = 1,
	KNIGHT = 2,
	BISHOP = 3,
	ROOK = 4,
	QUEEN = 5,
	KING = 6,
};

/* Represents piece's color */
enum class Color : byte
{
	WHITE = 0b1000'0000,
	BLACK = 0b0000'0000,
};

KAREN_OVERLOAD_ENUM_BIN_OPERATOR(^, Color)
	
/**
 * Invert `color`.
 */
inline constexpr Color operator! (Color color) noexcept
{
	return color ^ Color::WHITE;
}

/**
 * Bits:
 *  0..2: code
 *  6: moved
 *  7: color
 */
enum class Piece : byte
{
	EMPTY =           0,
	
	WHITE_PAWN =      toByte(Code::PAWN)       | toByte(Color::WHITE),
	WHITE_ROOK =      toByte(Code::ROOK)       | toByte(Color::WHITE),
	WHITE_KNIGHT =    toByte(Code::KNIGHT)     | toByte(Color::WHITE),
	WHITE_BISHOP =    toByte(Code::BISHOP)     | toByte(Color::WHITE),
	WHITE_QUEEN =     toByte(Code::QUEEN)      | toByte(Color::WHITE),
	WHITE_KING =      toByte(Code::KING)       | toByte(Color::WHITE),
	BLACK_PAWN =      toByte(Code::PAWN)       | toByte(Color::BLACK),
	BLACK_ROOK =      toByte(Code::ROOK)       | toByte(Color::BLACK),
	BLACK_KNIGHT =    toByte(Code::KNIGHT)     | toByte(Color::BLACK),
	BLACK_BISHOP =    toByte(Code::BISHOP)     | toByte(Color::BLACK),
	BLACK_QUEEN =     toByte(Code::QUEEN)      | toByte(Color::BLACK),
	BLACK_KING =      toByte(Code::KING)       | toByte(Color::BLACK),
	
	MOVED_WHITE_PAWN = toByte(WHITE_PAWN)      | 0b0100'0000, /* Make gdb recognize moved figures */
	MOVED_WHITE_ROOK = toByte(WHITE_ROOK)      | 0b0100'0000,
	MOVED_WHITE_KNIGHT = toByte(WHITE_KNIGHT)  | 0b0100'0000,
	MOVED_WHITE_BISHOP = toByte(WHITE_BISHOP)  | 0b0100'0000,
	MOVED_WHITE_QUEEN = toByte(WHITE_QUEEN)    | 0b0100'0000,
	MOVED_WHITE_KING = toByte(WHITE_KING)      | 0b0100'0000,
	MOVED_BLACK_PAWN = toByte(BLACK_PAWN)      | 0b0100'0000,
	MOVED_BLACK_ROOK = toByte(BLACK_ROOK)      | 0b0100'0000,
	MOVED_BLACK_KNIGHT = toByte(BLACK_KNIGHT)  | 0b0100'0000,
	MOVED_BLACK_BISHOP = toByte(BLACK_BISHOP)  | 0b0100'0000,
	MOVED_BLACK_QUEEN = toByte(BLACK_QUEEN)    | 0b0100'0000,
	MOVED_BLACK_KING = toByte(BLACK_KING)      | 0b0100'0000,
};

template<typename T>
inline constexpr T get(Piece) noexcept
{
	static_assert(sizeof(T) == 2 * sizeof(T) + 1,
				  "get(Piece) is not defined for this type.");
	return T{};
}

/**
 * Get piece's code.
 */
template<>
inline constexpr Code get<Code>(Piece piece) noexcept
{
	return static_cast<Code>(toByte(piece) & 7);
}
/**
 * Get piece's color.
 */
template<>
inline constexpr Color get<Color>(Piece piece) noexcept
{
	return static_cast<Color>(toByte(piece) & toByte(Color::WHITE));
}

/**
 * Check whether piece is white.
 */
inline constexpr bool isWhite(Piece piece)
{
	if (piece == Piece::EMPTY)
		return false;
	return get<Color>(piece) == Color::WHITE;
}
/**
 * Check whether piece is white.
 */
inline constexpr bool isBlack(Piece piece) noexcept
{
	if (piece == Piece::EMPTY)
		return false;
	return get<Color>(piece) == Color::BLACK;
}
/**
 * Check whether piece is pawn.
 */
[[nodiscard]]
inline constexpr bool isPawn(Piece piece) noexcept
{
	return get<Code>(piece) == Code::PAWN;
}
/**
 * Check whether piece is rook.
 */
[[nodiscard]]
inline constexpr bool isRook(Piece piece) noexcept
{
	return get<Code>(piece) == Code::ROOK;
}
/**
 * Check whether piece is knight.
 */
[[nodiscard]]
inline constexpr bool isKnight(Piece piece) noexcept
{
	return get<Code>(piece) == Code::KNIGHT;
}
/**
 * Check whether piece is bishop.
 */
[[nodiscard]]
inline constexpr bool isBishop(Piece piece) noexcept
{
	return get<Code>(piece) == Code::BISHOP;
}
/**
 * Check whether piece is queen.
 */
[[nodiscard]]
inline constexpr bool isQueen(Piece piece) noexcept
{
	return get<Code>(piece) == Code::QUEEN;
}
/**
 * Check whether piece is king.
 */
[[nodiscard]]
inline constexpr bool isKing(Piece piece) noexcept
{
	return get<Code>(piece) == Code::KING;
}
KAREN_DEFINE_PIECE_CHECKER(White, Pawn)
KAREN_DEFINE_PIECE_CHECKER(White, Rook)
KAREN_DEFINE_PIECE_CHECKER(White, Knight)
KAREN_DEFINE_PIECE_CHECKER(White, Bishop)
KAREN_DEFINE_PIECE_CHECKER(White, Queen)
KAREN_DEFINE_PIECE_CHECKER(White, King)
KAREN_DEFINE_PIECE_CHECKER(Black, Pawn)
KAREN_DEFINE_PIECE_CHECKER(Black, Rook)
KAREN_DEFINE_PIECE_CHECKER(Black, Knight)
KAREN_DEFINE_PIECE_CHECKER(Black, Bishop)
KAREN_DEFINE_PIECE_CHECKER(Black, Queen)
KAREN_DEFINE_PIECE_CHECKER(Black, King)
/**
 * Check whether piece is moved.
 */
[[nodiscard]]
inline constexpr bool isMoved(Piece piece)
{
	KAREN_ASSERT(piece != Piece::EMPTY,
				 "piece must not be EMPTY.");
	return toByte(piece) & 0b0100'0000;
}
inline void makeMoved(Piece& rPiece) noexcept
{
	KAREN_ASSERT(rPiece != Piece::EMPTY,
				 "piece must not be EMPTY.");
	rPiece = static_cast<Piece>(toByte(rPiece) | 0b0100'0000);
}

[[nodiscard]]
inline constexpr std::string_view to_string_view(Piece piece, bool unicode = false) noexcept
{
	using namespace std::string_view_literals;
		
	if (piece == Piece::EMPTY) return "  "sv;

	Color color = get<Color>(piece);
	Code code = get<Code>(piece);

	if (!unicode)
		switch(color)
		{
			case Color::WHITE:
				switch (code)
				{
					case Code::PAWN: return "WP"sv;
					case Code::ROOK: return "WR"sv;
					case Code::KNIGHT: return "WN"sv;
					case Code::BISHOP: return "WB"sv;
					case Code::QUEEN: return "WQ"sv;
					case Code::KING: return "WK"sv;
				}
				break;
				
			case Color::BLACK:
				switch (code)
				{
					case Code::PAWN: return "BP"sv;
					case Code::ROOK: return "BR"sv;
					case Code::KNIGHT: return "BN"sv;
					case Code::BISHOP: return "BB"sv;
					case Code::QUEEN: return "BQ"sv;
					case Code::KING: return "BK"sv;
				}
				break;
		}
	else
		switch(color)
		{
			case Color::WHITE:
				switch (code)
				{
					case Code::PAWN: return "\u2659"sv;
					case Code::ROOK: return "\u2658"sv;
					case Code::KNIGHT: return "\u2657"sv;
					case Code::BISHOP: return "\u2656"sv;
					case Code::QUEEN: return "\u2655"sv;
					case Code::KING: return "\u2654"sv;
				}
				break;
				
			case Color::BLACK:
				switch (code)
				{
					case Code::PAWN: return "\u265F"sv;
					case Code::ROOK: return "\u265E"sv;
					case Code::KNIGHT: return "\u265D"sv;
					case Code::BISHOP: return "\u265C"sv;
					case Code::QUEEN: return "\u265B"sv;
					case Code::KING: return "\u265A"sv;
				}
				break;
		}
	return "to_string_view(Piece): error"sv;
}

[[nodiscard]]
inline std::string to_string(Piece piece, bool unicode = false) noexcept
{
	return std::string(to_string_view(piece, unicode));
}

/**
 * Bits:
 *  0-2: x coord
 *  3-5: y coord
 */
enum class Square : byte
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
};

KAREN_OVERLOAD_ENUM_BIN_OPERATOR(+, Square)
KAREN_OVERLOAD_ENUM_BIN_OPERATOR(-, Square)
KAREN_OVERLOAD_ENUM_CR_OPERATOR(+, Square)
KAREN_OVERLOAD_ENUM_CR_OPERATOR(-, Square)

[[nodiscard]]
inline std::string_view to_string_view(Square square) noexcept
{
	using namespace std::string_view_literals;
	switch(square)
	{
		case Square::A1: return "A1"sv;
		case Square::B1: return "B1"sv;
		case Square::C1: return "C1"sv;
		case Square::D1: return "D1"sv;
		case Square::E1: return "E1"sv;
		case Square::F1: return "F1"sv;
		case Square::G1: return "G1"sv;
		case Square::H1: return "H1"sv;
		case Square::A2: return "A2"sv;
		case Square::B2: return "B2"sv;
		case Square::C2: return "C2"sv;
		case Square::D2: return "D2"sv;
		case Square::E2: return "E2"sv;
		case Square::F2: return "F2"sv;
		case Square::G2: return "G2"sv;
		case Square::H2: return "H2"sv;
		case Square::A3: return "A3"sv;
		case Square::B3: return "B3"sv;
		case Square::C3: return "C3"sv;
		case Square::D3: return "D3"sv;
		case Square::E3: return "E3"sv;
		case Square::F3: return "F3"sv;
		case Square::G3: return "G3"sv;
		case Square::H3: return "H3"sv;
		case Square::A4: return "A4"sv;
		case Square::B4: return "B4"sv;
		case Square::C4: return "C4"sv;
		case Square::D4: return "D4"sv;
		case Square::E4: return "E4"sv;
		case Square::F4: return "F4"sv;
		case Square::G4: return "G4"sv;
		case Square::H4: return "H4"sv;
		case Square::A5: return "A5"sv;
		case Square::B5: return "B5"sv;
		case Square::C5: return "C5"sv;
		case Square::D5: return "D5"sv;
		case Square::E5: return "E5"sv;
		case Square::F5: return "F5"sv;
		case Square::G5: return "G5"sv;
		case Square::H5: return "H5"sv;
		case Square::A6: return "A6"sv;
		case Square::B6: return "B6"sv;
		case Square::C6: return "C6"sv;
		case Square::D6: return "D6"sv;
		case Square::E6: return "E6"sv;
		case Square::F6: return "F6"sv;
		case Square::G6: return "G6"sv;
		case Square::H6: return "H6"sv;
		case Square::A7: return "A7"sv;
		case Square::B7: return "B7"sv;
		case Square::C7: return "C7"sv;
		case Square::D7: return "D7"sv;
		case Square::E7: return "E7"sv;
		case Square::F7: return "F7"sv;
		case Square::G7: return "G7"sv;
		case Square::H7: return "H7"sv;
		case Square::A8: return "A8"sv;
		case Square::B8: return "B8"sv;
		case Square::C8: return "C8"sv;
		case Square::D8: return "D8"sv;
		case Square::E8: return "E8"sv;
		case Square::F8: return "F8"sv;
		case Square::G8: return "G8"sv;
		case Square::H8: return "H8"sv;
	}
	return "to_string_view(Square): error"sv;
}

[[nodiscard]]
inline std::string to_string(Square square) noexcept
{
	return std::string(to_string_view(square));
}

[[nodiscard]]
inline constexpr Square makeSquare(byte x, byte y) noexcept
{
	return static_cast<Square>(x + (y << 3));
}

[[nodiscard]]
inline constexpr byte getX(Square square) noexcept
{
	return toByte(square) & 7;
}

[[nodiscard]]
inline constexpr byte getY(Square square) noexcept
{
	return (toByte(square) >> 3) & 7;
}

[[nodiscard]]
inline constexpr bool isValid(Square square) noexcept
{
	return toByte(square) < 64;
}

/**
 * Bits:
 *  0-5: origin
 *  6-11: destination
 *  12-13: promotion piece type
 *  14-15: special move
 */
enum class Move : uint16_t {};

enum class MoveType : uint16_t
{
	NORMAL = 0,
	ENPASSANT       = 0b0100'0000'0000'0000,
	SHORT_CASTLING  = 0b1000'0000'0000'0000,
	LONG_CASTLING   = 0b1100'0000'0000'0000,	
};

template<typename T>
T get(Move) noexcept;

template<>
[[nodiscard]]
inline MoveType get<MoveType>(Move move) noexcept
{
	return static_cast<MoveType>(
		static_cast<uint16_t>(move) & 0b1100'0000'0000'0000);
}

[[nodiscard]]
inline constexpr Square getOrig(Move move) noexcept
{
	return static_cast<Square>(static_cast<uint16_t>(move) & 63);
}

[[nodiscard]]
inline constexpr Square getDest(Move move) noexcept
{
	return static_cast<Square>((static_cast<uint16_t>(move) >> 6) & 63);
}

[[nodiscard]]
inline constexpr Move makeMove(Square orig, Square dest, MoveType type = MoveType::NORMAL) noexcept
{
	return static_cast<Move>(
		static_cast<uint16_t>(orig) |
		(static_cast<uint16_t>(dest) << 6) |
		static_cast<uint16_t>(type));
}

[[nodiscard]]
inline std::string to_string(Move move) noexcept
{
	switch (get<MoveType>(move))
	{
		case MoveType::NORMAL:
		case MoveType::ENPASSANT:
			return to_string(getOrig(move)) + to_string(getDest(move));
		case MoveType::SHORT_CASTLING:
			return "OO";
		case MoveType::LONG_CASTLING:
			return "OOO";
	}
	return "to_string(move): error";
}

using Score = int32_t;
inline constexpr Score ZERO = 0;
inline constexpr Score INF = 32000;
inline constexpr Score DRAW = ZERO;
inline constexpr Score MATE = -INF;
inline constexpr Score PAWN_SCORE = 100;
inline constexpr Score KNIGHT_SCORE = 375;
inline constexpr Score BISHOP_SCORE = 400;
inline constexpr Score ROOK_SCORE = 550;
inline constexpr Score QUEEN_SCORE = 1080;

class Board
{
public:
	Board() = default;
	~Board() = default;
	Board(const Board&) = default;
	Board(Board&&) = default;
	Board& operator=(const Board&) = default;

	Piece operator[] (Square square) const
	{
		KAREN_ASSERT(isValid(square),
					 "square out of range [0..63] = [A1..H8]");
		return data[toByte(square)];
	}
	Piece& operator[] (Square square)
	{
		KAREN_ASSERT(isValid(square),
					 "square out of range [0..63] = [A1..H8]");
		return data[toByte(square)];
	}

	Piece* begin() noexcept
	{
		return data;
	}

	Piece* end() noexcept
	{
		return data + 64;
	}

	static Board standard() noexcept
	{
		Board board;
		board[Square::A1] = Piece::WHITE_ROOK;
		board[Square::B1] = Piece::WHITE_KNIGHT;
		board[Square::C1] = Piece::WHITE_BISHOP;
		board[Square::D1] = Piece::WHITE_QUEEN;
		board[Square::E1] = Piece::WHITE_KING;
		board[Square::F1] = Piece::WHITE_BISHOP;
		board[Square::G1] = Piece::WHITE_KNIGHT;
		board[Square::H1] = Piece::WHITE_ROOK;
		board[Square::A2] = Piece::WHITE_PAWN;
		board[Square::B2] = Piece::WHITE_PAWN;
		board[Square::C2] = Piece::WHITE_PAWN;
		board[Square::D2] = Piece::WHITE_PAWN;
		board[Square::E2] = Piece::WHITE_PAWN;
		board[Square::F2] = Piece::WHITE_PAWN;
		board[Square::G2] = Piece::WHITE_PAWN;
		board[Square::H2] = Piece::WHITE_PAWN;
		board[Square::A3] = Piece::EMPTY;
		board[Square::B3] = Piece::EMPTY;
		board[Square::C3] = Piece::EMPTY;
		board[Square::D3] = Piece::EMPTY;
		board[Square::E3] = Piece::EMPTY;
		board[Square::F3] = Piece::EMPTY;
		board[Square::G3] = Piece::EMPTY;
		board[Square::H3] = Piece::EMPTY;
		board[Square::A4] = Piece::EMPTY;
		board[Square::B4] = Piece::EMPTY;
		board[Square::C4] = Piece::EMPTY;
		board[Square::D4] = Piece::EMPTY;
		board[Square::E4] = Piece::EMPTY;
		board[Square::F4] = Piece::EMPTY;
		board[Square::G4] = Piece::EMPTY;
		board[Square::H4] = Piece::EMPTY;
		board[Square::A5] = Piece::EMPTY;
		board[Square::B5] = Piece::EMPTY;
		board[Square::C5] = Piece::EMPTY;
		board[Square::D5] = Piece::EMPTY;
		board[Square::E5] = Piece::EMPTY;
		board[Square::F5] = Piece::EMPTY;
		board[Square::G5] = Piece::EMPTY;
		board[Square::H5] = Piece::EMPTY;
		board[Square::A6] = Piece::EMPTY;
		board[Square::B6] = Piece::EMPTY;
		board[Square::C6] = Piece::EMPTY;
		board[Square::D6] = Piece::EMPTY;
		board[Square::E6] = Piece::EMPTY;
		board[Square::F6] = Piece::EMPTY;
		board[Square::G6] = Piece::EMPTY;
		board[Square::H6] = Piece::EMPTY;
		board[Square::A7] = Piece::BLACK_PAWN;
		board[Square::B7] = Piece::BLACK_PAWN;
		board[Square::C7] = Piece::BLACK_PAWN;
		board[Square::D7] = Piece::BLACK_PAWN;
		board[Square::E7] = Piece::BLACK_PAWN;
		board[Square::F7] = Piece::BLACK_PAWN;
		board[Square::G7] = Piece::BLACK_PAWN;
		board[Square::H7] = Piece::BLACK_PAWN;
		board[Square::A8] = Piece::BLACK_ROOK;
		board[Square::B8] = Piece::BLACK_KNIGHT;
		board[Square::C8] = Piece::BLACK_BISHOP;
		board[Square::D8] = Piece::BLACK_QUEEN;
		board[Square::E8] = Piece::BLACK_KING;
		board[Square::F8] = Piece::BLACK_BISHOP;
		board[Square::G8] = Piece::BLACK_KNIGHT;
		board[Square::H8] = Piece::BLACK_ROOK;
		return board;
	}

private:
	Piece data[64];
};

struct Figure
{
	Square pos;
	Figure* pNext;
};
using FigureList = Figure*;

struct MoveEx
{
	int16_t score;
	Move move;
	
	bool operator< (MoveEx rhs) const noexcept
	{
		return score < rhs.score;
	}

	bool operator> (MoveEx rhs) const noexcept
	{
		return score > rhs.score;
	}
};

/**
 * Almost same to std::vector but uses stack memory.
 * Therefore it has constexpr capacity - `C`.
 */
template<typename T, unsigned C = 256>
class VectorOnStack
{
public:
	using size_type = unsigned int;
	using iterator = T*;
	using const_iterator = const T*;
	
	static constexpr size_type capacity = C;
	
private:
	byte bytes[capacity * sizeof(T)];
	T* data;
	size_type sz;

public:
	VectorOnStack() noexcept : data(reinterpret_cast<T*>(bytes)), sz(0) {}
	VectorOnStack(size_type size) noexcept : VectorOnStack()
	{
		if constexpr (!std::is_trivially_default_constructible_v<T>)
						 while (sz < size)
							 new(&data[sz]) T();
	}
	VectorOnStack(const VectorOnStack&) = default;
	VectorOnStack(VectorOnStack&&) = default;
	~VectorOnStack() noexcept
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
					 {
						 for (auto& elem : *this)
							 elem.~T();
					 }
	}

	iterator begin() noexcept { return data; }
	iterator end() noexcept { return data + sz; }
	const_iterator begin() const noexcept { return data; }
	const_iterator end() const noexcept { return data + sz; }
	size_type size() const noexcept { return sz; }

	T& operator[](size_type index) noexcept { return data[index]; }
	const T& operator[](size_type index) const noexcept { return data[index]; }

	void push_back(T&& rvalue)
	{
		KAREN_ASSERT(sz < capacity, "vector on stack : size is maximum");
		new(&data[sz++]) T(std::move(rvalue));
	}
	
	void push_back(const T& lvalue)
	{
		KAREN_ASSERT(sz < capacity, "vector on stack : size is maximum");
		new(&data[sz++]) T(lvalue);
	}

	void pop_back()
	{
		KAREN_ASSERT(sz > 0, "vector on stack : size is 0");
		if constexpr (~std::is_trivially_destructible_v<T>)
						 data[sz].~T();
		sz--;
	}

	iterator erase(iterator pos)
	{
		KAREN_ASSERT(pos >= begin() && pos < end(),
					 "VectorOnStack: passed iterator out of range [ begin(), end() )");
		/* Call mecmpy when possible */
		if constexpr (std::is_trivially_move_assignable_v<T>)
						 std::memcpy(pos, pos + 1, (sz - (pos - data) - 1) * sizeof(T));
		else
		{
			auto it = pos;
			while (it != end() - 1)
				*it = std::move(*++it);
		}
		sz--;
		end()->~T(); /* Calling last element's destructor */
		return pos;
	}

	iterator erase(size_type index)
	{
		return erase(begin() + index);
	}
};

struct DummyType {};

/**
 * The main chess engine class.
 */
class Engine
{
public:
	struct ThinkInfo
	{
		std::chrono::milliseconds time;
		unsigned positionsEvaluated = 0;
		unsigned positionsTransfered = 0;
	};

	struct MoveInfo
	{
		byte enPassantAvailable = 8;
		Move move;
		Figure* moved = nullptr;
		Piece movedPiece;
		Figure* erased = nullptr;
		Piece erasedPiece;
	};

	static constexpr bool enable_think_info = true;

	enum class GameState { PLAY, DRAW, MATE };
	
	struct State :
		std::conditional_t<enable_think_info, ThinkInfo, DummyType>,
		MoveInfo
	{
		Color side;
		GameState game;
		bool isCheck;
	};

	static constexpr struct
	{
		/* Indicates major version */
		int major;
		/* Indicates minor version */
		int minor;
	} version = {1, 1};

	static constexpr unsigned max_available_moves = 256;
	static constexpr unsigned max_ply = 16;
		
private:
	Board board;
	State state;
	/* List with all white figures */
	FigureList whiteList;
	/* List with all black figures */
	FigureList blackList;
	/* Buffer for avoiding allocating memory on heap */
	Figure figuresBuffer[64];

	Engine(const Engine&) = default;
	
public:
	/**
	 * Create karen chess engine.
	 */
	Engine(const Board& board, Color side)
		: board(board)
	{
		state.side = side;
		// state.game = State::PLAY;
		state.isCheck = false;

		fillLists();
	}

	/**
	 * It isn't good to call this function everytime you move because it
	 * ineffective: it copies whole `board` and reevaluates
	 * figure lists.
	 * This function is usable when user wants to make
	 * move forbidden in chess.
	 */
	void setBoard(const Board& board) noexcept
	{
		this->board = board;

		// state.game = State::PLAY;
		state.isCheck = false;
		
		fillLists();
	}
	
	auto doMove(Move move)
	{
		MoveInfo info;

		const auto type = get<MoveType>(move);
		const auto from = getOrig(move);
		const auto to = getDest(move);
		info.move = move;
		info.enPassantAvailable = state.enPassantAvailable;
		info.erased = nullptr;

		[[maybe_unused]]
		const byte x1 = getX(from), y1 = getY(from),
			x2 = getX(to), y2 = getY(to);

		switch(type)
		{
			case MoveType::NORMAL:
			{
				KAREN_ASSERT(board[from] != Piece::EMPTY,
							 "Moving piece must not be EMPTY.");
				KAREN_ASSERT(to != from,
							 "Piece must move from it's position(which is " + to_string(to) + ").");
				KAREN_ASSERT((board[to] == Piece::EMPTY) || (get<Color>(board[from]) != get<Color>(board[to])),
							 "Piece(which is " + to_string(board[from]) +
							 ") must not fell down piece(which is " + to_string(board[to]) +
							 ") with the same color.");
				auto moving = find(from, state.side);
				KAREN_ASSERT(moving,
							 "Moving piece is EMPTY or doesn't match current side.");
				auto erasing = erase(to, !state.side);

				if (isWhitePawn(board[from]) && y1 == 1 && y2 == 3)
					state.enPassantAvailable = x1;
				else if (isBlackPawn(board[from]) && y1 == 6 && y2 == 4)
					state.enPassantAvailable = x1;
				else state.enPassantAvailable = 8;

				info.erased = erasing;
				info.moved = moving;
				info.erasedPiece = board[to];
				info.movedPiece = board[from];
				
				moving->pos = to;
				if (isWhitePawn(board[from]) && getY(to) == 7) /* white promotion */
					board[to] = Piece::MOVED_WHITE_QUEEN;
				else if (isBlackPawn(board[from]) && getY(to) == 0) /* black promotion */
					board[to] = Piece::MOVED_BLACK_QUEEN;
				else /* any other move */
				{
					board[to] = board[from];
					makeMoved(board[to]);
				}
				board[from] = Piece::EMPTY;
			}
			break;
			case MoveType::ENPASSANT:
			{
				KAREN_ASSERT(state.enPassantAvailable == x2,
							 "Last move wasn't a pawn move of 2 squares.\n"
							 "En passant is impossible.");
				KAREN_ASSERT(board[to] == Piece::EMPTY,
							 "in en passant pawn must move to an EMPTY square.");
				if (state.side == Color::WHITE)
				{
					KAREN_ASSERT((y1 == 4) && (y2 == 5) && (x1 - x2 == 1 || x2 - x1 == 1),
								 "invalid move coordinates.");
					KAREN_ASSERT(isWhitePawn(board[from]), "invalid piece.");
					KAREN_ASSERT(isBlackPawn(board[makeSquare(x2, y1)]),
								 "felled piece isn't black pawn");
				}
				else
				{
					KAREN_ASSERT((y1 == 3) && (y2 == 2) && (x1 - x2 == 1 || x2 - x1 == 1),
								 "invalid move coordinates.");
					KAREN_ASSERT(isBlackPawn(board[from]), "invalid piece.");
					KAREN_ASSERT(isWhitePawn(board[makeSquare(x2, y1)]),
								 "felled piece isn't white pawn");
				}

				const Square felledPos = makeSquare(x2, y1);

				info.erased = erase(felledPos, !state.side);
				info.moved = find(from, state.side);
				info.moved->pos = to;
				info.movedPiece = board[from];
				info.erasedPiece = board[felledPos];

				board[to] = board[from];
				board[from] = Piece::EMPTY;
				board[felledPos] = Piece::EMPTY;
				makeMoved(board[to]);

				state.enPassantAvailable = 8;
			}
			break;
			case MoveType::SHORT_CASTLING:
			{
				const Square positions[] = {
					(state.side == Color::WHITE) ? Square::E1 : Square::E8, /* king */
					(state.side == Color::WHITE) ? Square::F1 : Square::F8,
					(state.side == Color::WHITE) ? Square::G1 : Square::G8,
					(state.side == Color::WHITE) ? Square::H1 : Square::H8, /* rook */
				};

				Piece& king = board[positions[0]];
				Piece& midL = board[positions[1]];
				Piece& midR = board[positions[2]];
				Piece& rook = board[positions[3]];

				KAREN_ASSERT(isKing(king), "King must be at E1 or E8");
				KAREN_ASSERT(isRook(rook), "Rook must be at F1 or F8");
				KAREN_ASSERT(!isMoved(king),
							 "doing castling after king moved is not allowed");
				KAREN_ASSERT(!isMoved(rook),
							 "doing castling after rook moved is not allowed");

				makeMoved(king);
				makeMoved(rook);

				find(positions[0], state.side)->pos = positions[2];
				find(positions[3], state.side)->pos = positions[1];

				midL = rook;
				midR = king;
				king = Piece::EMPTY;
				rook = Piece::EMPTY;

				state.enPassantAvailable = 8;
			}
			break;
			case MoveType::LONG_CASTLING:
			{
				const Square positions[] = {
					(state.side == Color::WHITE) ? Square::A1 : Square::A8, /* rook pos */
					(state.side == Color::WHITE) ? Square::B1 : Square::B8,
					(state.side == Color::WHITE) ? Square::C1 : Square::C8,
					(state.side == Color::WHITE) ? Square::D1 : Square::D8,
					(state.side == Color::WHITE) ? Square::E1 : Square::E8 /* king pos */
				};

				Piece& rook = board[positions[0]];
				[[maybe_unused]] Piece& midL = board[positions[1]];
				Piece& midM = board[positions[2]];
				Piece& midR = board[positions[3]];
				Piece& king = board[positions[4]];

				KAREN_ASSERT(isKing(king), "in castling king must be at E1 or E8");
				KAREN_ASSERT(isRook(rook), "rook must be at A1 or A8");
				KAREN_ASSERT(!isMoved(king), "doing castling after king moved is not allowed");
				KAREN_ASSERT(!isMoved(rook), "doing castling after rook moved is not allowed");
				KAREN_ASSERT(midL == Piece::EMPTY && midM == Piece::EMPTY && midR == Piece::EMPTY,
							 "space between king and rook must be EMPTY");

				makeMoved(king);
				makeMoved(rook);

				find(positions[0], state.side)->pos = positions[3];
				find(positions[4], state.side)->pos = positions[2];

				midM = king;
				midR = rook;
				king = Piece::EMPTY;
				rook = Piece::EMPTY;

				state.enPassantAvailable = 8;
			}
			break;
			default:
				KAREN_ASSERT(false, "Invalid move type");
				break;
		}
		state.side = !state.side;
		
		return info;
	}
	
	void undoMove(const MoveInfo& info) noexcept
	{
		if (info.erased)
		{
			insert(info.erased, state.side);
		}

		state.side = !state.side;

		auto moveType = get<MoveType>(info.move);
		Square from = getOrig(info.move);
		Square to = getDest(info.move);

		switch (moveType)
		{
			case MoveType::NORMAL:
				info.moved->pos = from;
				board[from] = info.movedPiece;
				board[to] = info.erasedPiece;
				break;
				
			case MoveType::ENPASSANT:
				info.moved->pos = from;
				board[from] = info.movedPiece;
				board[to] = Piece::EMPTY;
				board[makeSquare(getX(to), getY(from))] = info.erasedPiece;
				break;
			case MoveType::SHORT_CASTLING:
				if (state.side == Color::WHITE)
				{
					find(Square::G1, state.side)->pos = Square::E1;
					find(Square::F1, state.side)->pos = Square::H1;

					board[Square::E1] = Piece::WHITE_KING;
					board[Square::F1] = Piece::EMPTY;
					board[Square::G1] = Piece::EMPTY;
					board[Square::H1] = Piece::WHITE_ROOK;
				}
				else
				{
					find(Square::G8, state.side)->pos = Square::E8;
					find(Square::F8, state.side)->pos = Square::H8;

					board[Square::E8] = Piece::BLACK_KING;
					board[Square::F8] = Piece::EMPTY;
					board[Square::G8] = Piece::EMPTY;
					board[Square::H8] = Piece::BLACK_ROOK;
				}
				break;
			case MoveType::LONG_CASTLING:
				if (state.side == Color::WHITE)
				{
					find(Square::D1, state.side)->pos = Square::A1;
					find(Square::C1, state.side)->pos = Square::E1;

					board[Square::A1] = Piece::WHITE_ROOK;
					board[Square::C1] = Piece::EMPTY;
					board[Square::D1] = Piece::EMPTY;
					board[Square::E1] = Piece::WHITE_KING;
				}
				else
				{
					find(Square::D8, state.side)->pos = Square::A8;
					find(Square::C8, state.side)->pos = Square::E8;

					board[Square::A8] = Piece::BLACK_ROOK;
					board[Square::C8] = Piece::EMPTY;
					board[Square::D8] = Piece::EMPTY;
					board[Square::E8] = Piece::BLACK_KING;
				}
				break;
		}
		state.enPassantAvailable = info.enPassantAvailable;
	}

	/**
	 * Get board that engine is playing on.
	 */
	[[nodiscard]]
	auto& getBoard() const noexcept { return board; }
	/**
	 * Get engine's state.
	 */
	[[nodiscard]]
	auto& getState() const noexcept { return state; }
	/**
	 * Get list with all white figures.
	 */
	[[nodiscard]]
	auto& getWhiteList() const noexcept { return whiteList; }
	/**
	 * Get list with all black figures.
	 */
	[[nodiscard]]
	auto& getBlackList() const noexcept { return blackList; }
	/**
	 * Get list depending on `color`.
	 */
	[[nodiscard]]
	auto& getList(Color side) const noexcept { return (side == Color::WHITE) ? whiteList : blackList; }

private:
	struct SquareEx { byte x, y; };
	
	static constexpr SquareEx knightMoves[8] = {
		{byte(1),  byte(2)},
		{byte(2),  byte(1)},
		{byte(-1), byte(2)},
		{byte(2),  byte(-1)},
		{byte(-2), byte(1)},
		{byte(1),  byte(-2)},
		{byte(-1), byte(-2)},
		{byte(-2), byte(-1)},
	};

	static constexpr SquareEx bishopMoves[4] = {
		{byte(1),  byte(1)},
		{byte(-1), byte(1)},
		{byte(1),  byte(-1)},
		{byte(-1), byte(-1)},
	};

	static constexpr SquareEx rookMoves[4] = {
		{byte(1),  byte(0)},
		{byte(-1), byte(0)},
		{byte(0),  byte(1)},
		{byte(0),  byte(-1)},
	};

	static constexpr SquareEx queenNKingMoves[8] = {
		bishopMoves[0],
		bishopMoves[1],
		bishopMoves[2],
		bishopMoves[3],
		rookMoves[0],
		rookMoves[1],
		rookMoves[2],
		rookMoves[3],
	};
	
	/**
	 * Get non constant list depending on `color`.
	 */
	[[nodiscard]]
	auto& getList(Color side) noexcept { return (side == Color::WHITE) ? whiteList : blackList; }

	/**
	 * Find node in figure list that has pos == `pos`.
	 */
	Figure* find(Square pos, Color hint) noexcept
	{
		for (auto node = getList(hint); node; node = node->pNext)
			if (node->pos == pos)
				return node;
		return nullptr;
	}
	
	/**
	 * Erases figure with pos = `pos` from list `hint`.
	 */
	Figure* erase(Square pos, Color hint)
	{
		/* Here is simple scheme of how this works:
		 * Let's say we want to erase 2nd element:
		 *  /---\   /---\   /---\
		 * >| 1 |-->| 2 |-->| 3 |-
		 *  \---/   \---/   \---/
		 * Firstly we find it. Then we make the element
		 * that is before the erasing element(in this case 1st)
		 * point to element that the erasing element points to
		 * (in this case 3rd element).
		 *  /---\           /---\
		 * >| 1 |---------->| 3 |-
		 *  \---/           \---/		 
		 *          /---\     ^
		 *          | 2 |-----/
		 *          \---/
		 * Now we're done!
		 * What about erasing 1st element? Ah, it's impossible:
		 * in chess king can't be felled!
		 */
		for (Figure* node = getList(hint), *prev = nullptr; node; node = node->pNext)
		{
			if (node->pos == pos)
			{
				KAREN_ASSERT(prev, "Tried to erase king from list");
				prev->pNext = node->pNext;
				return node;
			}
			prev = node;
		}
		return nullptr;
	}

	/**
	 * Returns back to list figure that was erased from it via `erase`.
	 * If `node` isn't result of `erase` then assertion will be failed.
	 */
	void insert(Figure* node, Color hint)
	{
		Figure* prev = getList(hint);
		while (prev->pNext != node->pNext)
		{
			KAREN_ASSERT(prev,
						 "Tried to insert figure that wasn't erased from the list");
			prev = prev->pNext;
		}
		prev->pNext = node;
	}
	
    /**
	 * Fills white and black lists with pieces from board.
	 * This function is ineffective; it must be called only
	 * when engine is initializing or when user wants to
	 * change board.
	 */
	void fillLists() noexcept
	{
		FigureList whiteLists[6] = {0}; /* [0] -> pawns, [1] -> knights, [2] -> bishops, */
		FigureList blackLists[6] = {0};	/* [3] -> rooks, [4] -> queens, [5] -> kings */
		byte figuresCount = 0;

		/* Fill lists with figures from board */
		for (Square n = Square::A1; isValid(n); ++n)
		{
			auto piece = board[n];
			if (piece != Piece::EMPTY)
			{
				figuresBuffer[figuresCount] = {n, nullptr};
				auto p = &figuresBuffer[figuresCount];
				byte code = toByte(get<Code>(piece)) - 1;
				if (isWhite(piece)) /* Add figure to white list */
				{
					p->pNext = whiteLists[code];
					whiteLists[code] = p;
				}
				else /* Add figure to black list */
				{
					p->pNext = blackLists[code];
					blackLists[code] = p;
				}
				figuresCount++;
			}
		}
		/* Combine lists */
		for (sbyte i = 4; i >= 0; i--)
		{
			if (!whiteLists[i])
				for (sbyte j = i - 1; j >= 0; j--)
					if (whiteLists[j])
					{
						whiteLists[i] = whiteLists[j];
						whiteLists[j] = nullptr;
						break;
					}
			if (!blackLists[i])
				for (sbyte j = i - 1; j >= 0; j--)
					if (blackLists[j])
					{
						blackLists[i] = blackLists[j];
						blackLists[j] = nullptr;
						break;;
					}
		}
		for (byte i = 1; i < 6; i++)
		{
			FigureList currList = whiteLists[i];
			if (currList)
			{
				while (currList->pNext) currList = currList->pNext;
				currList->pNext = whiteLists[i - 1];
			}

			currList = blackLists[i];
			if (currList)
			{
				while (currList->pNext) currList = currList->pNext;
				currList->pNext = blackLists[i - 1];
			}
		}
		whiteList = whiteLists[5];
		blackList = blackLists[5];
	}

	/**
	 * Check if square `pos` can be atacked by side - `!side`.
	 */
	[[nodiscard]]
	bool isAtacked(Square pos, Color side) const
	{
		/* The idea of the algorithm of this function is
		 * following: check only necessary squares.
		 * This scheme shows how it works:
		 * _ _ _ R _ _ _ _
		 * _ _ _ R _ _ _ B
		 * B _ _ R _ _ B _
		 * _ B N R N B _ _
		 * _ N P K P N _ _
		 * R R K * K R R R
		 * _ N K K K N _ _
		 * _ B N R N B _ _
		 * Where * - is `pos`, K - squares from that king
		 * can fell, P - squares from that pawns can fell,
		 * B - squares from that bishop can fell,
		 * R - squares from that rooks can fell,
		 * N - squares from that knights can fell.
		 */
		const byte x = getX(pos);
		const byte y = getY(pos);
		byte newX, newY;

		for (auto delta : knightMoves) /* Check if there're knights nearby */
		{
			newX = delta.x + x;
			newY = delta.y + y;
			if (newX < 8 &&
				newY < 8) /* Check if {newX, newY} is not out of board */
			{
				auto piece = board[makeSquare(newX, newY)];
				if (get<Code>(piece) == Code::KNIGHT &&
					get<Color>(piece) != side)
					return true;
			}
		}
		for (auto delta : bishopMoves) /* Check if there're bishops nearby */
		{
			newX = x;
			newY = y;
			for (byte i = 1; i < 8; i++) /* Throw ray diagonally */
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 &&
					newY < 8) /* Check if {newX, newY} is not out of board */
				{
					auto piece = board[makeSquare(newX, newY)];
					if (piece != Piece::EMPTY)
					{
						/* piece is not empty, so we need to exit the cycle anyway */
						auto code = get<Code>(piece);
						if (get<Color>(piece) != side &&
							(code == Code::BISHOP ||
							 code == Code::QUEEN)) /* queen can atack exact same as bishop */
							return true;
						break;
					}
				}
				else break;
			}
		}
		for (auto delta : rookMoves) /* Check if there're rooks nearby */
		{
			newX = x;
			newY = y;
			for (byte i = 1; i < 8; i++) /* Throw ray horizontally/vertically */
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 &&
					newY < 8) /* Check if {newX, newY} is not out of board */
				{
					auto piece = board[makeSquare(newX, newY)];
					if (piece != Piece::EMPTY)
					{
						auto code = get<Code>(piece);
						if (get<Color>(piece) != side &&
							(code == Code::ROOK ||
							 code == Code::QUEEN)) /* queen can atack exact same as rook */
							return true;
						break;
					}
				}
				else break;
			}
		}
		if (side == Color::WHITE) /* Check if there're black pawns nearby */
		{
			newY = y + 1;
			if (newY < 8)
			{
				newX = x + 1;
				if (newX < 8 && isBlackPawn(board[makeSquare(newX, newY)]))
					return true;
				newX = x - 1;
				if (newX < 8 && isBlackPawn(board[makeSquare(newX, newY)]))
					return true;
			}
		}
		else /* Check if there're white pawns nearby */
		{
			newY = y - 1;
			if (newY < 8)
			{
				newX = x + 1;
				if (newX < 8 && isWhitePawn(board[makeSquare(newX, newY)]))
					return true;
				newX = x - 1;
				if (newX < 8 && isWhitePawn(board[makeSquare(newX, newY)]))
					return true;
			}
		}
		for (auto delta : queenNKingMoves)
		{
			newX = delta.x + x;
			newY = delta.y + y;
			if (newX < 8 &&
				newY < 8)
			{
				auto piece = board[makeSquare(newX, newY)];
				if (get<Code>(piece) == Code::KING &&
					get<Color>(piece) != side)
					return true;
			}
		}
		return false;
	}
	
	/**
	 * Writes all available captures of current side to `moves`.
	 * `moves` must be preallocated array with at least 256 elements.
	 */
	template<unsigned C>
	void genCaptures(VectorOnStack<MoveEx, C>& moves) const
	{
		byte enemyKingX = getX(getList(!state.side)->pos);
		byte enemyKingY = getY(getList(!state.side)->pos);
		
		auto add = [&](Square from, Square to, int16_t extra = 0) noexcept {
			/* See MVV/LVA priciple: https://www.chessprogramming.org/MVV-LVA */
			int16_t score = int16_t(get<Code>(board[to])) - int16_t(get<Code>(board[to])) + 6 + extra;
			moves.push_back(MoveEx{ score, makeMove(from, to) });
		};
		auto addEnPassant = [&](Square from, Square to) noexcept {
			moves.push_back(MoveEx{7, makeMove(from, to, MoveType::ENPASSANT)});
		};
		auto processLongRange = [&](Square pos, SquareEx delta) noexcept {
			byte newX = getX(pos);
			byte newY = getY(pos);
			for (byte i = 1; i < 8; i++)
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					Square newPos = makeSquare(newX, newY);
					if (board[newPos] != Piece::EMPTY)
					{
						if (get<Color>(board[newPos]) != state.side)
						{
							int16_t extraScore = 0; /* Extra score for approaching the enemy king */
							if (std::max(enemyKingX, newX) - std::min(enemyKingX, newX) +
								std::max(enemyKingY, newY) - std::min(enemyKingY, newY) <= 5)
								extraScore += 3;
							add(pos, newPos, extraScore);
						}
						break;
					}
				}
				else break;
			}
		};

		for (auto node = getList(state.side); node; node = node->pNext)
		{
			const Square pos = node->pos;
			const Piece piece = board[pos];
			
			const byte x = getX(pos);
			const byte y = getY(pos);
			const Code code = get<Code>(piece);

			switch(code)
			{
				case Code::PAWN:
					if (x != 0)
					{
						Square newPos = makeSquare(x - 1, (state.side == Color::WHITE) ? (y + 1) : (y - 1));
						if (board[newPos] != Piece::EMPTY && get<Color>(board[newPos]) != state.side)
							add(pos, newPos);
						else if (state.enPassantAvailable == x - 1)
						{
							if ((state.side == Color::WHITE && y == 4) ||
								(state.side == Color::BLACK && y == 3))
								addEnPassant(pos, newPos);
						}
					}
					if (x < 7)
					{
						Square newPos = makeSquare(x + 1, (state.side == Color::WHITE) ? (y + 1) : (y - 1));
						if (board[newPos] != Piece::EMPTY && get<Color>(board[newPos]) != state.side)
							add(pos, newPos);
						else if (state.enPassantAvailable == x + 1)
						{
							if ((state.side == Color::WHITE && y == 4) ||
								(state.side == Color::BLACK && y == 3))
								addEnPassant(pos, newPos);
						}
					}
					
					break;
				case Code::KNIGHT:
					for (auto delta : knightMoves)
					{
						byte newX = x + delta.x;
						byte newY = y + delta.y;
						if (newX < 8 && newY < 8)
						{
							Square newPos = makeSquare(newX, newY);
							if (board[newPos] != Piece::EMPTY &&
								get<Color>(board[newPos]) != state.side)
							{
								int16_t extraScore = 0;
								if (newX >= 2 && newX <= 5 &&
									newY >= 2 && newY <= 5)
									extraScore++;
								add(pos, newPos, extraScore);
							}
						}
					}
					
					break;
				case Code::BISHOP:
					for (auto delta : bishopMoves)
						processLongRange(pos, delta);
					break;
				case Code::ROOK:
					for (auto delta : rookMoves)
						processLongRange(pos, delta);
					break;
				case Code::QUEEN:
					for (auto delta : queenNKingMoves)
						processLongRange(pos, delta);
					break;
				case Code::KING:
					for (auto delta : queenNKingMoves)
					{
						byte newX = x + delta.x;
						byte newY = y + delta.y;
						if (newX < 8 && newY < 8)
						{
							Square newPos = makeSquare(newX, newY);
							if (board[newPos] != Piece::EMPTY && get<Color>(board[newPos]) != state.side)
								add(pos, newPos, -1);
						}
					}
					break;
			}
		}
	}
	
	template<unsigned C>
	void genMoves(VectorOnStack<MoveEx, C>& moves) const
	{
		byte enemyKingX = getX(getList(!state.side)->pos);
		byte enemyKingY = getY(getList(!state.side)->pos);
		
		auto add = [&](Move mov, int16_t score = 0) noexcept {
			moves.push_back({score, mov});
		};
		auto processLongRange = [&](Square pos, SquareEx delta) noexcept {
			byte newX = getX(pos);
			byte newY = getY(pos);
			for (byte i = 1; i < 8; i++)
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 && newY < 8)
				{
					Square newPos = makeSquare(newX, newY);
					if (board[newPos] == Piece::EMPTY)
					{
						int16_t extraScore = 0; /* Extra score for approaching the enemy king */
						if (std::max(enemyKingX, newX) - std::min(enemyKingX, newX) +
							std::max(enemyKingY, newY) - std::min(enemyKingY, newY) <= 5)
							extraScore += 3;
						add(makeMove(pos, newPos), extraScore);
					}
					else break;
				}
				else break;
			}
		};

		const Square kingPos = getList(state.side)->pos;
		const bool kingMoved = isMoved(board[kingPos]);

		for (auto node = getList(state.side); node; node = node->pNext)
		{
			const Square pos = node->pos;
            const Piece piece = board[pos];
            const byte x = getX(pos);
            const byte y = getY(pos);
            const Code code = get<Code>(piece);

			switch(code)
			{
				case Code::PAWN:
					if (state.side == Color::WHITE)
					{
						Square newPos = makeSquare(x, y + 1);
						if (board[newPos] == Piece::EMPTY)
						{
							if (y == 6) add(makeMove(pos, newPos), 6); /* promotion is 'good' move */
							else add (makeMove(pos, newPos), 1);
							if (y == 1)
							{
								newPos = makeSquare(x, y + 2);
								if (board[newPos] == Piece::EMPTY)
									add(makeMove(pos, newPos), 1);
							}
						}
					}
					else
					{
						Square newPos = makeSquare(x, y - 1);
						if (board[newPos] == Piece::EMPTY)
						{
							if (y == 1) add(makeMove(pos, newPos), 6); /* promotion is 'good' move */
							else add (makeMove(pos, newPos), 1);
							if (y == 6)
							{
								newPos = makeSquare(x, y - 2);
								if (board[newPos] == Piece::EMPTY)
									add(makeMove(pos, newPos), 1);
							}
						}
					}
					break;
				case Code::KNIGHT:
					for (auto delta : knightMoves)
					{
						byte newX = x + delta.x;
						byte newY = y + delta.y;
						if (newX < 8 && 
							newY < 8)
						{
							Square newPos = makeSquare(newX, newY);
							if (board[newPos] == Piece::EMPTY)
							{
								int16_t extraScore = 0;
								if (newX >= 2 && newX <= 5 &&
									newY >= 2 && newY <= 5)
									extraScore++;
								add(makeMove(pos, newPos), extraScore);
							}
						}
					}

					break;
				case Code::BISHOP:
					for (auto delta : bishopMoves)
						processLongRange(pos, delta);
					break;
				case Code::ROOK:
					if (!kingMoved && !state.isCheck && !isMoved(piece))
					{
						if (x == 0)
						{
							if (board[makeSquare(1, y)] == Piece::EMPTY && /* Is B1 empty? */
								board[makeSquare(3, y)] == Piece::EMPTY && /* Is D1 empty? */
								board[makeSquare(2, y)] == Piece::EMPTY)   /* Is C1 empty? */
							{
								if (!isAtacked(makeSquare(3, y), state.side) && /* Is D1 safe? */
									!isAtacked(makeSquare(2, y), state.side))   /* Is C1 safe? */
									add(makeMove(Square::A1, Square::A1, MoveType::LONG_CASTLING));
							}
						}
						else if (x == 7)
						{
							if (board[makeSquare(5, y)] == Piece::EMPTY && /* Is F1 empty? */
								board[makeSquare(6, y)] == Piece::EMPTY)   /* Is G1 empty? */
							{
								if (!isAtacked(makeSquare(5, y), state.side) && /* Is F1 safe? */
									!isAtacked(makeSquare(6, y), state.side))   /* Is G1 safe? */
									add(makeMove(Square::A1, Square::A1, MoveType::SHORT_CASTLING));
							}
						}
					}
					for (auto delta : rookMoves)
						processLongRange(pos, delta);
					break;
				case Code::QUEEN:
					for (auto delta : queenNKingMoves)
						processLongRange(pos, delta);
					break;
				case Code::KING:
					for (auto delta : queenNKingMoves)
					{
						byte newX = x + delta.x;
						byte newY = y + delta.y;
						if (newX < 8 &&
							newY < 8)
						{
							Square newPos = makeSquare(newX, newY);
							if (board[newPos] == Piece::EMPTY)
								add(makeMove(pos, newPos), -2);
						}
					}
					break;
			}
		}
	}
	
public:
	/**
	 * Returns true if `side` in check.
	 */
	[[nodiscard]]
	bool isCheck(Color side) const
	{
		return isAtacked(getList(side)->pos, side);
		/*       king's position~~~~~~~~^ */
	}

	/**
	 * Returns true if current state in checkmate.
	 */
	[[nodiscard]]
	bool isCheckMate() const
	{
		if (isCheck(state.side))
		{
			return availableMoves(true).size() == 0;
		}
		return false;
	}

	/**
	 * Returns true if current side in stalemate.
	 */
	[[nodiscard]]
	bool isStaleMate() const
	{
		return availableMoves(true).size() == 0;
	}

	/**
	 * Get moves that available for `side`.
	 * If `considerChecks` is true then
	 * this function will return only moves that
	 * don't cause check.
	 */
	[[nodiscard]]
	VectorOnStack<Move, max_available_moves> availableMoves(bool considerChecks = true) const
	{
		
		VectorOnStack<MoveEx, max_available_moves> movesEx;
		genMoves(movesEx);
		genCaptures(movesEx);
		VectorOnStack<Move, max_available_moves> moves;
		if (considerChecks)
		{
			/* Need const_cast to do moves
			 * but nothing changes because we
			 * undo them.
			 */
			auto mutThis = const_cast<Engine*>(this);
			
			for (auto& [s, move] : movesEx)
			{
				auto undo = mutThis->doMove(move); /* do move */
				bool check = isCheck(!state.side); /* is this move causing check? */
				mutThis->undoMove(undo); /* undo move */
				if (!check)
					moves.push_back(std::move(move));
			}
		}
		else
			for (auto& [s, move] : movesEx)
				moves.push_back(std::move(move));
		return moves;
	}

private:
	/**
	 * Swaps first and best move in range [index, moves.size()).
	 */
	static void pick(VectorOnStack<MoveEx, max_available_moves>& moves, unsigned index) noexcept
	{
		auto max = index;
		const unsigned size = moves.size();
		for (unsigned i = index + 1; i < size; i++)
				if (moves[i].score > moves[max].score)
				max = i;
		std::swap(moves[index], moves[max]);
	}

	[[nodiscard]]
	Score alphaBeta(Score alpha, Score beta, int depth, unsigned ply)
	{
		if constexpr (enable_think_info)
						 state.positionsTransfered++;
		if (depth <= 0 || ply >= max_ply)
		{
			if constexpr (enable_think_info)
							 state.positionsEvaluated++;
			return evaluate();
		}

		const bool wasCheck = state.isCheck = isCheck(state.side);
		const Color us = state.side;
		bool moved = false;

		if (!wasCheck && depth > 2)
		{
			const unsigned R = 1 + (depth >> 1);

			state.side = !state.side; /* Do zero move */
			auto w = state.enPassantAvailable;
			state.enPassantAvailable = 8;

			Score zeroMove = -alphaBeta(-beta, -alpha, depth - 1 - R, ply + 1 + R);
			// Score zeroMove = -alphaBeta(-beta, -beta +1, depth - 1 - R, ply + 1 + R);

			state.side = !state.side; /* Undo zero move */
			state.enPassantAvailable = w;

			if (zeroMove >= beta)
				return beta;
		}

		if (wasCheck && depth <= 2) /* Compute deeper when check */
			depth++;
		
		VectorOnStack<MoveEx, max_available_moves> moves;
		genCaptures(moves);
		if (ply < 7) /* generate 'quiet' moves only in the beginning of the tree */
			genMoves(moves);
		
		for (unsigned i = 0; i < moves.size(); i++)
		{
			pick(moves, i);
			auto undo = doMove(moves[i].move);
			const bool check = state.isCheck = isCheck(us);
			if (!check)
			{
				moved = true;
				Score score = -alphaBeta(-beta, -alpha, depth - 1, ply + 1);
				undoMove(undo);
				if (score > alpha) alpha = score;
				if (alpha >= beta)
					return alpha;
			}
			else undoMove(undo);
			state.isCheck = wasCheck;
		}

		if (!moved)
		{
			if (wasCheck) return MATE - ply;
			else return DRAW;
		}
		
		return alpha;
	}

public:
	[[nodiscard]]
	Score evaluate() const
	{
		/* Most things that implemented there
		 * was brought from CPW - https://www.chessprogramming.org/Evaluation#Linear_vs._Nonlinear
		 * Thank you, Chess Programming Wiki!
		 */
		
		Score score = ZERO;

		byte whiteCount[toByte(Code::KING) + 1] = {0};
		byte blackCount[toByte(Code::KING) + 1] = {0};
		const bool whiteCheck = isCheck(Color::WHITE);
		const bool blackCheck = isCheck(Color::BLACK);

		for (auto node = whiteList; node; node = node->pNext)
		{
			auto code = get<Code>(board[node->pos]);
			whiteCount[toByte(code)]++;
			switch(code)
			{
				case Code::PAWN:
					score += evalPawn(node->pos);
					break;
				case Code::KNIGHT:
					score += evalKnight(node->pos);
					break;
				case Code::BISHOP:
					score += evalBishop(node->pos);
					break;
				case Code::ROOK:
					score += evalRook(node->pos);
					break;
				case Code::QUEEN:
					score += evalQueen(node->pos);
					break;
				case Code::KING:
					score += evalKing(node->pos);
					break;
			}
		}
		for (auto node = blackList; node; node = node->pNext)
		{
			auto code = get<Code>(board[node->pos]);
			blackCount[toByte(code)]++;
			switch(code)
			{
				case Code::PAWN:
					score -= evalPawn(node->pos);
					break;
				case Code::KNIGHT:
					score -= evalKnight(node->pos);
					break;
				case Code::BISHOP:
					score -= evalBishop(node->pos);
					break;
				case Code::ROOK:
					score -= evalRook(node->pos);
					break;
				case Code::QUEEN:
					score -= evalQueen(node->pos);
					break;
				case Code::KING:
					score -= evalKing(node->pos);
					break;
			}
		}

		/* Bonus for the bishop pair */
		if (whiteCount[toByte(Code::BISHOP)] > 1)
			score += 30;
		if (blackCount[toByte(Code::BISHOP)] > 1)
			score -= 30;

		/* Penalty for having no pawns, as it makes it more difficult to win the endgame */
		if (whiteCount[toByte(Code::PAWN)] == 0)
			score -= 50;
		if (blackCount[toByte(Code::PAWN)] == 0)
			score += 50;
		
		/* Knights lose value as pawns disappear. */
		score += (whiteCount[toByte(Code::KNIGHT)] * whiteCount[toByte(Code::PAWN)]) << 1;
		score -= (blackCount[toByte(Code::KNIGHT)] * blackCount[toByte(Code::PAWN)]) << 1;

		if (whiteCheck)
			score += 20;
		if (blackCheck)
			score -= 20;

		if (state.side == Color::BLACK) score = -score;
		return score;
	}

private:
	[[nodiscard]]
	Score evalPawn(Square square) const
	{
		const byte x = getX(square);
		const byte y = getY(square);
		Score score = PAWN_SCORE;
		if (get<Color>(board[square]) == Color::WHITE)
		{
			constexpr sbyte evalTable[64] = {
				0,   0,  0,  0,  0,  0,  0,  0,
				4,   4,  4,  0,  0,  4,  4,  4,
				6,   8,  2, 10, 10,  2,  8,  6,
				6,   8, 12, 18, 18, 12,  8,  6,
				8,  12, 16, 24, 24, 16, 12,  8,
				12, 16, 24, 32, 32, 24, 16, 12,
				20, 36, 36, 36, 36, 36, 36, 20,
				0,   0,  0,  0,  0,  0,  0,  0,
			};
			if (isWhitePawn(board[makeSquare(x, y - 1)]))
				score -= 5;
			if (board[makeSquare(x, y + 1)] != Piece::EMPTY)
				score -= 5;
			if (x != 0 && isBlack(board[makeSquare(x - 1, y + 1)]))
				score += toByte(get<Code>(board[makeSquare(x - 1, y + 1)])) + 2;
			if (x < 7 && isBlack(board[makeSquare(x + 1, y + 1)]))
				score += toByte(get<Code>(board[makeSquare(x - 1, y + 1)])) + 2;
			score += evalTable[toByte(square)];
		}
		else
		{
			constexpr sbyte evalTable[64] = {
				0,  0,  0,  0,  0,  0,  0,  0,
				20, 36, 36, 36, 36, 36, 36, 20,
				12, 16, 24, 32, 32, 24, 16, 12,
				8, 12, 16, 24, 24, 16, 12,  8,
				6,  8, 12, 18, 18, 12,  8,  6,
				6,  8,  2, 10, 10,  2,  8,  6,
				4,  4,  4,  0,  0,  4,  4,  4,
				0,  0,  0,  0,  0,  0,  0,  0,
			};
			if (isBlackPawn(board[makeSquare(x, y + 1)]))
				score -= 5;
			if (board[makeSquare(x, y - 1)] != Piece::EMPTY)
				score -= 6;
			if (x != 0 && isWhite(board[makeSquare(x - 1, y + 1)]))
				score += toByte(get<Code>(board[makeSquare(x - 1, y + 1)])) + 2;
			if (x < 7 && isWhite(board[makeSquare(x + 1, y + 1)]))
				score += toByte(get<Code>(board[makeSquare(x - 1, y + 1)])) + 2;
			score += evalTable[toByte(square)];
		}
		
		return score;
	}

	[[nodiscard]]
	Score evalKnight(Square square) const
	{
		constexpr sbyte evalTable[64] = {
			0,  4,  8, 10, 10,  8,  4,  0,
			4,  8, 16, 20, 20, 16,  8,  4,
			8, 16, 24, 28, 28, 24, 16,  8,
			10, 20, 28, 32, 32, 28, 20, 10,
			10, 20, 28, 32, 32, 28, 20, 10,
			8, 16, 24, 28, 28, 24, 16,  8,
			4,  8, 16, 20, 20, 16,  8,  4,
			0,  4,  8, 10, 10,  8,  4,  0,
		};
		Score score = KNIGHT_SCORE;
		score += evalTable[toByte(square)];
		return score;
	}

	[[nodiscard]]
	Score evalBishop(Square square) const
	{
		constexpr sbyte evalTable[64] = {
			2, 0,  0,  0,  0,  0, 0, 2,
			0, 8,  4,  4,  4,  4, 8, 0,
			0, 4, 10, 10, 10, 10, 4, 0,
			0, 4, 10, 10, 10, 10, 4, 0,
			0, 4, 10, 10, 10, 10, 4, 0,
			0, 4, 10, 10, 10, 10, 4, 0,
			0, 8,  4,  4,  4,  4, 8, 0,
			2, 0,  0,  0,  0,  0, 0, 2,
		};
		Score score = BISHOP_SCORE;
		score += evalTable[toByte(square)];
		const byte x = getX(square), y = getY(square);
		for (auto delta : bishopMoves)
		{
			byte newX = x, newY = y;
			for (byte i = 1; i < 8; i++)
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					auto piece = board[makeSquare(newX, newY)];
					if (piece != Piece::EMPTY)
					{
						if (get<Color>(piece) != get<Color>(board[square]))
							score += 3;
						break;
					}
					score += 2;
				}
				else break;
			}
		}
		return score;
	}

	[[nodiscard]]
	Score evalRook(Square square) const
	{
		Score score = ROOK_SCORE;
		const auto x = getX(square);
		const auto y = getY(square);
		constexpr SquareEx offsets[4] = {
			{byte(1), byte(0)},
			{byte(-1), byte(0)},
			{byte(0), byte(1)},
			{byte(0), byte(-1)},
		};
		if (get<Color>(board[square]) == Color::WHITE)
		{
			for (auto delta : offsets)
			{
				byte newX = x + delta.x;
				byte newY = y + delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					if (isWhiteRook(board[makeSquare(newX, newY)]) ||
						isWhiteQueen(board[makeSquare(newX, newY)]))
						score += 5;
				}
			}
			constexpr sbyte evalTable[64] = {
				0,  0,  0,  5,  5,  0,  0,  0,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				5, 10, 10, 10, 10, 10, 10,  5,
				0,  0,  0,  0,  0,  0,  0,  0,
			};
			score += evalTable[toByte(square)];
		}
		else
		{
			for (auto delta : offsets)
			{
				byte newX = x + delta.x;
				byte newY = y + delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					if (isBlackRook(board[makeSquare(newX, newY)]) ||
						isBlackQueen(board[makeSquare(newX, newY)]))
						score += 5;
				}
			}
			constexpr sbyte evalTable[64] = {
				0,  0,  0,  0,  0,  0,  0,  0,
				5, 10, 10, 10, 10, 10, 10,  5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				-5,  0,  0,  0,  0,  0,  0, -5,
				0,  0,  0,  5,  5,  0,  0,  0
			};
			score += evalTable[toByte(square)];
		}
		for (auto delta : rookMoves)
		{
			byte newX = x, newY = y;
			for (byte i = 1; i < 8; i++)
			{
				newX += delta.x;
				newY += delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					auto piece = board[makeSquare(newX, newY)];
					if (piece != Piece::EMPTY)
					{
						if (get<Color>(piece) != get<Color>(board[square]))
							score += 3;
						break;
					}
					score += 2;
				}
				else break;
			}
		}
		return score;
	}

	[[nodiscard]]
	Score evalQueen(Square square) const
	{
		Score score = QUEEN_SCORE;
		if (get<Color>(board[square]) == Color::WHITE)
		{
			constexpr sbyte evalTable[64] = {
				-20,-10,-10, -5, -5,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5,  5,  5,  5,  0,-10,
				-5,  0,  5,  5,  5,  5,  0,  0,
				-5,  0,  5,  5,  5,  5,  0, -5,
				-10,  5,  5,  5,  5,  5,  0,-10,
				-10,  0,  5,  0,  0,  0,  0,-10,
				-20,-10,-10, -5, -5,-10,-10,-20
			};
			score += evalTable[toByte(square)];
		}
		else
		{
			constexpr sbyte evalTable[64] = {
				-20,-10,-10, -5, -5,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5,  5,  5,  5,  0,-10,
				-5,  0,  5,  5,  5,  5,  0, -5,
				0,  0,  5,  5,  5,  5,  0, -5,
				-10,  5,  5,  5,  5,  5,  0,-10,
				-10,  0,  5,  0,  0,  0,  0,-10,
				-20,-10,-10, -5, -5,-10,-10,-20
			};
			score += evalTable[toByte(square)];
		}
		return score;
	};

	[[nodiscard]]
	Score evalKing(Square square) const
	{
		Score score = INF;
		constexpr sbyte evalTable[64] = {
			0,   0,  -4,  -10, -10,  -4,   0,   0,
			-4, -4,  -8,  -12, -12,  -8,  -4,  -4,
			-12, -16, -20, -20, -20, -20, -16, -12,
			-16, -20, -24, -24, -24, -24, -20, -16,
			-16, -20, -24, -24, -24, -24, -20, -16,
			-12, -16, -20, -20, -20, -20, -16, -12,
			-4, -4,  -8,  -12, -12,  -8,  -4,  -4,
			0,   0,  -4,  -10, -10,  -4,   0,   0,
		};
		score += evalTable[toByte(square)];
		const auto x = getX(square);
		const auto y = getY(square);
		if (get<Color>(board[square]) == Color::WHITE)
		{
			constexpr SquareEx shieldOffset[3] = {
				{ byte(1), byte(1) },
				{ byte(0), byte(1) },
				{ byte(-1), byte(1) },
			};
			for (auto delta : shieldOffset)
			{
				const auto newX = x + delta.x;
				const auto newY = y + delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					if (get<Color>(board[makeSquare(newX, newY)]) == Color::WHITE)
						score += 8;
				}
			}
		}
		else
		{
			constexpr SquareEx shieldOffset[3] = {
				{ byte(1), byte(-1) },
				{ byte(0), byte(-1) },
				{ byte(-1), byte(-1) },
			};
			for (auto delta : shieldOffset)
			{
				const auto newX = x + delta.x;
				const auto newY = y + delta.y;
				if (newX < 8 &&
					newY < 8)
				{
					if (get<Color>(board[makeSquare(newX, newY)]) == Color::WHITE)
						score -= 8;
				}
			}
		}
		return score;
	}

public:
	class NoMovesAvailable : public std::logic_error
	{
		NoMovesAvailable(GameState state)
			: std::logic_error("Engine::think: there're no moves available"),
			  state(state) { }
	public:
		GameState state;

		friend class Engine;
	};	

	[[nodiscard]]
	Move think(int preferedDepth = 7)
	{
		using namespace std::chrono;
		
		[[maybe_unused]]
		auto now = steady_clock::now();
		
		if constexpr (enable_think_info)
					 {
						 state.positionsTransfered = 0;
						 state.positionsEvaluated = 0;
					 }

		state.isCheck = isCheck(state.side);
		
		VectorOnStack<MoveEx, max_available_moves> moves;
		genCaptures(moves);
		genMoves(moves);

		bool moved = false;
		const Color us = state.side;
		
		Score alpha = -INF;
		Score beta = INF;
		Move bestMove = makeMove(Square::A1, Square::A1);

		std::sort(moves.begin(), moves.end(), std::greater{}); /* Because we're iterating over
																  all available moves we can
																  order all that moves at once. */

		for (auto& [s, move] : moves)
		{
			auto st = doMove(move);
			if (!isCheck(us))
			{
				moved = true;
				Score score = -alphaBeta(-beta, -alpha, preferedDepth, 1);
				undoMove(st);
				if (score > alpha)
				{
					alpha = score;
					bestMove = move;
				}
			}
			else undoMove(st);
		}
		
		if constexpr (enable_think_info)
						 state.time = duration_cast<
							 milliseconds>(steady_clock::now() - now);
		if (!moved)
		{
			if (state.isCheck) throw NoMovesAvailable(GameState::MATE);
			else throw NoMovesAvailable(GameState::DRAW);
		}
		return bestMove;
	}
}; /* class Engine */

class Play
{
private:
	Engine karen;
	std::vector<Move> movesHistory;

protected:
	const Color playerSide;
	unsigned maxMoves = 50;
	
	const Engine& engine() const noexcept { return karen; }

	Play(const Board& board, Color playerSide)
		: karen(board, Color::WHITE), playerSide(playerSide) {}
	Play(Color playerSide)
		: Play(Board::standard(), playerSide) {}

	auto& history() const { return movesHistory; }
	
	[[nodiscard]]
	virtual bool renderBoard(Color side) = 0;
	[[nodiscard]]
	virtual bool inputMove(Move& move) = 0;
	virtual void win() = 0;
	virtual void gameOver() = 0;
	virtual void draw() = 0;
	
public:
	enum class Result : sbyte
	{
		DRAW = 0,
		WHITE_WON = 1,
		BLACK_WON = -1,
		NONE = 100,
	};

	Result operator() (byte depth = 7)
	{
		Color side = Color::WHITE;
		for (unsigned moveNo = 1; !(maxMoves > 0 && moveNo > 2 * maxMoves); moveNo++)
		{
			if (renderBoard(side))
				return Result::NONE;
			Move move = makeMove(Square::A1, Square::A1);
			if (side == playerSide)
			{
				if (inputMove(move))
					return Result::NONE;
			}
			else
			{
				move = karen.think(depth);
			}
			if (move == makeMove(Square::A1, Square::A1))
				throw std::runtime_error("Play:: failed to get move :(");
			
			karen.doMove(move);
			side = !side;
			movesHistory.push_back(move);

			if (karen.isCheckMate())
			{
				if (side == playerSide) win();
				else gameOver();
				return (side == Color::WHITE) ? Result::BLACK_WON : Result::WHITE_WON;
			}
			if (karen.isStaleMate())
			{
				draw();
				return Result::DRAW;
			}
		}
		return Result::DRAW;
	}

};

} /* namespace karen11 */

namespace std
{
    /* karen11::to_string = std::to_string */
	using karen11::to_string;
}

#undef KAREN_OVERLOAD_ENUM_BIN_OPERATOR
#undef KAREN_OVERLOAD_ENUM_UN_OPERATOR
#undef KAREN_OVERLOAD_ENUM_CR_OPERATOR
#undef KAREN_DEFINE_PIECE_CHECKER

/* Karen.hpp ends here */

