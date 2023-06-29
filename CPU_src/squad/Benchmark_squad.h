#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <bits/stdc++.h>
#include <hash.h>
#include <Mmap.h>
#include "CorrectDetector.h"
#include "Param.h"
#include <time.h>
#include "testSquadGk.h"

struct Seattle_Tuple {
    uint64_t timestamp;
    uint64_t id;
};


class SeattleBenchmark 
{
public:
    SeattleBenchmark(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        int datalen=0;
        std::vector<double> num;

        std::ifstream file(PATH.c_str());
        while( ! file.eof() )
        {
            double temp;
            file>>temp;
            num.push_back(temp*100000);
            datalen++;
        }
        file.close();
        std::cout<<datalen<<" "<<num.size()<<"\n";

        //seattle: 688file, each file has 99*99 item

        dataset = new Seattle_Tuple[datalen];
        int index = 0;
        for (int filenum=0;filenum<688;filenum++)
        {
            for (int flow_id=0;flow_id<99;flow_id++)
            {
                for (int i=0;i<99;i++)
                {
                    dataset[index].id=index % (9801) * 41 + 41;
                    dataset[index].timestamp=num[index];
                    index++;
                }   
                  
            }
        }
        std::cout<<"index="<<index<<", datalen="<<datalen<<std::endl;
        //printf("dataset_len=%d, index=%d, large=%d, mid=%d, small=%d\n",datalen,index,large_stream,mid_stream,small_stream);
        
        length = datalen;

    }
    ~SeattleBenchmark() {}

    void Run(uint32_t memory, double eps, double quantile = 0.95)
    {
        uint32_t running_length = length;
        std::cout<<"running_length = "<<running_length<<std::endl;

        double reqquantile = quantile;
        double theta = 0.000074; //500 / 6743089
        std::cout << "theta: " << theta << "\n";

        epsilon = eps;       
        long long internal_counters = (int) ((double) (pow(theta, -1) * (double) pow(epsilon,-0.5)));
        long long samples_num = 4* ((int) ((double) (pow(theta, -2) * (double) pow(epsilon,-2)) * pow((double) internal_counters, -1)));
        double qsketch_eps = epsilon / (double) 2.0;
        
        LCU_type* squad = LCU_Init((double) 1/ (double) internal_counters, qsketch_eps, samples_num);
        double size_squad = LCU_Size(squad)/1024;
        std::cout<<"size: "<<size_squad<<" kb"<<std::endl;

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 
        std::map<uint64_t, uint32_t> id_map;

        //insert
        {
            std::map<uint64_t, uint64_t> last_time;
            std::set<uint64_t> id_set;

            clock_t begin,finish;
            clock_t total=0;
            double totaltime;
            uint32_t actual_length = 0;

            for (int i = 0; i < running_length; ++i) 
            {
                if (i%1000000==0) printf("i = %d\n",i);
                if (dataset[i].timestamp == 0) continue;
                
                if (id_set.find(dataset[i].id) == id_set.end()) 
                {
                    id_set.insert(dataset[i].id);
                    id_map[dataset[i].id] = 0;
                }
                id_map[dataset[i].id]++;
    
                #ifdef TIME_BASED
                if (id_map[dataset[i].id] > 1) 
                {
                    uint64_t latency = dataset[i].timestamp;
                    long double to_insert = static_cast<long double>(latency);

                    begin = clock();
                    LCU_UpdateLatency(squad, dataset[i].id, to_insert);
                    finish=clock();
                    total += finish-begin;

                    actual_length++;
                    correct_detector->insert(dataset[i].id, 0, dataset[i].timestamp);
                }
                last_time[dataset[i].id] = dataset[i].timestamp;
                #else
                if (id_map[dataset[i].id] > 1) 
                {
                    correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                    //icde_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
                }
                last_time[dataset[i].id] = i;
                #endif
            }

            totaltime=(double)(total)/CLOCKS_PER_SEC;
            std::cout <<"insert time taken: "<<totaltime<<" seconds"<< std::endl;
            double throughput = double(actual_length) / totaltime;
            std::cout <<"insert throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;
        }

        //query        
        clock_t begin,finish;
        clock_t total=0;
        double totaltime;

        double log_error = 0;
        int num = 0;
        double query_w = quantile;
        for (auto i : id_map) 
        {
            if (i.second < 500)
                continue;
            num++;

            begin = clock();
            long double est = LCU_QuantileQuery(squad, i.first, query_w);
            finish=clock();
            total += finish-begin;

            uint64_t predict = static_cast<uint64_t>(est);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );
        }

        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"query time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(num) / totaltime;
        std::cout <<"query throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";
        
