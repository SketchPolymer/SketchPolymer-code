import subprocess
import math
import time
import requests
import sys
import argparse

filename="result_{}.txt".format( str(time.strftime("%Y-%m-%d-%H:%M", time.localtime())) )

eps_list_caida = [0.1,0.1,0.09,0.068,0.05,0.04]

mem_list = [3000, 4000, 5000, 6000, 7000, 8000]
w_list = [0.5,0.6,0.7,0.8,0.9]

parser = argparse.ArgumentParser()
parser.add_argument('--dataset', '-d', help='the dataset you want to run on\n \
											c for caida, \
											s for seattle, \
											w for webget', required=True)
args = parser.parse_args()
eps_list = None
if args.dataset == 'c': k_list = eps_list_caida
elif args.dataset == 's': k_list = eps_list_seattle
elif args.dataset == 'w': k_list = eps_list_webget

with open(file=filename, mode="w") as fb:	
	for mem,eps in zip(mem_list, eps_list):
		fb.write("{}\n".format( str(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())) ))
		
		prefix = "(base) root@labserver0:~/hongyisen/ddsketch_all/squad# "
		command = "./main {MEM} {EPS} 0.95".format(MEM=mem, EPS=eps)
		s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
		fb.write(prefix + command + "\n")
		fb.write(s.stdout.decode("UTF-8"))
		fb.flush()