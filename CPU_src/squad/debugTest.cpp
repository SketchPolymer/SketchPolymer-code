#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <boost/math/distributions/normal.hpp>
#include <sys/timeb.h>
#include "qsketch/gk.h"
#include "qsketch/random.h"


#include "lossycount.h"
#include "ranker.h"

//g++ --std=c++11 -m32 -g  -o out test.cpp lossycount.cc prng.cc rand48.cc
using namespace std;


#ifndef CLK_PER_SEC
#ifdef CLOCKS_PER_SEC
#define CLK_PER_SEC CLOCKS_PER_SEC
#endif
#endif

std::unordered_map<string, int> flows;
std::unordered_map<string, int> flows_id;
double theta = 0.01;
double epsilon = 0.01;
double reqquantile = 0.9;
#define LOOP_NUM 1

GK<double>* gksketch;
Random<double>* randomsketch;


//##################################################################


int main (int argc, char* argv[]) {
  double qsketch_eps = 0.01;
  if (argc != 3) {
    std::cout << "Four command-line arguments expected:" << std::endl;
    std::cout << "(1) file name" << std::endl;
    std::cout << "(2) epsilon" << std::endl;
    return -1;
  }

  epsilon = atof(argv[2]);

  gksketch = new GK<double>(epsilon);
  randomsketch = new Random<double> (epsilon);


  int traceSize = 0;

  //Parse the data
  ifstream infile;
  string line;
  uint64_t flow_id;
  double latency;
  infile.open(argv[1]);

  while (!infile.eof() && traceSize < 10000000)
  {
    std::vector<std::string> splits;
    std::string split;

    // Read a line
    getline(infile, line);
    std::istringstream iss(line);

    //split the line by ,
    while (std::getline(iss, split, ','))
    {
      splits.push_back(split);
    }

    flow_id = stoi(splits[0]);
    latency = stoi(splits[1]);

    gksketch->feed(latency);
    randomsketch->feed(latency);  
    ++traceSize;
  }

    int entriesNum = gksketch->GetEnryMapSize();

	int gkmem = entriesNum* 12; // Each enrty is of size 12 byte
	std::cout << "Memory of GK sketch mem:[Mbyte] "<< gkmem / (double) (pow (10.0, 6.0)) << std::endl;

    int rndmem = randomsketch->sketch_memory();
	std::cout << "Memory of Random sketch mem:[Mbyte] "<< rndmem / (double) (pow (10.0, 6.0)) << std::endl;
  return 1;
}