        std::cout << "\n";
    }
//private:
    std::string filename;
    LoadResult load_result;
    Seattle_Tuple* dataset;
    uint64_t length;
};

struct Webget_Tuple {
    uint64_t timestamp;
    uint64_t id;
};

class WebgetBenchmark 
{
public:
    WebgetBenchmark(std::string PATH) 
    {
        
        std::cout<<"dataset = "<<PATH<<std::endl;

        int datalen=0;
        std::vector<uint64_t> num1;
        std::vector<uint64_t> num2;

        for (int i=0;i<2;++i)
        {
            std::ifstream file(PATH.c_str());
            while( ! file.eof() )
            {
                uint64_t temp1;
                uint64_t temp2;
                file>>temp1>>temp2;
                if (temp2==0) continue;
    
                num1.push_back(temp1);
                //num.push_back(temp2 * 123465);
                num2.push_back(temp2);
                
                datalen+=1;
            }
            file.close();
        }
        
        std::cout<<datalen<<" "<<num1.size()<<" "<<num2.size()<<"\n";

        //Webget: 9368 flow,some timestamp is 0

        dataset = new Webget_Tuple[datalen];
        for (int i=0;i<datalen;i++)
        {
            dataset[i].id=num1[i];
            dataset[i].timestamp=num2[i];
        }
        
        length = datalen;
    }
    ~WebgetBenchmark() {}

    void Run(uint32_t memory, double eps, double quantile = 0.95)
    {
        uint32_t running_length = length;
        std::cout<<"running_length = "<<running_length<<std::endl;

        double reqquantile = quantile;
        double theta = 0.0000626; //1000 / 1596_3378
        std::cout << "theta: " << theta << "\n";

        epsilon = eps;       
        long long internal_counters = (int) ((double) (pow(theta, -1) * (double) pow(epsilon,-0.5)));
        long long samples_num = 4* ((int) ((double) (pow(theta, -2) * (double) pow(epsilon,-2)) * pow((double) internal_counters, -1)));
        double qsketch_eps = epsilon / (double) 2.0;
        
        LCU_type* squad = LCU_Init((double) 1/ (double) internal_counters, qsketch_eps, samples_num);
        double size_squad = LCU_Size(squad)/1024;
        std::cout<<"size: "<<size_squad<<" kb"<<std::endl;

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 

        std::map<uint64_t, uint32_t> id_map;

        //insert
        {
            std::map<uint64_t, uint64_t> last_time;
            std::set<uint64_t> id_set;

            clock_t begin,finish;
            clock_t total=0;
            double totaltime;
            uint32_t actual_length = 0;

            for (int i = 0; i < running_length; ++i) 
            {
                if (i%1000000==0) printf("i = %d\n",i);
                if (id_set.find(dataset[i].id) == id_set.end()) 
                {
                    id_set.insert(dataset[i].id);
                    id_map[dataset[i].id] = 0;
                }
                id_map[dataset[i].id]++;
    
                #ifdef TIME_BASED
                if (id_map[dataset[i].id] > 1) 
                {
                    uint64_t latency = dataset[i].timestamp;
                    long double to_insert = static_cast<long double>(latency);

                    begin = clock();
                    LCU_UpdateLatency(squad, dataset[i].id, to_insert);
                    finish=clock();
                    total += finish-begin;

                    actual_length++;
                    correct_detector->insert(dataset[i].id, 0, dataset[i].timestamp);
                }
                last_time[dataset[i].id] = dataset[i].timestamp;
                #else
                if (id_map[dataset[i].id] > 1) 
                {
                    correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                    //icde_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
                }
                last_time[dataset[i].id] = i;
                #endif
            }

            totaltime=(double)(total)/CLOCKS_PER_SEC;
            std::cout <<"insert time taken: "<<totaltime<<" seconds"<< std::endl;
            double throughput = double(actual_length) / totaltime;
            std::cout <<"insert throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;
        }

        //query        
        clock_t begin,finish;
        clock_t total=0;
        double totaltime;

        double log_error = 0;
        int num = 0;
        double query_w = quantile;
        for (auto i : id_map) 
        {
            if (i.second < 1000)
                continue;
            num++;

            begin = clock();
            long double est = LCU_QuantileQuery(squad, i.first, query_w);
            finish=clock();
            total += finish-begin;

            uint64_t predict = static_cast<uint64_t>(est);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );
        }

        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"query time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(num) / totaltime;
        std::cout <<"query throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";

