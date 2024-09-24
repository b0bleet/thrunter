#include "thrunter/kernel.h"
#include "thrunter/thrunter.h"

#include "exec/cpu_ldst.h"

inline target_ulong curr_syscall_num(CPUState *cpu)
{
    CPUX86State *env = cpu_env(cpu);
    hwaddr kthread = cpu_ldq_data(env, env->kernelgsbase + KTHREAD);
    target_ulong syscall_number =
        cpu_ldq_data(env, kthread + KTHREAD_SYSCALL_NUMBER);
    return syscall_number;
}

inline target_ulong get_new_procid(CPUState *cpu)
{
    CPUX86State *env = cpu_env(cpu);
    hwaddr rsp = env->regs[R_R8] + 0x8;
    target_ulong pid = cpu_ldq_data(env, rsp + PROCESS_HANDLE);
    target_ulong result = env->regs[R_EAX];
    if (!result) {
        return pid;
    }

    return 0;
}
