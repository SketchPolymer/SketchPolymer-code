import sys

begin=0
save=0
content=""
rest=""

with open(file="Param.h",mode="r") as fb:
	while True:
		line = fb.readline()
		if not line:break
		
		begin=line.find("//log_base")
		if begin != -1:
			save=fb.tell()
			fb.seek(0)
			content=fb.read(save)
			fb.readline()
			rest=fb.read()
			break
with open(file="Param.h",mode="w") as fb:
	fb.write(content)
	fb.write("#define log_base {}\n".format(sys.argv[1]))
	fb.write(rest)