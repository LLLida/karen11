# Brief
Karen11 is lightweight chess engine written in C++17 for school project. She doesn't very small good but can beat a regular human.


# Installation 
_Warning this section is not done_
You'll need [cmake](https://cmake.org/) and [C++17 compiler](https://en.cppreference.com/w/cpp/compiler_support/17) to build the program.

# Detail
I tried to make Karen as simpler as I can: without hash table and difficult heuristics. At the moment Karen uses alpha-beta algorithm, null move prunning, move ordering and some other little heuristics. It allows she to compute for depth 8 in less than second. The source code is now over than 3000 lines of pure C++. Karen is written only for educational purposes she can't compete with other chess engines(but I hope she will).

# Usage
1. Install Karen.
2. Launch Karen in command prompt.
3. Play!

# License
Copyright (c) 2021 Adil Mokhammad
`Karen` is made available under the terms of the GPLv3 license.
See [license](https://www.gnu.org/licenses/gpl-3.0.en.html) for details.
