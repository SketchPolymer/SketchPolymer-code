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


#include "lossycountwSgk.h"
//#include "ranker.h"

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



double computePercentile(vector <double> vec, double item)
{
  double percent;
  int count = 0;

  for(auto const& value: vec) {
    if(item >= value) {
        count=count+1;
    }
  }

  percent=(double)(count)/(double) ((vec.size() - 1));

  return percent;
}


double query_for_val_vector(vector <double> vec, double rank) {
  std::sort (vec.begin(), vec.end());
  int index = vec.size() * rank;

  return vec[index];
}

//##########################################################################################


int tailLatencyErrTest(std::string filename, LCU_type* space_saving) {
  int nonZerosLatency = 0;
  int traceSize = 0;
  unordered_map<uint64_t, vector <double>> map;// key (flow id), vector of latencies of the flow id
  unordered_map<uint64_t, uint64_t> countersMap;// key (flow id) , counter
	int total_id = 1;

  //Parse the data
  ifstream infile;
  string line;
  uint64_t flow_id;
  double latency;
  infile.open(filename);
  ofstream myfile;

  myfile.open ("parsed_dataset.txt");
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

    //Parsing src ip as flow id
    std::istringstream srcip(splits[0]);
    string s = srcip.str();

		if (flows_id[s] == 0)
      flows_id[s] = total_id++;

    //Parsing latency
    std::istringstream latencystr(splits[5]);
    std::getline(latencystr, split, '.');
    split.erase(0, 1);
    latency = stoi(split);

    myfile << flows_id[s] << "," << latency << endl;

    if (latency != 0) {
      ++nonZerosLatency;
    }

    //cout <<"Updating ss with flow: " << flows_id[s] << ", latency: " << latency << endl; 
    LCU_UpdateLatency(space_saving, flows_id[s], latency);
    
    map[flows_id[s]].push_back(latency);
    ++countersMap[flows_id[s]];
    ++traceSize;
  }

  myfile.close();

  double memory;
  //cout << "**nonZerosLatency: " << nonZerosLatency << endl;
  //cout << "printing THE FINAL space saving" << endl;

  int thresh = ceil(theta * traceSize);

  std::map<uint32_t, uint32_t> ssElements = LCU_Output(space_saving, thresh);

  int hh_num = 0;
  //memory = memory / (double) (pow (10.0, 6.0));

  double error = 0, errorAll = 0;
  
  for(auto const& element: ssElements) {
    int i = element.first;
    if (!map[i].empty() && countersMap[i] > thresh)
    {
      ++hh_num;
      //cout <<" Testing flow id: " << i << endl;
      double est = LCU_QuantileQuery(space_saving, i, reqquantile);

      if(est == 0) {
        cout << "estimated quantile is 0!! flow id: " << i << endl;
      }
      double estQuantile = computePercentile(map[i], est);
      double errorPercentile = abs(estQuantile - reqquantile); 

      error = (double)((double)estQuantile - (double)reqquantile) / (double)reqquantile;
      error = pow(error, 2);
      errorAll += error;
      if (errorPercentile > epsilon) {
        cout << "There is an EXCEPTION!!, flow id: " << i << ", counter in test: " << countersMap[i] << ", counter in ss: " << element.second << endl;
      }
      cout << "estQuantile: " << estQuantile << ", Error percentile: " << errorPercentile  <<endl;

    }
  }

  errorAll = (double) errorAll / (double)hh_num;
  errorAll = sqrt(errorAll);
  cout << "**number of heavyhitters: "<< hh_num << ", trace size: " << traceSize << endl;

  return 0;
}



//##################################################################

