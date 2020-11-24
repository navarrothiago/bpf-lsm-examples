### Detection/Mitigation of the *subject*'s *operation* on the *object*.

- *subject* : The process who performs operation on the object
	- Available data: Every data in the struct task of the process.
	- Method: BPF helper (e.g) bpf_get_current_comm(), bpf_get_current_task()). 

- *object*: Each object has corresponding operations. Most of the object structure's members are accessible.
	- Inode/File: 
		- `inode_create` : creating a new file
		- `inode_link` : creating a new hard link
		- `inode_unlink` : removing a hard link
		- `inode_symlink` : creating a new symbolic link
		- `inode_mkdir` : creating a new directory
		- `inode_rmdir` : removing a directory
		- `inode_mknod` : creating a new special file (such as socket) via mknod() syscall
		- `inode_rename` : renaming a file or a directory
		- `path_chmod` : changing file's mode.
		- `path_chown` : changing the owner/group of the file.
		- `inode_readlink` : read a symbolic link
		- `inode_permission` : checked when a file is opened
		- `inode_setattr` : setting attributes
		- `inode_getattr` : obtaining attributes
		- `file_permission` : reading/writing a file
		- `file_ioctl` : ioctl syscall
		- `mmap_addr`, `mmap_file` : mmap operation
		- `file_mprotect` : changing memory access permission
		- `file_fcntl` : fcntl syscall. 

	- Socket:
		- `socket_create`
		- `socket_socketpair`
		- `socket_bind`
		- `socket_connect`
		- `socket_listen`
		- `socket_accept`
		- `socket_sendmsg`
		- `socket_recvmsg`
		- `socket_getsockopt`
		- `socket_setsockopt`
		- `socket_sock_shutdown`
		- `socket_sock_rcv_skb` : checking permission on incoming network packets.  

	- Filesystem:
		- `sb_statfs`
		- `sb_mount`
		- `sb_remount`
		- `sb_umount`
		- `sb_pivotroot`

	- Binary: 
		-  Execution
	- Task:
		- `task_get_uid/gid/pgid/sid/secid`
		- `task_set_uid/gid`
		- `task_get_nice/ioprio`
		- `task_set_nice/ioprio`
		- `task_get_rlimit`
		- `task_set_rlimit`
		- `task_kill` : checking permission before sending signal to another process.
		- `task_prctl` : prctl syscall

	- Others - bpf-related, ptrace, xfrm, ...
