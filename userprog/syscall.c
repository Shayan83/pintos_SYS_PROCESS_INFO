#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include <string.h>
#include <userprog/process.h>

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void
syscall_handler (struct intr_frame *f) {
	uint64_t syscall_num = f->R.rax;
	int ret = 0;
	switch (syscall_num) {
		case SYS_PROCESS_INFO: {
			int pid = (int)f->R.rdi;
			int info_type = (int)f->R.rsi;
			void *out_buffer = (void *)f->R.rdx;
			unsigned buffer_size = (unsigned)f->R.r10;
			ret = sys_process_info_impl(pid, info_type, out_buffer, buffer_size);
			f->R.rax = ret;
			break;
		}
		default:
			printf ("Unknown system call!\n");
			thread_exit();
	}
}

// Implementation of sys_process_info
static int sys_process_info_impl(int pid, int info_type, void *out_buffer, unsigned buffer_size) {
	if (!out_buffer || buffer_size < sizeof(struct process_cpu_info))
		return -1; // EFAULT
	struct process_cpu_info info;
	// For now, just return dummy values
	info.utime = 42;
	info.stime = 24;
	// Copy to user buffer (in real Pintos, use copy_out or similar)
	memcpy(out_buffer, &info, sizeof(info));
	return 0;
}
