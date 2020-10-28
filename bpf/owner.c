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
	u32 uid;
	u32 owneruid;
	char buf[PATHLEN];
};


SEC("lsm/bprm_creds_from_file")
int BPF_PROG(owner, struct linux_binprm *bprm, int ret)
{
	int len;
	struct output o;

	o.uid = bpf_get_current_uid_gid() >> 32;
	o.owneruid = bprm->file->f_inode->i_uid.val;
	
	len = bpf_probe_read_str(o.buf, sizeof(o.buf), bprm->filename);
	
	if (o.owneruid != 0 && o.owneruid != o.uid) {
		bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);
		return -EPERM;
	}
	
	bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);

	return 0;
}
