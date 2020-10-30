// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020 Facebook

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

#define PATHLEN 256

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 12);
} ringbuf SEC(".maps");

SEC("lsm/bprm_committed_creds")
int BPF_PROG(print_name, struct linux_binprm *bprm)
{
	int len;
	char buf[PATHLEN];

	len = bpf_probe_read_str(buf, sizeof(buf), bprm->filename);

	if (len > 0)
		bpf_ringbuf_output(&ringbuf, buf, len, 0);
	return 0;
}
