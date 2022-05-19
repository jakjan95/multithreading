## False sharing:

Problem occurs when we try to access separate data(which is on the same cache memory) on different cores.

The reason of false sharing is trying to modify separate data on separate cores(as we think) but unfortunately during modification of data on ones of core it blocks all cache line which cause separate data to not being available on other cores

## Solution

To fix this problem all of data which we want to acces on separate cores needs to be on separate cache line.  We can achieve that using **alignas** specifier.

## Examples

```c++
struct alignas(64) Value {
    std::atomic<int> val_ { 0 };
};
```

Now in structure **Value** variable val_ occupy only one cache line.

## Performance for file with false sharing problem

Performance for file [false.cpp](false.cpp) which have got false sharing problem

```
result = 199980000

 Performance counter stats for './false':

              6,62 msec task-clock                #    1,834 CPUs utilized          
                 3      context-switches          #  453,508 /sec                   
                 0      cpu-migrations            #    0,000 /sec                   
               139      page-faults               #   21,013 K/sec                  
        13 774 876      cycles                    #    2,082 GHz                    
        11 725 438      stalled-cycles-frontend   #   85,12% frontend cycles idle   
         4 989 197      instructions              #    0,36  insn per cycle         
                                                  #    2,35  stalled cycles per insn
           888 868      branches                  #  134,370 M/sec                  
            25 277      branch-misses             #    2,84% of all branches        

       0,003607452 seconds time elapsed
```

## Performance for file with fixed false sharing problem

Performance for file [fixed.cpp](fixed.cpp) where false sharing is fixed

```
result = 199980000

 Performance counter stats for './fixed':

              3,64 msec task-clock                #    1,135 CPUs utilized          
                 3      context-switches          #  824,389 /sec                   
                 0      cpu-migrations            #    0,000 /sec                   
               141      page-faults               #   38,746 K/sec                  
         5 564 296      cycles                    #    1,529 GHz                    
         3 475 717      stalled-cycles-frontend   #   62,46% frontend cycles idle   
         5 033 220      instructions              #    0,90  insn per cycle         
                                                  #    0,69  stalled cycles per insn
           897 547      branches                  #  246,643 M/sec                  
            25 572      branch-misses             #    2,85% of all branches        

       0,003205467 seconds time elapsed
```
