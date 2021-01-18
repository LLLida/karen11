#include "ConsolePlay.hpp"

using namespace karen11;

int main(int argc, char** argv)
{
	if (ConsolePlay::parseOptions(argc, argv))
		return 0;
	ConsolePlay play;
	play();

	return 0;
}
