#ifndef _CORRECT_H_
#define _CORRECT_H_

#include <bits/stdc++.h>
#include "Param.h"

template<typename ID_TYPE>
class CorrectDetector {
public:
	CorrectDetector() {
		record.clear();
		id_set.clear();
	}
	~CorrectDetector() {}
	void insert(ID_TYPE id, uint64_t ts, uint64_t te) {
		if (id_set.find(id) == id_set.end()) {
			id_set.insert(id);
			record[id] = {};
		}
		record[id].push_back(te - ts);
	}
	uint64_t query(ID_TYPE id, double w) {
		uint32_t index = record[id].size() * w;
		sort(record[id].begin(), record[id].end());
		 //std::cout << "correct: \n";
		 //std::cout << record[id].size() << "\n";
		 std::map<uint32_t, uint32_t> mp;
		 for (auto i : record[id]) {
		 	uint32_t t = (0.5 + log(i) / log(log_base));
		 	mp[t]++;
		 }
		 /*
		 for (auto i = mp.rbegin(); i != mp.rend(); ++i) {
		 	std::cout << i->first << ", " << i->second << "\n";
		 }
		 std::cout << std::endl;
		 */
		return record[id][index - 1];
	}
	uint64_t get_id_len(ID_TYPE id)
	{
		return record[id].size();
	}

private:
	std::map<ID_TYPE, std::vector<uint64_t>> record;
	std::set<ID_TYPE> id_set;
};



#endif