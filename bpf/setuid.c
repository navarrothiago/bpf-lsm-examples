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
	u32 old_uid;
	u32 new_uid;
};

SEC("lsm/task_fix_setuid")
int BPF_PROG(trace_rw, struct cred *new, const struct cred *old, int flags, int ret)
{
	if (ret)	return ret;

	struct output o;
	int len;
	
	o.pid = bpf_get_current_pid_tgid();	
	o.old_uid = old->uid.val;
	o.new_uid = new->uid.val;

	bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);

	if ( (o.old_uid != 0) && (o.old_uid != o.new_uid))
		return -EPERM;
	
	return 0;
}