        std::cout << "\n";
    }
//private:
    std::string filename;
    LoadResult load_result;
    Webget_Tuple* dataset;
    uint64_t length;
};

struct CAIDA_Tuple {
    uint64_t timestamp;
    uint64_t id;
};

class CAIDABenchmark 
{
public:
    CAIDABenchmark(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;
        load_result = Load(PATH.c_str());
        dataset = (CAIDA_Tuple*)load_result.start;
        length = load_result.length / sizeof(CAIDA_Tuple);
        std::cout<<"lenght="<<length<<"\n";
    }
    ~CAIDABenchmark() {}

    uint64_t getLowBits(uint64_t num, int n) {
        uint64_t mask = (1 << n) - 1;
        return num & mask;
    }

    void Run(uint32_t memory, double eps, double quantile = 0.95)
    {
        uint32_t running_length = 20000000;

        double reqquantile = quantile;
        double theta = 0.00008; //1000 / 2000_0000
        std::cout << "theta: " << theta << "\n";

        epsilon = eps;       
        long long internal_counters = (int) ((double) (pow(theta, -1) * (double) pow(epsilon,-0.5)));
        long long samples_num = 4* ((int) ((double) (pow(theta, -2) * (double) pow(epsilon,-2)) * pow((double) internal_counters, -1)));
        double qsketch_eps = epsilon / (double) 2.0;
        
        LCU_type* squad = LCU_Init((double) 1/ (double) internal_counters, qsketch_eps, samples_num);
        double size_squad = LCU_Size(squad)/1024;
        std::cout<<"size: "<<size_squad<<" kb"<<std::endl;

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 
        std::map<uint64_t, uint32_t> id_map;
        std::map<uint64_t, uint64_t> last_time;
        std::set<uint64_t> id_set;

        //insert
        clock_t begin,finish;
        clock_t total=0;
        double totaltime;
        uint32_t actual_length = 0;
        for (int i = 0; i < running_length; ++i) 
        {
            if (i%1000000==0) printf("i = %d\n",i);
            if (id_set.find(dataset[i].id) == id_set.end()) 
            {
                id_set.insert(dataset[i].id);
                id_map[dataset[i].id] = 0;
            }
            id_map[dataset[i].id]++;

            #ifdef TIME_BASED
            if (id_map[dataset[i].id] > 1 && dataset[i].timestamp > last_time[dataset[i].id]) 
            {
                uint64_t latency = dataset[i].timestamp - last_time[dataset[i].id];
                long double to_insert = static_cast<long double>(latency);

                begin = clock();
                LCU_UpdateLatency(squad, dataset[i].id, to_insert);
                finish=clock();
                total += finish-begin;

                actual_length++;
                correct_detector->insert(dataset[i].id, last_time[dataset[i].id], dataset[i].timestamp);
            }
            last_time[dataset[i].id] = dataset[i].timestamp;
            #else
            if (id_map[dataset[i].id] > 1) 
            {
                correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                //icde_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
            }
            last_time[dataset[i].id] = i;
            #endif
        }
        
        std::cout<<"actual len = "<<actual_length<<std::endl;
        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"insert time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(actual_length) / totaltime;
        std::cout <<"insert throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        //query
        total=0;
        double log_error = 0;
        int num = 0;
        double query_w = quantile;
        for (auto i : id_map) 
        {
            if (i.second < 1000)
                continue;
            num++;
            begin = clock();
            long double est = LCU_QuantileQuery(squad, i.first, query_w);
            finish=clock();
            total += finish-begin;

            uint64_t predict = static_cast<uint64_t>(est);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );
        }
        
        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"query time taken: "<<totaltime<<" seconds"<< std::endl;
        throughput = double(num) / totaltime;
        std::cout <<"query throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";

        std::cout << "\n";
    }

//private:
    std::string filename;
    LoadResult load_result;
    CAIDA_Tuple* dataset;
    uint64_t length;
};

#endif


