# SketchPolymer

This repository contains the C++ implementation of SketchPolymer, a data structure for per-flow tail latency estimation. We compare different aspects of performance of SketchPolymer and other quantiles sketches. 



## Project Structure

```
├─baseline------------source code of baseline
│  
├─dataset-------------contains different datasets to test SketchPolymer
│  
├─dd------------------source code of DDSketch
│  
├─gk------------------source code of Greenwald Khanna (GK) sketch
│ 
├─kll-----------------source code of KLL Sketch
│  
├─parameter_verify----To find the best parameters of SketchPolymer 
│  
├─SketchPolymer-------source code of SketchPolymer
│  
├─FPGA_distri_sketch_anonymously.zip----FPGA implementation of SketchPolymer
│  
├─SketchPolymer.p4----P4 implementation of SketchPolymer
```



## Installation

We recommend you to run this project on Linux platfrom. 

cd each directory(except parameter_verify) and use cmake(version >= 3.16.3) and make(version>= 4.2.1) to build the executable file.

For comparison between SketchPolymer and other data structures:

```
#build baseline and SketchPolymer
cd ./baseline
cmake.
make
cd ../SketchPolymer
cmake .
make
```

To find best parameters of SketchPolymer: copy ./SketchPolymer to parameter_verify and rename it as SketchPolymer_i (i=0,1,...,7). Then, build  executable file for SketchPolymer_i.



## Run

Download datasets and move them to ./dataset. If you want to use Seattle or Webget, remember use the python program in ./dataset to generate the dataset that our benchmark can read.

In SketchPolymer/Param.h, you can change the parameters of SketchPolymer. 

```
An example
log_base=1.5 --- log base = 1.5
h1=3 ---  stage 1 will use 3 hash functions
mem3=0.8 ---  stage 3 will use 80% of memory that assign to SketchPolymer
```

For baseline / DDSketch / SketchPolymer, if you want to assign the total memory as 8000:

```
./main 8000
```

For Greenwald Khanna (GK) sketch, f you want to assign the total memory as 8000 and epsilon as 0.01:

```
./main 8000 0.01
```

For KLL Sketch sketch, f you want to assign the total memory as 8000 and K as 200:

```
./main 8000 200
```

To find best parameters of SketchPolymer: use find_para.py in ./parameter_verify, this program will call subprocess to run run_i.py in ./parameter_verify/SketchPolymer_i. 



## Reference

[1] GK Sketch: https://github.com/sengelha/streaming-percentiles

[2] Charles Masson, Jee E. Rim and Homin K. Lee. DDSketch: A Fast and Fully-Mergeable Quantile Sketch with Relative-Error Guarantees. 2019.

[3] KLL Sketch: https://github.com/apache/datasketches-cpp

[4] CAIDA dataset: https://catalog.caida.org/search?query=types=dataset%20links=tag:caida%20

[5] Seattle dataset: https://github.com/uofa-rzhu3/NetLatency-Data

[6] Webget dataset: https://zenodo.org/record/2547512
