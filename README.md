## Demo LSM + BPF programs

### Requirements

**Dependent Packages**    
- for libbpf: zlib and libelf  
- for BTF: dwarves(pahole), bpftool

Programs might be compatible with pahole version 1.16+. (not tested)

Tested with:  
- zlib 1.2.11  
- libelf 0.176   
- pahole 1.18  
- bpftool 5.8.13  

Build kernel version 5.8.13 with enabling below options in the configuration.  

```
CONFIG_DEBUG_INFO_BTF=y
CONFIG_BPF_LSM=y
CONFIG_LSM="[other LSMs],bpf"
```
Programs might be compatible with kernel version 5.8+. (not tested)  


### Current programs

`bpf/uid.c` / `usr/prog_uid.c` : prevent uid 1001 from ls execution.  
`bpf/owner.c` / `usr/prog_owner.c` : prevent users from execution of other users' binary files. (based on an owner UID of a binary file inode.)  
`bpf/parent.c` / `usr/prog_parent.c` : print the information of process that calls setrlimit(). (pid, process name, parent pid, parent process name, namespace information)  
`bpf/readlink.c` / `usr/prog_readlink.c` : prevent every users from reading symbolic link in `/proc/[pid]/ns/` directory.

### TODO
- Precise specification of dependent packages
- Add more programs that could be useful.
