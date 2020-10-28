## Demo LSM + BPF programs

### Requirements

Build kernel version 5.8.13 with enabling below options in the configuration.  

```
CONFIG_DEBUG_INFO_BTF=y
CONFIG_BPF_LSM=y
CONFIG_LSM="<other LSMs>,bpf"
```
Programs might be compatible with kernel version 5.8+. (not tested)  

### Current programs

`bpf/uid.c` / `usr/prog_uid.c` : prevent uid 1001 from ls execution.  
`bpf/owner.c` / `usr/prog_owner.c` : prevent users from execution of other users' binary files. (based on an owner UID of a binary file inode.) 

### TODO

- Enable programs to be built out of the kernel source tree.
- Add more programs that could be useful.
