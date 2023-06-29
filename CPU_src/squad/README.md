same as other directory, run benchmark and get the result

SQUAD - Heavy Hitters Quantile Estimation
===================
SQUAD is a method that combines sampling and sketching while improving the asymptotic space complexity. Intuitively, SQUAD uses a background sampling process to capture the behaviour of the quantiles of an item before it is allocated with a sketch, thereby allowing us to use fewer samples and sketches.


Getting Started
--------------------------------
To compile Sampling algorithm type:
```
make square
```
To compile SSGK algorithm type:
```
make quasi
```
To compile SQUAD algorithm type implemented with GK as quanile sketch:
```
make squad_gk
```
To compile SQUAD algorithm type implemented with Random as quanile sketch:
```
make squad_random
```

#### Arguments used by Sampling, SSGK and SQUAD:
 - **file name** - Path to dataset file.
 - **theta** - The heavy hitters threshold.
 - **epsilon** - An estimate accuracy parameter.
 - **quantile** - The quantile q i.e. the qth largest value

