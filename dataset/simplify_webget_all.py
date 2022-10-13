import math

with open(file="./webget-all-simplify.dat", mode='w') as target:
	for i in range(1):
		with open(file="./webget-all.csv", mode='r') as fb:
			content = fb.readline()
			row = 1
			while (True):
				content = fb.readline().split(",")
				if len(content) == 1:break
				row += 1
	
				first = content[3][1:-1].split(".")#address
				first = "".join(first)

				if content[4][1]=="-":
					second = content[4][2:-1]#fetchtime can't be negative
				else:
					second = content[4][1:-1]#fetchtime
	
				line=first + "\t" + second + "\n"
				if len(line) != 3: #second can't be 0
					target.write(first + "\t" + second + "\n")
	
			print("row = ",row)

L=[]
max_fetch = -1
min_fetch = 100000000000
with open(file="./webget-all-simplify.dat", mode='r') as fb: 
	d={}
	row = 0
	while True:
		content = fb.readline()
		if content == "":break
		row += 1

		content = list(map(int,content.split("\t")))

		first,second = content[0],content[1]
		if second > max_fetch: max_fetch=second
		if second < min_fetch: min_fetch=second
		if second<0:
			print(content)
			print(row)
			break
		if first in d:
			d[first] += 1
		else:
			d[first]=1
	
	print("item_num: ",len(d))
	print("max_fetch = ", max_fetch)
	print("min_fetch = ", min_fetch)
	print("Range1.5 = ", math.log(max_fetch,1.5)-math.log(min_fetch,1.5))
	print("Range1.1 = ", math.log(max_fetch,1.1)-math.log(min_fetch,1.1))
	L = sorted(d.items(),key=lambda item:item[1],reverse=True)

with open(file="./webget-all-stat.dat", mode='w') as target:
	for item in L:
		target.write(str(item[0])+"\t"+str(item[1])+"\n")

