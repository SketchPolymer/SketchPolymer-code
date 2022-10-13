#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <bits/stdc++.h>
#include <hash.h>
#include <Mmap.h>
#include "CorrectDetector.h"
#include "kll_sketch.hpp"
#include "Param.h"
#include <time.h>


template<typename ID_TYPE>
class compare_kll
{
public:
    uint64_t max_memory;
    uint32_t k_of_sketch;
    uint64_t max_memory_per_bucket;
    uint64_t bucket_num;
    uint64_t item_inserted;
    
    std::vector<datasketches::kll_sketch<uint64_t>> array_kll;
    
    compare_kll(uint64_t mem, uint32_t KK, uint64_t total_item)
    {
        max_memory=mem*1024;
        k_of_sketch=KK;

        {
            datasketches::kll_sketch<uint64_t> sketch_temp(k_of_sketch);
            for (int i=0;i<total_item;++i)
                sketch_temp.update(i);
            max_memory_per_bucket = sketch_temp.get_serialized_size_bytes();
    
            double temp = double(max_memory) / double( max_memory_per_bucket ); 
            bucket_num = uint64_t(floor(temp));
        }
        
        //compute max_memory_per_bucket and bucket_num again. a bucket does not need to insert all item
        {
            uint64_t new_total_item = total_item/bucket_num;
            datasketches::kll_sketch<uint64_t> sketch_temp1(k_of_sketch);
            for (int i=0;i<new_total_item;++i)
                sketch_temp1.update(i);
            max_memory_per_bucket = sketch_temp1.get_serialized_size_bytes();
    
            double temp1 = double(max_memory) / double( max_memory_per_bucket ); 
            bucket_num = uint64_t(floor(temp1));
        }

        for (int i=0;i<bucket_num;++i)
            array_kll.push_back(datasketches::kll_sketch<uint64_t>(k_of_sketch));

        item_inserted = 0;
    } 
    
