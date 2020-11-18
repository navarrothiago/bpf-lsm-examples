// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020 Facebook

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <errno.h>

#define PATHLEN 256
#define MAXFILESIZE 0x20000000
#define MAY_WRITE 0x00000002
#define MAY_READ 0x00000004

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 12);
} ringbuf SEC(".maps");

struct output {
	u32 pid;
	u32 read_write; // 0: read, 1: write
	u64 bytes;
	char buf[PATHLEN];
};

SEC("lsm/file_permission")
int BPF_PROG(trace_rw, struct file *file, int mask, int ret)
{
	if (ret)	return ret;

	struct output o;
	int len;
//	char comm[32];
	o.pid = bpf_get_current_pid_tgid();		
	o.bytes = file->f_inode->i_size;
	len = bpf_probe_read_str(o.buf, sizeof(o.buf), file->f_path.dentry->d_name.name);
	
//	bpf_get_current_comm(&comm, sizeof(comm));
//	
//	if ( !(comm[0]=='c' && comm[1]=='p' && comm[2]=='\0') )
//		return 0;
	if (o.bytes < 4096)
		return 0;
	
	if (mask == MAY_READ){
		o.read_write = 0;
		bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);
	} else if (mask == MAY_WRITE){
		o.read_write = 1;
		bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);
	}
	if (o.bytes > MAXFILESIZE ){
		return -EFBIG;
	}
	return 0;
}
