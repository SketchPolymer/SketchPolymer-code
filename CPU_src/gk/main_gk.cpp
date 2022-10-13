#include <bits/stdc++.h>
#include "Benchmark_gk.h"
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
using namespace std;

int main(int argc, char** argv)
{
    double epsilon = 0.01;
    
    string path_caida = "/share/datasets/CAIDA2016/CAIDA.dat";
    string path_webget = "../dataset/5_zendo/webget-all-simplify.dat";
    string path_seattle = "../dataset/Seattle/SeattleData_all";

    //WebgetBenchmark benchmark(path_webget);
    //CAIDABenchmark benchmark(path_caida);
    benchmark.Run(atoi(argv[1]),stod(argv[2]));

    return 0;
}
