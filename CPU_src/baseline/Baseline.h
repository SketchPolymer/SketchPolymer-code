#ifndef _BASE_H_
#define _BASE_H_
#include <bits/stdc++.h>
#include <hash.h>

#define bucket_num 2000

class Baseline_Slot {
public:
	Baseline_Slot() {}
	Baseline_Slot(uint32_t s): max_slot(s) {
		slot = new uint64_t [max_slot];
		memset(slot, 0, max_slot * sizeof(uint64_t));
	}
	~Baseline_Slot() {
		delete slot;
	}
	void insert(uint64_t t) {
		uint64_t min_t = slot[0];
		uint32_t min_index = 0;
		for (int i = 0; i < max_slot; ++i) {
			if (slot[i] < min_t) {
				min_t = slot[i];
				min_index = i;
			}
		}
		slot[min_index] = t;
	}
	uint64_t query(uint32_t s) {
		std::sort(slot, slot + max_slot, std::greater<uint64_t>());
		if (s >= max_slot)
			return slot[max_slot - 1];
		return slot[s];
	}

private:
	uint32_t max_slot;
	uint64_t* slot;
};

template<typename ID_TYPE>
class Baseline {
public:
	Baseline_Slot** Slot;
	uint32_t frequency[bucket_num];
	uint64_t max_memory;
	uint64_t item_inserted;
	uint32_t baseline_slot_per_bucket;

	Baseline() {}
	Baseline(uint32_t memory) {
		max_memory = memory * 1024;
		baseline_slot_per_bucket = memory * 1024 / bucket_num / (sizeof(uint32_t) + sizeof(Baseline_Slot));
		std::cout << "slot_per_bucket = "<< baseline_slot_per_bucket << "\n";
		Slot = new Baseline_Slot* [bucket_num];
		for (int i = 0; i < bucket_num; ++i) {
			Slot[i] = new Baseline_Slot(baseline_slot_per_bucket);
		}
		memset(frequency, 0, bucket_num * sizeof(uint32_t));
		item_inserted = 0;
	}
	~Baseline() {
		for (int i = 0; i < bucket_num; ++i) {
			delete Slot[i];
		}
		delete[] Slot;
	}
	void insert(uint64_t id, uint64_t t) {
		uint32_t index = hash(id, 2) % bucket_num;
		Slot[index]->insert(t);
		frequency[index]++;
		item_inserted++;
	}
	uint64_t query(uint64_t id, double w) {
		uint32_t index = hash(id, 2) % bucket_num;
		uint32_t m = (1 - w) * frequency[index];
		return Slot[index]->query(m);
	}

	void print_status()
    {        
        printf("-------baseline status------------\n");

        printf("----max_memory               : %lu bytes = %lu Kb\n", max_memory, max_memory/1024);
        printf("----baseline_slot_per_bucket : %u \n",baseline_slot_per_bucket);
        printf("----bucket_num               : %d \n",bucket_num);
        printf("----item_inserted            : %lu \n",item_inserted);

        printf("-------baseline status end--------\n");
    }
	
};

#endif