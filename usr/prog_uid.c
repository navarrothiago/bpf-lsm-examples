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
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/perf_event.h>
#include <linux/ring_buffer.h>
#include "uid.skel.h"

#define PATHLEN 256

static int freed = 0;

struct output {
	u32 uid;
	char buf[PATHLEN];
};

void sig_handler (int signo);

static int process_sample(void *ctx, void *data, size_t len)
{
	struct output *rcv = data;
	fprintf(stdout, "%s - uid: %u\n", rcv->buf, rcv->uid);
	return 0;
}


static struct uid *skel;
static struct ring_buffer *ringbuf;

int main()
{
	int err;
	struct rlimit rlim_new = {
		.rlim_cur = RLIM_INFINITY,
		.rlim_max = RLIM_INFINITY,
	};
	
	setrlimit(RLIMIT_MEMLOCK, &rlim_new);
	signal (SIGINT, (void *)sig_handler);

	skel = uid__open_and_load();
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

	err = uid__attach(skel);
	if (err){
		fprintf(stdout, "skeleton attachment failed: %d\n", err);
		goto cleanup;
	}

	/* poll for samples */
	while (1){
		err = ring_buffer__poll(ringbuf, -1);
	
		if (err < 0){
			fprintf(stdout, "failed to poll data from ringbuf: err %d\n", err); 
			goto cleanup;
		}
	}
cleanup:
	if (freed == 0) {
		ring_buffer__free(ringbuf);
		uid__destroy(skel);
	}
	return 0;
}

void sig_handler(int signo){
	ring_buffer__free(ringbuf);
	uid__destroy(skel);
	freed = 1;
}
