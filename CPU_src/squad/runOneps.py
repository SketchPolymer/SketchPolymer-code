#!/usr/bin/python
import subprocess
import random
import math
import re
import cPickle
import sys
import os
import time 
from datetime import datetime



if (sys.argv[1] == "SquadRandom"):
        fn = "./squad_random"
        filestr = "squad_random"
if (sys.argv[1] == "Quasi"):
        fn = "./quasi"
        filestr = "quasi"

if (sys.argv[1] == "SquadGk"):
        fn = "./squad_gk"
        filestr = "squad_gk"

if (sys.argv[1] == "Square"):
        fn = "./square"
        filestr = "square"

#############################################################################
#
#           Run On epsilon
#
#############################################################################

epsilons_perf = [0.001, 0.0025, 0.005, 0.006, 0.008] 
#epsilons_50 = [0.01, 0.025, 0.05, 0.1, 0.5, 0.8]
epsilons_50 = [0.1, 0.2, 0.3, 0.4, 0.5]

epsilons_90 = [0.001, 0.0025, 0.005, 0.01, 0.05, 0.08]
#epsilons_90 = [0.005, 0.1, 0.15, 0.2]
epsilons_90 = [0.005, 0.007, 0.01, 0.03, 0.05, 0.08]

epsilons_99 = [0.001, 0.0025, 0.005, 0.006, 0.008]

if (sys.argv[2] == "50"):
        epsilons = epsilons_50
        filestr = filestr + "_50_"

if (sys.argv[2] == "90"):
        epsilons = epsilons_90
        filestr = filestr + "_90_"
if (sys.argv[2] == "99"):
        epsilons = epsilons_99
        filestr = filestr + "_99_"

if (sys.argv[2] == "perf"):
        epsilons = epsilons_perf
        filestr = filestr + "_perf_"


i = datetime.now()

fileName= filestr + sys.argv[4] + "_" + i.strftime('%d-%m-%Y=%H-%M')+".txt"
#fileName=filestr+ "e="+str(e)+"_"+i.strftime('%d-%m-%Y=%H-%M')+".txt"

f = open(fileName, "w+")

RUNS = int(sys.argv[3])


if (sys.argv[4] == "DCTCP"):
        data_set = "dctcp_parsed_dataset.txt"
        #theta = 0.000712
        theta = 0.01

if (sys.argv[4] == "FB"):
        data_set = "fb_parsed_dataset_new.txt"
        theta = 0.01

if (sys.argv[4] == "Hadoop"):
        data_set = "parsed_dataset.txt"
        theta = 0.01

for e in epsilons:
        for run in xrange(RUNS):
                timing_list = []


                command = [fn]
                command += [data_set, str(theta), str(e), sys.argv[2]]
                print " ".join(command)

                print command

                out = subprocess.check_output(command)

                f.write(out)
                print out
