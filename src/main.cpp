#include "ConsolePlay.hpp"
	
int main(int argc, char** argv)
{
	using namespace karen11;
	if (ConsolePlay::parseOptions(argc, argv))
		return 0;
	ConsolePlay play;
	play();
	return 0;
}


