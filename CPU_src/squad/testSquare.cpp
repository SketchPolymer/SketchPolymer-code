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
#include <unordered_map>


#include "ranker.h"

//g++ --std=c++11 -m32 -g  -o out testSample.cpp
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


class sampleEntry
{
public:
    unsigned long long flow;
    double latency;
};

/* Link list node */
class sampleNode {
public:
    sampleEntry data;
    sampleNode* next;
};

unsigned long long nb_processed = 0;
std::vector<sampleEntry> samples;
long long sample_size;


int xcount = 0;
int ycount = 0;
int gcount = 0;

void addToSample(long long samples_num, int flowid, double latency) {
	++nb_processed;
	sampleEntry se;
	se.flow = flowid;
	se.latency = latency;

  if (samples_num >= nb_processed) {
      samples.push_back(se);
  } else {
      ++gcount;
      unsigned long long j = rand() % (nb_processed);
      if (j < samples_num) {
        samples[j] = se;
        ++ycount;
      } else {
        ++xcount;
      }
  }
}


double query_for_val_vector(vector <double> vec, double rank) {
  std::sort (vec.begin(), vec.end());
  int index = vec.size() * rank;

  return vec[index];
}

int computeFreqfromSample(int flowid, double rank, double* quantile) {

	std::vector<double> sample_buff;
    int counter = 0;
 // std::cout << "Compute frequency from sample for flow id: " << flowid << std::endl;
	for(auto const& value: samples) {
		//std::cout << "value from samples, flow id: : " << value.flow << std::endl;
   
		if (value.flow == flowid) {
		//	std::cout << "To sample buffer:! flow: " << value.flow <<" latency: " <<value.latency <<  std::endl;
			sample_buff.push_back(value.latency);
            ++counter;
		}
	}

    int w;
    if (nb_processed > sample_size) {
        w = (int) ((double) nb_processed / (double)sample_size);
    } else {
        w = 1;
    }

  if (!sample_buff.empty())
    *quantile = query_for_val_vector(sample_buff, rank);
  else {
    std::cout << "Sample buffer is empty so ignore! " << std::endl;
    *quantile = 0;
  }

  return w*counter;
}

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





//##################################################################

int sampleLatencyErrTestParsed(std::string filename, long long samples_size) {
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

  while (!infile.eof())
  {
    std::vector<std::string> splits;
    std::string split;

    // Read a line
    getline(infile, line);
    if(line.empty()) continue;
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

    addToSample(samples_size, flow_id, latency);

    map[flow_id].push_back(latency);
    ++countersMap[flow_id];
    ++traceSize;
  }

  double memory;
  int thresh = ceil(theta * traceSize);
  //thresh = 46007; //TODO

  int hh_num = 0;
  double error = 0, errorAll = 0;
  double mem; // 2byte for latency, 4 byte for flow

/*
  if (traceSize < sample_size)
    mem = traceSize * 8;
  else
    mem = mem = samples_size * 8;
*/

  mem = mem = samples_size * 8;
  mem = (double) mem / (double) (pow (10.0, 6.0));
  double est;
  
  for(auto const& flow: countersMap) {
    int flowid = flow.first;
//    std::cout << "flowid: " << flowid << " real counter: " << countersMap[flowid] << " est counter: " << computeFreqfromSample(flow_id, reqquantile, &est) << " threshold: " << thresh << std::endl;
//    if (!map[flowid].empty() && countersMap[flowid] > thresh && computeFreqfromSample(flow_id, reqquantile, &est) > thresh)
    if (!map[flowid].empty() && countersMap[flowid] > thresh)
    {
      ++hh_num;
      cout <<" Testing flow id: " << flowid << endl;
      int est_cnt = computeFreqfromSample(flowid, reqquantile, &est);
      std::cout << "flowid: " << flowid << " real counter: " << countersMap[flowid] << " est counter: " << est_cnt << " threshold: " << thresh << std::endl;

      double excatVal = query_for_val_vector(map[flowid], reqquantile);


      cout << "est: " << est << ", Error excatVal: " << excatVal << endl;

      if(est == excatVal) {
        
        cout << "estQuantile: " << reqquantile << ", Error percentile: " << 0  << ", MEM: " << mem << endl;
        continue;
      } else {
        double estQuantile = computePercentile(map[flowid], est);
        double errorPercentile = abs(estQuantile - reqquantile);

        error = (double)((double)estQuantile - (double)reqquantile) / (double)reqquantile;
        error = pow(error, 2);
        errorAll += error;
        if (errorPercentile > epsilon) {
          cout << "There is an EXCEPTION!!, flow id: " << flowid << ", counter in test: " << countersMap[flowid] << ", counter in ss: " << flow.second << ", estVal: " << est << endl;
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

int performanceTest(std::string filename, long long samples_size, double epsilon) {
  int traceSize = 0;
  unordered_map<uint64_t, vector <double>> map;// key (flow id), vector of latencies of the flow id
  unordered_map<uint64_t, uint64_t> countersMap;// key (flow id) , counter

  //Parse the data
  ifstream infile;
  string line;
  uint64_t flow_id;
  double latency;
  infile.open(filename);

  while (!infile.eof())
  {
    std::vector<std::string> splits;
    std::string split;

    // Read a line
    getline(infile, line);
    if(line.empty()) continue;
    std::istringstream iss(line);

    //split the line by ,
    while (std::getline(iss, split, ','))
    {
      splits.push_back(split);
    }

    flow_id = stoi(splits[0]);
    latency = stoi(splits[1]);

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
      addToSample(samples_size, pair.first, latency);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Update_Square %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);
  /*
  std::cout << "counts, done rand() operation: " << gcount << std::endl;
  std::cout << "counts, No replacment operation: " << xcount << std::endl;
  std::cout << "counts, replacment operation: " << ycount << std::endl;
  


  double est;
  double reqquantile = 0.9;
  numItems = 0;
	
  begint = clock();
	ftime(&begintb);

  for(auto pair : map) {
    for(auto latency: pair.second) {
      computeFreqfromSample(pair.first, reqquantile, &est);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Query_Square %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);
*/
  return 0;
}


//##################################################################

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

  double delta = 0.01;

  sample_size = (int) ((double) (pow(theta, -1) * (double) pow(epsilon,-2) * (double) log(pow(delta, -1))));


  //std::cout << "samples_size: " << sample_size << ", eps: " << epsilon << ", theta: " << theta << ", delta: " << delta <<", quantile: "<< reqquantile << std::endl;

  //int result = sampleLatencyErrTestParsed(argv[1], sample_size);

  int result = performanceTest(argv[1], sample_size, epsilon);
  return result;
}