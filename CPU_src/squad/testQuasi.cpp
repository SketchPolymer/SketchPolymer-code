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

  while (!infile.eof())
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

  std::cout << "threashold is: " << thresh << std::endl;
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
  cout <<"**number of heavyhitters: "<< hh_num << ", trace size: " << traceSize << endl;

  return 0;
}


//##################################################################
int test5tupleParsingflows(std::string filename, LCU_type* space_saving) {
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

  myfile.open("srcdst_parsed_dataset.txt");

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

    //Parsing src ip as flow id
    std::istringstream srcip(splits[0]);
    std::istringstream dstip(splits[1]);
    //std::istringstream srcport(splits[2]);
    //std::istringstream dstport(splits[3]);

    //string s = srcip.str() + dstip.str() + srcport.str() + dstport.str();
    string s = srcip.str() + dstip.str();

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

  std::cout << "Number of unique flows is: " << countersMap.size() << std::endl;

  double memory;

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
  cout <<"**number of heavyhitters: "<< hh_num << ", trace size: " << traceSize << endl;

  return 0;
}


//##################################################################
int tailLatencyErrTestUnparsed(std::string filename, LCU_type* space_saving) {
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

  myfile.open ("fb_parsed_dataset_new.txt");

  while (!infile.eof())
  {
    std::vector<std::string> splits;
    std::string split;

    // Read a line
    getline(infile, line);
    if(line.empty()) continue;
    std::istringstream iss(line);

    //split the line by ,
    while (std::getline(iss, split, ' '))
    {
      splits.push_back(split);
    }

    //Parsing src ip as flow id
    string s = splits[0];

		if (flows_id[s] == 0)
      flows_id[s] = total_id++;

    //Parsing latency
    string slatency = splits[1];
    latency = std::stod(slatency);


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

  while (!infile.eof())
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
      double est = LCU_QuantileQuery(space_saving, i, reqquantile);

      if(est == 0) { //TODO
        //cout << "estimated quantile is 0!! flow id: " << i << endl;

        double excatVal = query_for_val_vector(map[i], reqquantile);

        if (est == excatVal)
          cout << "estQuantile: " << reqquantile << ", Error percentile: " << 0  << ", MEM: " << mem << endl;

        continue;
      }
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

  errorAll = (double) errorAll / (double)hh_num;
  errorAll = sqrt(errorAll);
  cout << "number of heavyhitters: "<< hh_num << ", trace size: " << traceSize << ", MEM: " << mem << endl;

  return 0;
}




//##################################################################

int tailLatencyErrTestParsed2(std::string filename, LCU_type* space_saving) {
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

    LCU_UpdateLatency(space_saving, flow_id, latency);
  
    map[flow_id].push_back(latency);
    ++countersMap[flow_id];
    ++traceSize;
  }

  std::cout << "Number of unique flows is: " << countersMap.size() << std::endl;

  double memory;
  cout << "**nonZerosLatency: " << nonZerosLatency << endl;
  //cout << "printing THE FINAL space saving" << endl;

  int thresh = ceil(theta * traceSize);

  //thresh = 46007; //TODO
  std::cout << "Threshold: " << thresh << std::endl;
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
      double est = LCU_QuantileQuery(space_saving, i, reqquantile);
      double excatVal = query_for_val_vector(map[i], reqquantile);

      cout << "Est quantile is: " << est << " exactVal is: " << excatVal << endl;

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

//##################################################################

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
      LCU_UpdateLatency(space_saving, pair.first, latency);
      ++numItems;
    }
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Update_Quasi %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);
  //std::cout << "number of new sketch allocation is: " << returnNumNewSketch() << std::endl;
/*
  //Space Saving Only
  numItems = 0;
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

  numItems = 0;
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
	printf( "Update_GK %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);


  int thresh = ceil(theta * traceSize);
  thresh = 46007; //TODO

  std::map<uint32_t, uint32_t> ssElements = LCU_Output(space_saving, thresh);

  int query_items = 0;
 	begint = clock();
	ftime(&begintb);

  for(auto const& element: ssElements) {
    int i = element.first;
    if (!map[i].empty() && countersMap[i] > thresh)
    {
      double est = LCU_QuantileQuery(space_saving, i, reqquantile);
      ++query_items;
    }
  }
	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Query_Quasi %d pairs took %lfs %f epsilon\n", query_items, time, epsilon);


  numItems = 0;
  vector<double> quantiles;
  quantiles.push_back(0.1);
  quantiles.push_back(0.2);
  quantiles.push_back(0.3);
  quantiles.push_back(0.4);
  quantiles.push_back(0.5);
  quantiles.push_back(0.6);
  quantiles.push_back(0.7);
  quantiles.push_back(0.8);
  quantiles.push_back(0.9);
  quantiles.push_back(0.99);


	begint = clock();
	ftime(&begintb);

  for(auto q : quantiles) {
      qsketch->finalize ();
      qsketch->query_for_value(q);
      ++numItems;
  }

	endt = clock();
	ftime(&endtb);
	time = ((double)(endt-begint))/CLK_PER_SEC;
	printf( "Query_GK %d pairs took %lfs %f epsilon\n", numItems, time, epsilon);
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

  //theta = (double) theta / (double) (1+sqrt(epsilon));

  long long internal_counters = 2 * ((int) ((double) (pow(theta, -1) * (double) pow(epsilon,-1)))) + 2;

  qsketch_eps = epsilon / (double) 2.0;

  LCU_type* ss = LCU_Init((double) 1/ (double) internal_counters, qsketch_eps);


  //std::cout << "m: " << internal_counters << ", eps: " << epsilon << ", theta: " << theta << ", qsketch eps: " << qsketch_eps << ", quantile: "<< reqquantile << std::endl;
  //int result = tailLatencyErrTestParsed2(argv[1], ss);

  //int result = tailLatencyErrTestUnparsed(argv[1], ss);
  int result = performanceTest(argv[1], ss, epsilon);

  return result;
}