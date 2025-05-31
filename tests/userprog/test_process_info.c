#include <stdio.h>
#include <syscall.h>
#include <user/syscall.h>
#include <threads/thread.h>

int main(void) {
    struct process_cpu_info info;
    int ret = sys_process_info(1, 1, &info, sizeof(info)); // PID 1, info_type 1
    if (ret == 0) {
        printf("CPU times: user=%lu, kernel=%lu\n", info.utime, info.stime);
    } else {
        printf("Failed to get process info, error=%d\n", ret);
    }
    return 0;
} 