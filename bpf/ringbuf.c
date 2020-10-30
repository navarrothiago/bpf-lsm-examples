// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020 Facebook

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 12);
} ringbuf SEC(".maps");

SEC("lsm/bprm_committed_creds")
int BPF_PROG(print_name, struct linux_binprm *bprm)
{
	//const char* name = bprm->file->f_path.dentry->d_name.name;
	//const char* name2 = bprm->filename;
	int argc = bprm->argc;

	//char hw[16] = "hello_world";

	bpf_ringbuf_output(&ringbuf, &argc, sizeof(int), 0);

	return 0;
}
