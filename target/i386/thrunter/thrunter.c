#include "thrunter/thrunter.h"

#include "thrunter/kernel.h"

#include "exec/cpu_ldst.h"

static GHashTable *processes = NULL;
static GMutex lock;

int trace_syscall(CPUState *cpu) {
    Process *proc, *curr_proc;

    g_mutex_lock(&lock);
    if (processes == NULL) {
        processes = g_hash_table_new(NULL, g_direct_equal);
    }

    CPUX86State *env = cpu_env(cpu);
    target_ulong cpl = env->segs[R_CS].selector & 0x3;
    if (cpl != 0x3) {
        return -1;
    }

    curr_proc = get_curr_proc(cpu);
    if (curr_proc == NULL) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to initialize process\n");
#endif
        return -1;
    }

    proc = (Process*) g_hash_table_lookup(processes, GUINT_TO_POINTER(curr_proc->pid));
    if (proc) {
#if 0
        CPUX86State *env = cpu_env(cpu);
        printf("PROC: %"PRIu64" SYSCALL: %"PRIu64"\n", curr_proc->pid, env->regs[R_EAX]);
#endif
    }

    g_free(curr_proc);

    g_mutex_unlock(&lock);

    return 0;
}

int hook_new_proc(CPUState *cpu) {
    Process *proc = NULL, *syscall_proc = NULL;

    syscall_proc = get_curr_proc(cpu);
    if (syscall_proc == NULL) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to initialize syscall executor process\n");
#endif
        return -1;
    }
    
    g_mutex_lock(&lock);
    target_ulong new_pid = get_new_procid(cpu);
    proc = (Process*) g_hash_table_lookup(processes, GUINT_TO_POINTER(new_pid));
    if (proc) {
        printf("PROC: %"PRIu64" already exists and removed\n", new_pid);
        g_hash_table_remove(processes, GUINT_TO_POINTER(new_pid));
    }

    proc = g_new0(Process, 1);
    proc->pid = new_pid;
    proc->parent_pid = syscall_proc->pid;

    g_hash_table_insert(processes, GUINT_TO_POINTER(new_pid), (gpointer) proc);
    
    g_mutex_unlock(&lock);

    return 0;
}