int tailLatencyErrTestParsed(std::string filename, LCU_type* space_saving) {
  int nonZerosLatency = 0;
  int traceSize = 0;
  unordered_map<uint64_t, vector <double>> map;// key (flow id), vector of latencies of the flow id
  unordered_map<uint64_t, uint64_t> countersMap;// key (flow id) , counter

  //Parse the data
  ifstream infile;
  string line;
  uint64_t flow_id;
  double latency;
  infile.open(filename);

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

    if (latency != 0) {
      ++nonZerosLatency;
    }

    LCU_UpdateLatency(space_saving, flow_id, latency);

    map[flow_id].push_back(latency);
    ++countersMap[flow_id];
    ++traceSize;
  }

  double memory;
  //cout << "**nonZerosLatency: " << nonZerosLatency << endl;
  //cout << "printing THE FINAL space saving" << endl;

  int thresh = ceil(theta * traceSize);

  std::map<uint32_t, uint32_t> ssElements = LCU_Output(space_saving, thresh);

  int hh_num = 0;
  //memory = memory / (double) (pow (10.0, 6.0));

  double error = 0, errorAll = 0;
  
  double mem = LCU_ComputeMem(space_saving);

  mem = (double) mem / (double) (pow (10.0, 6.0));

  for(auto const& element: ssElements) {
    int i = element.first;
    if (!map[i].empty() && countersMap[i] > thresh)
    {
      ++hh_num;
      //cout <<" Testing flow id: " << i << endl;
      double est = LCU_QuantileQuerywGsamples(space_saving, i, reqquantile);

      double excatVal = query_for_val_vector(map[i], reqquantile);

      if(est == excatVal) {
        
        cout << "estQuantile: " << reqquantile << ", Error percentile: " << 0  << ", MEM: " << mem << endl;
        continue;
      } else {
        double estQuantile = computePercentile(map[i], est);
        double errorPercentile = abs(estQuantile - reqquantile);

        error = (double)((double)estQuantile - (double)reqquantile) / (double)reqquantile;
        error = pow(error, 2);
        errorAll += error;
        if (errorPercentile > epsilon) {
          cout << "There is an EXCEPTION!!, flow id: " << i << ", counter in test: " << countersMap[i] << ", counter in ss: " << element.second << ", estVal: " << est << endl;
        }
        cout << "estQuantile: " << estQuantile << ", Error percentile: " << errorPercentile  << ", MEM: " << mem << endl;
      }
    }
  }

  errorAll = (double) errorAll / (double)hh_num;
  errorAll = sqrt(errorAll);
  cout << "number of heavyhitters: "<< hh_num << ", trace size: " << traceSize << ", MEM: " << mem << endl;

  return 0;
}


//####################################################################

int performanceTest(std::string filename, LCU_type* space_saving, double epsilon) {
  int traceSize = 0;
  unordered_map<uint64_t, vector <double>> map;// key (flow id), vector of latencies of the flow id
  unordered_map<uint64_t, uint64_t> countersMap;// key (flow id) , counter

  //Parse the data
  ifstream infile;
  string line;
  uint64_t flow_id;
  double latency;
  infile.open(filename);
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
    std::cout<<"flow_id: "<<flow_id<<" "<<"latency: "<<latency<<std::endl;

    map[flow_id].push_back(latency);
    ++countersMap[flow_id];
    ++traceSize;
  }

	clock_t begint, endt;
	struct timeb begintb, endtb;
	double time;
  unsigned int numItems = 0;

	begint = clock();
	ftime(&begintb);

  for(auto pair : map) {
    for(auto latency: pair.second) {
      LCU_UpdateLatency(space_saving, pair.first, latency);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Update_SS_Rnd %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);


  //Space Saving Only

	begint = clock();
	ftime(&begintb);

  for(auto pair : map) {
    for(auto latency: pair.second) {
      LCU_Update(space_saving, pair.first);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Update_SS %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);

  //GK Only
  GK<double>* qsketch = new GK<double>(epsilon);
	begint = clock();
	ftime(&begintb);

  for(auto pair : map) {
    for(auto latency: pair.second) {
      qsketch->feed(latency);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Update_Rnd %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);

  return 0;
}

//##################################################################

/*
int main (int argc, char* argv[]) {
  double qsketch_eps = 0.01;
  if (argc != 5) {
    std::cout << "Four command-line arguments expected:" << std::endl;
    std::cout << "(1) file name" << std::endl;
    std::cout << "(2) theta " << std::endl;
    std::cout << "(3) epsilon" << std::endl;
    std::cout << "(3) quantile" << std::endl;
    return -1;
  }

  int percenile = atoi(argv[4]);
  reqquantile = (double) percenile / (double) 100;

  theta = atof(argv[2]);
  epsilon = atof(argv[3]);

  //theta = (double) theta / (double) (1+sqrt(epsilon));

  long long internal_counters = (int) ((double) (pow(theta, -1) * (double) pow(epsilon,-0.5)));
  long long samples_num = 4* ((int) ((double) (pow(theta, -2) * (double) pow(epsilon,-2)) * pow((double) internal_counters, -1)));

  qsketch_eps = epsilon / (double) 2.0;



  LCU_type* ss = LCU_Init((double) 1/ (double) internal_counters, qsketch_eps, samples_num);
  //int result = tailLatencyErrTestParsed(argv[1], ss);
  //std::cout << "m: " << internal_counters << ", eps: " << epsilon << ", theta: " << theta << ", qsketch eps: " << qsketch_eps << ", quantile: "<< reqquantile << std::endl;
  int result = performanceTest(argv[1], ss, epsilon);
  tailLatencyErrTest(argv[1], ss);
  return result;
}
*/