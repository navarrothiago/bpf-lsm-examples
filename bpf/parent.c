// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020 Facebook

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <errno.h>

#define PATHLEN 128

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 12);
} ringbuf SEC(".maps");

struct output {
	int pid;
	int parentpid;
	unsigned int inum;
	unsigned int level;
	int nr;
	char current[PATHLEN];
	char parent[PATHLEN];
};

SEC("lsm/task_setrlimit")
int BPF_PROG(print_parent, struct task_struct *p, unsigned int resource, struct rlimit *new_rlim, int ret)
{
	struct output o;
	int len;
	
	o.pid = p->pid;
	o.parentpid = p->real_parent->pid;	
	o.inum = p->nsproxy->mnt_ns->ns.inum;
	o.level = p->nsproxy->pid_ns_for_children->level;
	o.nr = p->thread_pid->numbers[0].nr;

	len = bpf_probe_read_str(o.current, sizeof(o.current), p->comm);
	len = bpf_probe_read_str(o.parent, sizeof(o.parent), p->real_parent->comm);
	
	bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);

	return 0;
}
