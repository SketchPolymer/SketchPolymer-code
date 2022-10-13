import sys

begin=0
save=0
content=""

with open(file="Param.h",mode="r") as fb:
	while True:
		line = fb.readline()
		if not line:break
		
		begin=line.find("//hash")
		if begin != -1:
			save=fb.tell()
			fb.seek(0)
			content=fb.read(save)
			break

para_hash=[2, 3, 4, 2]
para_threadsold=50
para_memory=[0.05, 0.1, 0.1, 0.75]

para_hash[0]=int(sys.argv[1])
para_hash[1]=int(sys.argv[2])
para_hash[2]=int(sys.argv[3])
para_hash[3]=int(sys.argv[4])
para_memory[0]=float(sys.argv[5])
para_memory[1]=float(sys.argv[6])
para_memory[2]=float(sys.argv[7])
para_memory[3]=float(sys.argv[8])
para_threadsold=int(sys.argv[9])


with open(file="Param.h",mode="w") as fb:
	fb.write(content)

	for i in range(1,4+1):
		tempstr="const int d{} = {};\n".format(str(i), str(para_hash[i-1]))
		fb.write(tempstr)
	
	tempstr="const int T = {};\n".format(str(para_threadsold))
	fb.write(tempstr)

	for i in range(1,4+1):
		tempstr="const double m{} = {};\n".format(str(i), str(para_memory[i-1]))
		fb.write(tempstr)

	fb.write("#endif\n")