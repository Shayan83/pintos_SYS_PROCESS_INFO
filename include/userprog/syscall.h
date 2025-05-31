#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

int sys_process_info(int pid, int info_type, void *out_buffer, unsigned buffer_size);

#endif /* userprog/syscall.h */
