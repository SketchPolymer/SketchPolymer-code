#ifndef _DD_H_
#define _DD_H_

#include <bits/stdc++.h>
#include "Param.h"
#include "hash.h"

template<typename ID_TYPE>
class Stage1 {
public:
	Stage1() {}
	Stage1(uint32_t memory) {
		size = memory * 1024 / sizeof(uint8_t) / d1;
		array = new uint8_t* [d1];
		for (int i = 0; i < d1; ++i) {
			array[i] = new uint8_t [size];
			memset(array[i], 0, size * sizeof(uint8_t));
		}
	}
	~Stage1() {
		for (int i = 0; i < d1; ++i) {
			delete[] array[i];
		}
		delete[] array;
	}
	void insert(ID_TYPE id) {
		for (int i = 0; i < d1; ++i) {
			uint32_t index = hash(id, i) % size;
			if (array[i][index] + 1) 
				array[i][index]++;
		}
	}
	uint32_t query(ID_TYPE id) {
		uint32_t result = inf;
		for (int i = 0; i < d1; ++i) {
			uint32_t index = hash(id, i) % size;
			result = MIN(result, array[i][index]);
		}
		return result;
	}

private:
	uint8_t** array;
	uint32_t size;
};

class Slot {
public:
	Slot() {
		f = 0;
		#ifdef _LOG_BIG_ENDIAN_
		t = inf;
		#else
		t = 0;
		#endif
	}
	void init() {
		f = 0;
		#ifdef _LOG_BIG_ENDIAN_
		t = inf;
		#else
		t = 0;
		#endif
	}
	void update(uint32_t tt) {
		f++;
		#ifdef _LOG_BIG_ENDIAN_
		t = MIN(t, tt);
		#else
		t = MAX(t, tt);
		#endif
	}
	void query(uint32_t* ff, uint32_t* tt) {
		*ff = f;
		*tt = t;
	}
private:
	uint32_t f;
	uint32_t t;
};

template<typename ID_TYPE>
class Stage2 {
public:
	Stage2() {}
	Stage2(uint32_t memory) {
		size = memory * 1024 / sizeof(Slot) / d2;
		slot = new Slot* [d2];
		for (int i = 0; i < d2; ++i) {
			slot[i] = new Slot [size];
			// memset(slot[i], 0, size * sizeof(Slot));
			for (int j = 0; j < size; ++j) {
				slot[i][j].init();
			}
		}
	} 
	~Stage2() {
		for (int i = 0; i < d2; ++i) {
			delete[] slot[i];
		}
		delete[] slot;
	} 
	void insert(ID_TYPE id, uint32_t t) {
		for (int i = 0; i < d2; ++i) {
			uint32_t index = hash(id, 33 + i) % size;
			slot[i][index].update(t);
		}
	}
	void query(ID_TYPE id, uint32_t* f, uint32_t* t) {
		*f = inf;
		#ifdef _LOG_BIG_ENDIAN_
		*t = 0;
		#else 
		*t = inf;
		#endif
		for (int i = 0; i < d2; ++i) {
			uint32_t index = hash(id, 33 + i) % size;
			uint32_t ff, tt;
			slot[i][index].query(&ff, &tt);
			*f = MIN(*f, ff);
			#ifdef _LOG_BIG_ENDIAN_
			*t = MAX(*t, tt);
			#else
			*t = MIN(*t, tt);
			#endif
		}
	}
private: 
	Slot** slot;
	uint32_t size;
};

