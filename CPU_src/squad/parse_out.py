#!/usr/bin/python
import subprocess
import matplotlib as mpl
from matplotlib import pyplot as plt
from matplotlib import ticker
import numpy as np
import re
import sys



csvFiles = dict()
csvFiles['FB'] =sys.argv[1]
speeds = dict()


lines = open(csvFiles['FB']).readlines()
for line in lines:
    if(re.search("FlowId=(\d+)\s", line)) :
        flowid = re.search("FlowId=(\d+)\s", line).groups()[0]
        latency = re.search("FlowId=(\d+) \+(\d+\.\d+)ns", line).groups()[1]
        print(flowid, latency)
