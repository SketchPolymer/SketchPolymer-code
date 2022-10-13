#include <bits/stdc++.h>
#include "Benchmark_SketchPolymer.h"
using namespace std;

int main(int argc, char** argv) {
    string path_seattle = "../dataset/Seattle/SeattleData_all";
	string path_caida = "/share/datasets/CAIDA2016/CAIDA.dat";
    string path_webget = "../dataset/5_zendo/webget-all-simplify.dat";

    SeattleBenchmark benchmark(path_seattle);
    //WebgetBenchmark benchmark(path_webget);
    //CAIDABenchmark benchmark(path_caida);
    benchmark.Run(atoi(argv[1]));
    
	return 0;
}