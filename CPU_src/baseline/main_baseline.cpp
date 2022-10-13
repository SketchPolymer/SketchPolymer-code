#include <bits/stdc++.h>
#include "Benchmark_baseline.h"
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main(int argc, char** argv)
{
        
    std::string path_caida = "/share/datasets/CAIDA2016/CAIDA.dat";
    std::string path_seattle = "../dataset/Seattle/SeattleData_all";
    std::string path_webget = "../dataset/5_zendo/webget-all-simplify.dat";
    
    SeattleBenchmark benchmark(path_seattle);
    //WebgetBenchmark benchmark(path_webget);
    //CAIDABenchmark benchmark(path_caida);

    benchmark.Run(atoi(argv[1]));

    return 0;
}
