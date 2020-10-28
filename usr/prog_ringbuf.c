// SPDX-License-Identifier: GPL-2.0
#define _GNU_SOURCE
#include <linux/compiler.h>
#include <asm/barrier.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <linux/perf_event.h>
#include <linux/ring_buffer.h>
#include "ringbuf.skel.h"


static int process_sample(void *ctx, void *data, size_t len)
{
	int *rcv = data;
	fprintf(stdout, "received data: %d\n", *rcv);
	return 0;
}

static struct ringbuf *skel;
static struct ring_buffer *ringbuf;

int main()
{
	int err;

	skel = ringbuf__open_and_load();
	if (!skel)
	{
		fprintf(stdout, "skeleton open&load failed\n");
		return 0;
	}

	ringbuf = ring_buffer__new(bpf_map__fd(skel->maps.ringbuf), process_sample, NULL, NULL);
	
	if (!ringbuf){
		fprintf(stdout, "failed to create ringbuf\n");
		goto cleanup;
	}

	err = ringbuf__attach(skel);
	if (err){
		fprintf(stdout, "skeleton attachment failed: %d\n", err);
		goto cleanup;
	}

	/* poll for samples */
	err = ring_buffer__poll(ringbuf, -1);
	
	if (err < 0){
		fprintf(stdout, "failed to poll data from ringbuf: err %d\n", err); 
		goto cleanup;
	}
cleanup:
	ring_buffer__free(ringbuf);
	ringbuf__destroy(skel);
	return 0;
}
