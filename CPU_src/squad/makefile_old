quasi: testQuasi.cpp lossycount.cc prng.cc rand48.cc
	g++ --std=c++11 -m32 -g -O3  testQuasi.cpp lossycount.cc prng.cc rand48.cc -o quasi

squad_random: testSquadRandom.cpp lossycountwS.cc prng.cc rand48.cc
	g++ --std=c++11 -mavx2 -m32 -g -O3  testSquadRandom.cpp lossycountwS.cc prng.cc rand48.cc -o squad_random

squad_gk: testSquadGk.cpp lossycountwSgk.cc prng.cc rand48.cc
	g++ --std=c++11 -m32 -g -O3  testSquadGk.cpp lossycountwSgk.cc prng.cc rand48.cc -o squad_gk

square: testSquare.cpp
	g++ --std=c++11 -m32 -g -O3  testSquare.cpp -o square
