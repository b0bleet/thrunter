#ifndef KERNEL_H
#define KERNEL_H

#define SYSCALL_CREATE_USER_PROCESS 0xaa

#define PROCESS_HANDLE 0x3a0

#define KTHREAD 0x188
#define KTHREAD_SYSCALL_NUMBER 0x1f8

target_ulong curr_syscall_num(CPUState *cpu);
target_ulong get_new_procid(CPUState *cpu);

#endif
