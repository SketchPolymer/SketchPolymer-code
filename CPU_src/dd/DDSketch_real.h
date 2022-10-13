#ifndef _DD_H_
#define _DD_H_
#include <bits/stdc++.h>
#include <hash.h>

#define dd_slot_per_bucket 400
#define dd_bucket_num 1000

class Slot {
public:
	Slot() {}
	Slot(uint32_t s): start(s), end(s + 1), f(1) {}
	~Slot() {}
	void create(Slot &t) {
		start = t.start;
		end = t.end;
		f = 1;
	}
	bool insert(uint32_t T) {
		if (start <= T && T < end) {
			f++;
			return true;
		}
		return false;
	} 
	void merge(Slot &t) {
		assert(t.start > end);
		end = t.end;
		f += t.f;
	}
	bool operator < (Slot &t) {
		return start < t.start;
	}
	uint32_t start;
	uint32_t end;
	uint32_t f;
};

class DDSketch {
public:
	DDSketch() 
	{
		memset(slot, 0, dd_slot_per_bucket * sizeof(Slot));
	}
	~DDSketch() {}
	void insert (uint32_t T) {
		f++;
		for (int i = 0; i < slot_num; ++i) {
			if (slot[i].insert(T)) {
				// std::cout << "insert\n";
				return;
			}
		}
		// fail to insert, create a new slot
		Slot s = Slot(T);
		if (slot_num < dd_slot_per_bucket) {
			slot[slot_num].create(s);
			slot_num++;
			std::sort(slot, slot + slot_num);
			// std::cout << "create\n";
		}
		else {
			// merge
			for (int i = 0; i < dd_slot_per_bucket; ++i) {
				if (s < slot[i]) {
					// std::cout << "merge\n";
					slot[i].merge(s);
					break;
				}
			}
		}
	}
	uint64_t query(double w) 
	{
		int32_t m = f * w;
		for (int i = 0; i < slot_num; ++i) 
		{
			m -= slot[i].f;
			if (m < 0) {
				return pow(1.5, (slot[i].start + slot[i].end) / 2);
			}
		}
		assert(0);
		return 0;
	}
//private:
	uint32_t f;
	uint32_t slot_num = 0;
	Slot slot[dd_slot_per_bucket];
};

template<typename ID_TYPE>
class compare_dd {
public:
	std::vector<DDSketch> bucket;
	uint64_t max_memory;
    uint64_t max_memory_per_bucket;
    uint64_t bucket_num;
    uint64_t item_inserted;

	compare_dd(uint64_t mem) 
	{
		max_memory=mem*1024;

        max_memory_per_bucket = dd_slot_per_bucket * sizeof(Slot);

        double temp = double(max_memory) / double( max_memory_per_bucket );
        bucket_num = uint64_t(floor(temp)); //max_memory=10, max_memory_per_bucket=3 --> 3 buckets only
        //bucket_num = dd_bucket_num;

        for (int i=0;i<bucket_num;++i)
            bucket.push_back(DDSketch());

        item_inserted = 0;
	}
	~compare_dd() {}
	void insert(ID_TYPE id, uint64_t t) {
		uint32_t index = hash(id, 1) % bucket_num;
		uint32_t T = (log(t) / log(1.5) + 0.5);
		bucket[index].insert(T);
		item_inserted++;
	}
	uint64_t query(ID_TYPE id, double w) 
	{
		uint32_t index = hash(id, 1) % bucket_num;
		return bucket[index].query(w);
	}
	uint32_t get_index(ID_TYPE id)
    {
        return hash(id, 1) % bucket_num;
    }
    uint32_t actual_len()
    {
        uint64_t res=0;
        for (int i=0;i<bucket_num;++i)
        {
        	res += bucket[i].slot_num;
        }
        return res;
    }
	void print_status()
    {
        uint64_t res=0;
        for (int i=0;i<bucket_num;++i)
        {
        	res += bucket[i].slot_num;
        }
        
        printf("-------compare_dd status------------\n");

        printf("----max_memory            : %lu bytes = %lu Kb\n", max_memory, max_memory/1024);
        printf("----DD_slot_per_bucket    : %u \n",dd_slot_per_bucket);
        printf("----max_memory_per_bucket : %lu bytes = %lu Kb\n", max_memory_per_bucket, max_memory_per_bucket/1024);
        printf("----bucket_num            : %lu \n",bucket_num);
        printf("----item_inserted         : %lu \n",item_inserted);
        printf("----total len             : %lu\n", res);
        printf("----total size            : %lu bytes = %lu Kb\n", res*sizeof(Slot), res*sizeof(Slot)/1024);
        printf("----nominal size - actual_size = %ld bytes = %ld Kb\n", max_memory-res*sizeof(Slot), (max_memory-res*sizeof(Slot))/1024);

        printf("-------compare_dd status end--------\n");
    }
};

#endif