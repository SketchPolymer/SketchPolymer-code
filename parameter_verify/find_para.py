import subprocess
import time
import requests
import sys
import os

#python run_0.py --mode find_best_mem --total_memory 5000 --hash_1 3 --hash_2 6 --hash_3 3 --hash_4 4
#python run_0.py --mode find_best_hash --total_memory 10000 --mem_1 0.05 --mem_2 0.2 --mem_3 0.2 --mem_4 0.55
#python run_0.py --mode verify_mem_4 --total_memory 5000
#python run_0.py --mode verify_hash_3 --total_memory 5000
#python run_0.py --mode verify_log_base --total_memory 1
#python run_0.py --mode verify_threadsold --total_memory 1
#python run_0.py --mode verify_w --total_memory 1

os.chdir('./SketchPolymer_1')
thread=[1,2,3,4,5,6,7,8]


for memory in range(4000,8000+1,2000):
    for i in range(0,3+1):
        os.chdir('../SketchPolymer_{}'.format(i))
        command = "python run_{}.py --mode verify_hash_{} --total_memory {}".format(i, i+1, memory)
        print(command)
        thread[i]=subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    for i in range(0,3+1):
        thread[i].wait()
    for i in range(0,3+1):
        print(thread[i].stdout.readlines()) 

for memory in range(4000,8000+1,2000):
    for i in range(4,7+1):
        os.chdir('../SketchPolymer_{}'.format(i))
        command = "python run_{}.py --mode verify_mem_{} --total_memory {}".format(i, i-3, memory)
        print(command)
        thread[i]=subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    for i in range(4,7+1):
        thread[i].wait()
    for i in range(4,7+1):
        print(thread[i].stdout.readlines()) 


for i in range(0,7+1):
    thread[i].wait()
for i in range(0,7+1):
    print(thread[i].stdout.readlines()) 


print('all mission completed')