template<typename ID_TYPE>
class Stage4 {
public:
	Stage4() {}
	Stage4(uint32_t memory) {
		size = memory * 1024 / sizeof(uint32_t) / d4;
		bf = new uint32_t* [d4];
		for (int i = 0; i < d4; ++i) {
			bf[i] = new uint32_t [size];
			memset(bf[i], 0, size * sizeof(uint32_t));
		}
	}
	~Stage4() {
		for (int i = 0; i < d4; ++i) {
			delete[] bf[i];
		}
		delete[] bf;
	}
	void insert(ID_TYPE id, uint32_t t) {
		for (int i = 0; i < d4; ++i) {
			uint32_t index = hash(id - t, 23 + i) % (32 * size);
			uint32_t j = index / 32, k = index % 32;
			bf[i][j] |= (1 << k);
		}
	}
	bool query(ID_TYPE id, uint32_t t) {
		for (int i = 0; i < d4; ++i) {
			uint32_t index = hash(id - t, 23 + i) % (32 * size);
			uint32_t j = index / 32, k = index % 32;
			uint32_t result = ((bf[i][j] >> k) & 1);
			if (!result)
				return false;
		}
		return true;
	}
private:
	uint32_t** bf;
	uint32_t size;
};

template<typename ID_TYPE>
class Stage3 {
public:
	Stage3() {}
	virtual ~Stage3() {}
	virtual void insert(ID_TYPE id, uint32_t t) {}
	virtual uint32_t query(ID_TYPE id, uint32_t w) = 0;
	virtual void print_status() {}
};

template<typename ID_TYPE>
class Stage3CM : public Stage3<ID_TYPE> {
public:
	Stage3CM() {};
	Stage3CM(uint32_t memory) {
		size = memory * 1024 / sizeof(uint8_t) / d3;
		counter = new uint8_t* [d3];
		for (int i = 0; i < d3; ++i) {
			counter[i] = new uint8_t [size];
			memset(counter[i], 0, size * sizeof(uint8_t));
		}
	}
	~Stage3CM() {
		for (int i = 0; i < d3; ++i) {
			delete counter[i];
		}
		delete counter;
	}
	void insert(ID_TYPE id, uint32_t t) {
		for (int i = 0; i < d3; ++i) {
			uint32_t index = hash(id * t, i) % size;
			if (counter[i][index] ^ 0xff)//!!! when you change the type of counter, remember to change this line
				counter[i][index]++;
		}
	}
	uint32_t query(ID_TYPE id, uint32_t t) {
		uint32_t result = inf;
		for (int i = 0; i < d3; ++i) {
			uint32_t index = hash(id * t, i) % size;
			result = MIN(result, counter[i][index]);
		}
		return result;
	}
	void print_status() {}

private:
	uint8_t** counter;
	uint32_t size;
};

template<typename ID_TYPE>
class Stage3Tower : public Stage3<ID_TYPE> {
public:
	Stage3Tower() {}
	Stage3Tower(uint32_t memory) {
		mem = memory;
		uint32_t cell_num = memory * 1024 / sizeof(uint32_t) / 3;
		counter = new uint32_t* [3];
		for (int i = 0; i < 3; ++i) {
			size[i] = cell_num * 32 / bits[i];
			counter[i] = new uint32_t[cell_num];
			memset(counter[i], 0, cell_num * sizeof(uint32_t));
		}
	}
	~Stage3Tower() {
		for (int i = 0; i < 3; ++i) 
			delete[] counter[i];
		delete[] counter;
	}
	uint32_t get(int i, int j, int s, int e) {
		// get the number in bit s-e in counter[i][j]
		return (counter[i][j] & (((1 << (e - s)) - 1) << s)) >> s;
	}
	void increment(int i, int j, int s, int e) {
		// increment bit s-e in counter[i][j]
		uint32_t result = get(i, j, s, e);
		if (result <= max_value[i]) {
			counter[i][j] = ((counter[i][j] & (~(((1 << (e - s)) - 1) << s))) | ((result + 1) << s));
			
			uint32_t now = get(i, j, s, e);
			assert(result + 1 == now);
		}
			
	}
	void insert(ID_TYPE id, uint32_t t) {
		for (int i = 0; i < 3; ++i) {
			uint32_t index = hash(id + t, i) % size[i];
			uint32_t cell = index * bits[i] / 32;
			uint32_t res = index - cell * 32 / bits[i];
			increment(i, cell, res * bits[i], res * bits[i] + bits[i]);
			//printf("index=%d, cell=%d, res=%d, counter=%d\n",index,cell,res,counter[i][cell]);
		}
	}
	uint32_t query(ID_TYPE id, uint32_t t) {
		uint32_t value = 255;
		for (int i = 0; i < 3; ++i) {
			uint32_t index = hash(id + t, i) % size[i];
			uint32_t cell = index * bits[i] / 32;
			uint32_t res = index - cell * 32 / bits[i];
			uint32_t result = get(i, cell, res * bits[i], res * bits[i] + bits[i]);
			if (result <= max_value[i]) 
				value = MIN(value, result);
		}
		return value;
	}
	void print_status()
	{
		std::cout<<"memory = "<<mem<<std::endl;
		uint32_t cell_num = mem * 1024 / sizeof(uint32_t) / 3;
		std::cout<<"cell num = "<<cell_num<<std::endl;
		for (int i=0;i<3;++i) printf("size [%d] = %d\n",i,size[i]);

		uint32_t overflow[3]={};
		for (int i=0;i<3;++i)
		{
			uint32_t number_of_comparisons = 32 / bits[i];

			for (int j=0;j<cell_num;++j)
			{
				uint32_t MAX = max_value[i] + 1;
				for (int k=0;k<number_of_comparisons;++k)
				{
					uint32_t temp = counter[i][j] & MAX;
					if (temp == MAX) overflow[i] += 1;
					MAX <<= bits[i];
				}
			}
		}
		for (int i=0;i<3;++i) printf("overflow [%d] = %d\n",i,overflow[i]);
	}
private:
	uint32_t bits[3] = {2, 4, 8};
	uint32_t max_value[3] = {2, 14, 254};
	uint32_t size[3];
	uint32_t** counter;
	uint32_t mem=-1;
};

