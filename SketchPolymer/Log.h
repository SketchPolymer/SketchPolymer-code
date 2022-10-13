/*
this header file is deprecated !
*/

#ifndef _LOG_H_
#define _LOG_H_

#include <bits/stdc++.h>
#include "Param.h"
#include "hash.h"
#include "SketchPolymer.h"

template<typename ID_TYPE>
class TowerSketch {
public:
	TowerSketch() {}
	TowerSketch(uint32_t memory) {
		uint32_t cell_num = memory * 1024 / sizeof(uint32_t) / 5;
		counter = new uint32_t* [4];
		for (int i = 0; i < 4; ++i) {
			size[i] = cell_num * 32 / bits[i];
			counter[i] = new uint32_t[cell_num];
			memset(counter[i], 0, cell_num * sizeof(uint32_t));
		}
		layer_size = cell_num;
		layer = new uint32_t [layer_size];
		memset(layer, 0, layer_size * sizeof(uint32_t));
	}
	~TowerSketch() {
		for (int i = 0; i < 4; ++i) {
			delete[] counter[i];
		}
		delete[] counter;
		delete[] layer;
	}
	void increment(int i, int j, int s, int e) {
		// increment bit s-e in counter[i][j]
		uint32_t result = ((counter[i][j] & (((1 << (e - s)) - 1) << s)) >> s);
		if (result <= max_value[i]) {
			counter[i][j] = ((counter[i][j] & (~(((1 << (e - s)) - 1) << s))) | ((result + 1) << s));
			uint32_t now = ((counter[i][j] & (((1 << (e - s)) - 1) << s)) >> s);
		}
	}
	void insert(ID_TYPE id) {
		for (int i = 0; i < 4; ++i) {
			uint32_t index = hash(id, i) % size[i];
			uint32_t cell = index * bits[i] / 32;
			uint32_t res = index - cell * 32 / bits[i];
			increment(i, cell, res, res + bits[i]);
		}
	}
	uint32_t query(ID_TYPE id) {
		uint32_t value = inf;
		for (int i = 0; i < 4; ++i) {
			uint32_t index = hash(id, i) % size[i];
			uint32_t cell = index * bits[i] / 32;
			uint32_t res = index - cell * 32 / bits[i];
			uint32_t result = ((counter[i][cell] & (((1 << (bits[i])) - 1) << res)) >> res);
			if (result <= max_value[i]) 
				value = MIN(value, result);
		}
		return value;
	}
	void set(ID_TYPE id, uint32_t t) {
		uint32_t index = hash(id, 4) % layer_size;
		layer[index] = MAX(layer[index], t);
	}
	void get(ID_TYPE id, uint32_t* f, uint32_t* t) {
		*f = query(id);
		*t = layer[hash(id, 4) % layer_size];
	}
private:
	uint32_t** counter;
	uint32_t* layer;
	uint32_t bits[4] = {2, 4, 8, 16};
	uint32_t max_value[4] = {2, 14, 254, 65534};
	uint32_t size[4];
	uint32_t layer_size;
};

template<typename ID_TYPE>
class Log {
public:
	Log() {}
	Log(uint32_t memory) {
		tower = new TowerSketch<ID_TYPE>(0.5 * memory);
		stage4 = new Stage4Tower<ID_TYPE>(0.5 * memory);
	}
	~Log() {
		delete tower;
		delete stage4;
	}
	void insert(ID_TYPE id, uint64_t ts, uint64_t te) {
		tower->insert(id);
		uint32_t t = (log(te - ts) / log(log_base) + 0.5);
		if (tower->query(id) >= T) {
			tower->set(id, t);
			stage4->insert(id, t);
		}
	}
	uint64_t query(ID_TYPE id, double w) {
		uint32_t f, t;
		tower->get(id, &f, &t);
		int m = (1 - w) * f;
		while (m > 0 && t != 0) {
			uint32_t result = stage4->query(id, t);
			m -= result;
			t--;
		}
		return pow(log_base, t);
	}
	void print_status()
	{
		stage4->print_status();
	}
private:
	TowerSketch<ID_TYPE>* tower;
	Stage4<ID_TYPE>* stage4;
};

#endif