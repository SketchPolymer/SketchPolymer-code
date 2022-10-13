import subprocess
import math
import time
import requests
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--mode", required=True)
parser.add_argument("--total_memory", required=True)
for i in range(1,4+1): parser.add_argument("--mem_{}".format(str(i)), default='0.5')
for i in range(1,4+1): parser.add_argument("--hash_{}".format(str(i)), default='-1')
args = parser.parse_args()
print(args)

para=[3, 5, 5, 3, 5, 20, 60, 15]
filename="result_{}.txt".format( str(time.strftime("%Y-%m-%d-%H:%M", time.localtime())) )

with open(file=filename, mode="w") as fb:
	
	if args.mode=='find_best_mem':
		for para[4] in range(5,10+1,5):#
			para[4] /= 100
			for para[5] in range(5,20+1,5):
				para[5] /= 100
				for para[6] in range(5,20+5,1):
					para[6] /= 100
					for para[7] in range(50,90+1,5):
						para[7] /= 100
	
						if math.isclose(para[4]+para[5]+para[6]+para[7], 1.00, rel_tol=1e-5):
	
							fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))#write time
							
							command="python changePara.py {} {} {} {} {} {} {} {} 50".format(\
								args.hash_1, args.hash_2, args.hash_3, args.hash_4,\
								str(para[4]), str(para[5]), str(para[6]), str(para[7]) )
							s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)#change parameter
							fb.write(s.stdout.decode("UTF-8"))
							
							s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)#compile
							
							s=subprocess.run("./main {}".format(args.total_memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)#run
							fb.write(s.stdout.decode("UTF-8"))
	
							fb.flush()
	
	if args.mode=='find_best_hash':
		for para[0] in range(3,3+1,1):#cm
			for para[1] in range(2,6+1,1):#cm
				for para[2] in range(3,3+1,1):#cm
					for para[3] in range(2,6+1,1):#bloom
	
						fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))#write time
						
						command="python changePara.py {} {} {} {} {} {} {} {} 50".format(\
							str(para[0]), str(para[1]), str(para[2]), str(para[3]), \
							args.mem_1, args.mem_2, args.mem_3, args.mem_4)
						s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)#change parameter
						fb.write(s.stdout.decode("UTF-8"))
						
						s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)#compile
							
						s=subprocess.run("./main {}".format(args.total_memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
						
						fb.write(s.stdout.decode("UTF-8"))
	
						fb.flush()

	if 'verify_hash' in args.mode:# i in [1,2,3,4]
		target = int(args.mode[-1])
		for para[target-1] in range(1,6+1):#cm
			fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
			
			sub_command = f"{para[0]} {para[1]} {para[2]} {para[3]}"
			command = "python changePara.py {} 0.05 0.20 0.60 0.15 50".format(sub_command)
			print(command)
			s = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			fb.write(s.stdout.decode("UTF-8"))
			
			s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
				
			s=subprocess.run("./main {}".format(args.total_memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

			fb.write(s.stdout.decode("UTF-8"))
			fb.flush()
			
	if 'verify_mem' in args.mode:# i in [1,2,3,4]
		target = int(args.mode[-1])
		
		for para[target+3] in range(5,90+1,5):#cm
			fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
			
			temp = para[4:7+1]
			print(temp)
			rest = sum(temp)-temp[target-1]
			for i in range(0,3+1):
				if i == target-1: continue
				else: temp[i] = (100-temp[target-1]) * (temp[i]/rest)#rest is divided in the original proportion to the remaining three parameters
			for i in range(0,3+1): temp[i] = round(temp[i]/100,3)

			sub_command = f"{temp[0]} {temp[1]} {temp[2]} {temp[3]}"
			command = "python changePara.py 3 5 5 3 {} 50".format(sub_command)	
			s = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			fb.write(s.stdout.decode("UTF-8"))
			
			s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
				
			s=subprocess.run("./main {}".format(args.total_memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

			fb.write(s.stdout.decode("UTF-8"))
			fb.flush()
	
	if args.mode=='verify_log_base':#remember to change log_base in para.h
		for memory in range(3000,8000+1,1000):#cm
		
			fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
			command="python changePara.py 3 5 5 3 0.05 0.2 0.60 0.15 50"
			s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			fb.write(s.stdout.decode("UTF-8"))
			
			s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
				
			s=subprocess.run("./main {}".format(memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

			fb.write(s.stdout.decode("UTF-8"))
		
			fb.flush()

	if args.mode=='verify_threadsold':
		for threadsold in [50, 100, 250, 500]:
			for memory in range(3000,8000+1,1000):#cm

				fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
				command="python changePara.py 3 5 5 3 0.05 0.2 0.60 0.15 {}".format(threadsold)
				s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
				fb.write(s.stdout.decode("UTF-8"))
				
				s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
					
				s=subprocess.run("./main {}".format(memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	
				fb.write(s.stdout.decode("UTF-8"))
			
				fb.flush()

	if args.mode=='verify_w':# change query_w in benchmark.h
		for memory in range(3000,8000+1,1000):#cm
			
			fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
			command="python changePara.py 3 5 5 3 0.05 0.2 0.60 0.15 50"
			s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			fb.write(s.stdout.decode("UTF-8"))
			
			s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
				
			s=subprocess.run("./main {}".format(memory), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

			fb.write(s.stdout.decode("UTF-8"))
		
			fb.flush()

	fb.write("mission finished\n")
	