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
	char buf[PATHLEN];
};

SEC("lsm/bprm_creds_from_file")
int BPF_PROG(print_name, struct linux_binprm *bprm, int ret)
{
	char bl[] = "/bin/ls";
	struct output o;
	int len;
	o.uid = bpf_get_current_uid_gid() >> 32;	
	
	len = bpf_probe_read_str(o.buf, sizeof(o.buf), bprm->filename);
	
	if (o.uid == 1001 && len >  7) {
		if (o.buf[0] == bl[0] && o.buf[1] == bl[1] && o.buf[2] == bl[2] && o.buf[3] == bl[3] && o.buf[4] == bl[4] && o.buf[5] == bl[5] && o.buf[6] == bl[6] && o.buf[7] == bl[7] ){
			bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);
			return -EPERM;
		}
	}
	
	bpf_ringbuf_output(&ringbuf, &o, sizeof(struct output), 0);

	return 0;
}
