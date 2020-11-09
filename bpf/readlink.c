// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020 Facebook

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <errno.h>

#define PATHLEN 256

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 12);
} ringbuf SEC(".maps");

struct output {
	u32 pid;
	u32 tgid;
	char buf[PATHLEN];
};

SEC("lsm/inode_readlink")
int BPF_PROG(readlink_block, struct dentry *dentry, int ret)
{
	struct output o;
	int len;
	o.pid = bpf_get_current_pid_tgid();	
	o.tgid = bpf_get_current_pid_tgid() >> 32;
	
	len = bpf_probe_read_str(o.buf, sizeof(o.buf), dentry->d_parent->d_parent->d_parent->d_name.name);
	if (o.buf[0] == '/' && o.buf[1] == '\0'){
		len = bpf_probe_read_str(o.buf, sizeof(o.buf), dentry->d_parent->d_name.name);
		if ( o.buf[0] == 'n' && o.buf[1] == 's' && o.buf[2] =='\0' ){
			len = bpf_probe_read_str(o.buf, sizeof(o.buf), dentry->d_name.name);
			bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);
			return -EPERM; 
		}
	}

	return 0;
}