template<typename ID_TYPE>
class SketchPolymer {
public:
	SketchPolymer() {}
	SketchPolymer(uint32_t memory) {
		mem=memory;
		stage1 = new Stage1<ID_TYPE>(m1 * memory);
		stage2 = new Stage2<ID_TYPE>(m2 * memory);
		stage3 = new Stage3CM<ID_TYPE>(m3 * memory);
		stage4 = new Stage4<ID_TYPE>(m4 * memory);
	}
	~SketchPolymer() {
		delete stage1;
		delete stage2;
		delete stage3;
		delete stage4;
	}
	void insert(ID_TYPE id, uint64_t ts, uint64_t te) {
		if (stage1->query(id) < T) {
			stage1->insert(id);
			return;
		}
		uint32_t t = (log(te - ts) / log(log_base) + 0.5);
		stage2->insert(id, t);
		stage3->insert(id, t);
		stage4->insert(id, t);
	}
	uint64_t query(ID_TYPE id, double w) {
		//std::cout << "ours: \n";
		uint32_t f, t;
		stage2->query(id, &f, &t);
		assert(t < inf);
		#ifdef _LOG_BIG_ENDIAN_
		std::cout << f << " " << t << "\n";
		int m = w * f;
		while (m > 0) {
			if (stage4->query(id, t)) {
				uint32_t result = stage3->query(id, t);
				m -= result;
				if (result)
					std::cout << t << ", " << result << std::endl;
			}
			t++;
		}
		#else
		int m = (1 - w) * f;
		while (m > 0 && t != 0) {
			if (stage4->query(id, t)) {
				uint32_t result = stage3->query(id, t);
				m -= result;
				//if (result)
				//std::cout << t << ", " << result << std::endl;
			}
			t--;
		}
		#endif
		return pow(log_base, t+0.5);
	}
	void print_status()
	{
		//stage4->print_status();
		printf("parameter:%d %d %d %d %.2f %.2f %.2f %.2f %d %.3f %d\n",d1,d2,d3,d4,m1,m2,m3,m4,T,log_base,mem);
	}
private:
	Stage1<ID_TYPE>* stage1;
	Stage2<ID_TYPE>* stage2;
	Stage3<ID_TYPE>* stage3;
	Stage4<ID_TYPE>* stage4;
	uint32_t mem;
};

#endif