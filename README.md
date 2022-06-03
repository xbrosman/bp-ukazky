# Examples of communication between the kernel and the Linux application

To test applications and modules goto some directory and run load_script.sh to build kernel module, then run ./buildRun.sh to start aplication in user space.

## Module - build and install

```
    cd <working dir>
    ./load_script.sh    
```
## Test application - build and run

```
    cd <working dir>
    ./buildRun.sh
```

## Module - unload and clean

```
    cd <working dir>
    ./remove_script.sh
```