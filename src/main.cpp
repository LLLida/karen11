#include "ConsolePlay.hpp"

int main(int argc, char** argv)
{
	using namespace karen11;
	if (ConsolePlay::parseOptions(argc, argv))
		return 0;
	ConsolePlay play;
	play.random.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	play();
	return 0;
}


