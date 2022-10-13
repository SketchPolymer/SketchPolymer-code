#collect data from camparision
import sys

structure = sys.argv[1]
dataset = sys.argv[2]

filename = "./{}/result_{}_{}.txt".format(structure, dataset, structure)

with open(file = filename, mode="r") as fb:
	content = ""
	res_ale, res_throughput, res_actual_mem, res_K, res_eps, res_slot = [],[],[],[],[],[]
	while True:
		content = fb.readline()
		if not content: break

		if "Average Log Error: " in content:
			res_ale.append( content[len("Average Log Error: "):-1] )
		if "throughput: " in content:
			res_throughput.append( content[len("throughput: ") : len(content)-len(" item/s\n")+1] )

		if "total size" in content and (structure == "gk" or structure == "kll"):
			temp=content.split(" bytes = ")
			temp=temp[1].split(" Kb")
			res_actual_mem.append( temp[0] )

		if "k_of_sketch" in content and structure == "kll":
			temp = content[:-1] #erase \n
			temp = content.split(": ")
			res_K.append( temp[1][:-1] )

		if "epsilon" in content and structure == "gk":
			temp = content[:-1] #erase \n
			temp = content.split(": ")
			res_eps.append( temp[1][:-1] )		
		
		if "_slot_per_bucket" in content and (structure == "dd" or structure == "baseline"):
			temp = content[:-1] #erase \n
			temp = content.split(": ")
			res_slot.append( temp[1][:-1] )

print("ale")
for item in res_ale:
	print(item)

print("throughput")
for item in res_throughput:
	print(item)

if structure == "gk" or structure == "kll":
	print("ctual_mem")
	for item in res_actual_mem:
		print(item)

if structure == "kll":
	print("K")
	for item in res_K:
		print(item)

if structure == "gk":
	print("eps")
	for item in res_eps:
		print(item)

if structure == "dd" or structure == "baseline":
	print("slot")
	for item in res_slot:
		print(item)