    void insert(ID_TYPE id, uint64_t timestamp)
    {
        uint32_t index = hash(id, 1024) % array_kll.size();
        array_kll[index].update(timestamp);
        item_inserted++;
    }
    uint64_t query(ID_TYPE id, double w)
    {
        uint32_t index = hash(id, 1024) % array_kll.size();

        const double fractions[1] {w};
        auto quantiles = array_kll[index].get_quantiles(fractions, 1);

        return quantiles[0];
    }
    uint32_t get_index(ID_TYPE id)
    {
        return hash(id, 1024) % array_kll.size();
    }
    uint32_t actual_len()
    {
        uint64_t res=0;
        for (int i=0;i<bucket_num;++i)
        {
            res += array_kll[i].get_serialized_size_bytes();
        }
        return res;
    }
    void print_status()
    {
        uint32_t res=0;
        for (int i=0;i<bucket_num;++i)
        {
            res += array_kll[i].get_serialized_size_bytes();
        }
        
        printf("-------compare_kll status------------\n");

        printf("----max_memory            : %lu bytes = %lu Kb\n", max_memory, max_memory/1024);
        printf("----k_of_sketch           : %u \n",k_of_sketch);
        printf("----max_memory_per_bucket : %lu bytes = %lu Kb\n", max_memory_per_bucket, max_memory_per_bucket/1024);
        printf("----bucket_num            : %lu \n",bucket_num);
        printf("----item_inserted         : %lu \n",item_inserted);
        printf("----total size            : %u bytes = %u Kb\n", res, res/1024);
        printf("----nominal size - actual_size = %ld bytes = %ld Kb\n", max_memory-res, (max_memory-res)/1024);

        printf("-------compare_kll status end--------\n");
    }
};


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

    void Run(uint32_t memory, uint32_t KKK) 
    {
        uint32_t running_length = length;

        compare_kll<uint64_t>* KLL_sketch = new compare_kll<uint64_t>(memory,KKK,running_length);

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 
        std::map<uint64_t, uint32_t> id_map;
        std::set<uint64_t> id_set;

        clock_t begin,finish;
        clock_t total=0;
        double totaltime;
        uint32_t actual_length=0;

        for (int i = 0; i < running_length; ++i) 
        {
            if (i % (1000000) ==0) printf("i = %d\n",i);
            if (dataset[i].timestamp == 0) continue;

            actual_length++;

            if (id_set.find(dataset[i].id) == id_set.end()) 
            {
                id_set.insert(dataset[i].id);
                id_map[dataset[i].id] = 0;
            }
            id_map[dataset[i].id]++;

            #ifdef TIME_BASED
            if (id_map[dataset[i].id] > 1) 
            {
                correct_detector->insert(dataset[i].id, 0, dataset[i].timestamp);

                begin=clock();
                KLL_sketch->insert(dataset[i].id, dataset[i].timestamp);
                finish=clock();
                total += finish-begin;
            }
            #else
            if (id_map[dataset[i].id] > 1) 
            {
                //correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                //KLL_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
            }
            #endif
        }
        
        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(actual_length) / totaltime;
        std::cout <<"throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        double log_error = 0;
        int num = 0;
        double query_w = 0.95;
        for (auto i : id_map) 
        {
            if (i.second < 500)
                continue;
            num++;

            uint64_t predict = KLL_sketch->query(i.first, query_w);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );

            // std::cout << log2(predict) << " " << log2(truth) << " " << "\n";
            // assert(0);
            // printf("id=%lu, predict=%lu, truth=%lu, relative_error=%f \n",i.first,predict,truth,relative_error);
            // if (num==20) break;
        }
        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";

        KLL_sketch->print_status();

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

    void Run(uint32_t memory, uint32_t KK) 
    {
        uint32_t running_length = length;

        compare_kll<uint64_t>* KLL_sketch = new compare_kll<uint64_t>(memory,KK,running_length);

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 
        std::map<uint64_t, uint32_t> id_map;
        std::set<uint64_t> id_set;

        clock_t begin,finish;
        clock_t total=0;
        double totaltime;
        uint32_t actual_length=0;

        for (int i = 0; i < running_length; ++i) 
        {
            if (i % (1000000) ==0) printf("i = %d\n",i);
            if (dataset[i].timestamp == 0) continue;

            actual_length++;

            if (id_set.find(dataset[i].id) == id_set.end()) 
            {
                id_set.insert(dataset[i].id);
                id_map[dataset[i].id] = 0;
            }
            id_map[dataset[i].id]++;

            #ifdef TIME_BASED
            if (id_map[dataset[i].id] > 1) 
            {
                correct_detector->insert(dataset[i].id, 0, dataset[i].timestamp);

                begin=clock();
                KLL_sketch->insert(dataset[i].id, dataset[i].timestamp);
                finish=clock();

                total += finish-begin;
            }
            #else
            if (id_map[dataset[i].id] > 1) 
            {
                //correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                //KLL_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
            }
            #endif
        }
        
        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(actual_length) / totaltime;
        std::cout <<"throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        double log_error = 0;
        int num = 0;
        double query_w = 0.95;
        for (auto i : id_map) 
        {
            if (i.second < 1000)
                continue;
            num++;

            uint64_t predict = KLL_sketch->query(i.first, query_w);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );

            // std::cout << log2(predict) << " " << log2(truth) << " " << "\n";
            // assert(0);
            // printf("id=%lu, predict=%lu, truth=%lu, relative_error=%f \n",i.first,predict,truth,relative_error);
            // if (num==20) break;
        }
        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";

        KLL_sketch->print_status();

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
    }
    ~CAIDABenchmark() {}

    void Run(uint32_t memory, uint32_t KK) 
    {
    
        uint32_t running_length = 20000000;

        compare_kll<uint64_t>* KLL_sketch = new compare_kll<uint64_t>(memory,KK,running_length);

        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>(); 
        std::map<uint64_t, uint32_t> id_map;
        std::map<uint64_t, uint64_t> last_time;
        std::set<uint64_t> id_set;

        clock_t begin,finish;
        clock_t total=0;
        double totaltime;

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
                correct_detector->insert(dataset[i].id, last_time[dataset[i].id], dataset[i].timestamp);
                //KLL_sketch->insert(dataset[i].id, last_time[dataset[i].id], dataset[i].timestamp);

                begin=clock();
                KLL_sketch->insert(dataset[i].id, dataset[i].timestamp - last_time[dataset[i].id]);
                finish=clock();

                total += finish-begin;
            }
            last_time[dataset[i].id] = dataset[i].timestamp;
            
            #else
            if (id_map[dataset[i].id] > 1) 
            {
                correct_detector->insert(dataset[i].id, last_time[dataset[i].id], i);
                //KLL_sketch->insert(dataset[i].id, last_time[dataset[i].id], i);
            }
            last_time[dataset[i].id] = i;
            #endif
        }
        
        totaltime=(double)(total)/CLOCKS_PER_SEC;
        std::cout <<"time taken: "<<totaltime<<" seconds"<< std::endl;
        double throughput = double(running_length) / totaltime;
        std::cout <<"throughput: "<<std::fixed<<std::setprecision(4)<< throughput <<" item/s"<< std::endl;

        double log_error = 0;
        int num = 0;
        double query_w = 0.95;
        for (auto i : id_map) 
        {
            if (i.second < 1000)
                continue;
            num++;
            
            uint64_t predict = KLL_sketch->query(i.first, query_w);
            uint64_t truth = correct_detector->query(i.first, query_w);
            log_error += fabs( log2(predict) - log2(truth) );

            /*
             std::cout << log2(predict) << " " << log2(truth) << " " << "\n";
            // assert(0);
             uint32_t index=KLL_sketch->get_index(i.first);
             printf("size of arrgk[%u] = %lu, real len of %lu = %lu \n",index,KLL_sketch->array_kll[index].get_serialized_size_bytes(),
                i.first,correct_detector->get_id_len(i.first));
             printf("id=%lu, predict=%lu, truth=%lu, log_error=%f \n\n",i.first,predict,truth,( log2(predict) - log2(truth) ));
             if (num==10) break;
             */
        }
        std::cout << "Estimate: " << num << "\n";
        std::cout << "Average Log Error: " << log_error / num << "\n";
        std::cout << "query_w: " << query_w<< "\n";

        KLL_sketch->print_status();

        std::cout << "\n";
    }

//private:
    std::string filename;
    LoadResult load_result;
    CAIDA_Tuple* dataset;
    uint64_t length;
};

#endif
