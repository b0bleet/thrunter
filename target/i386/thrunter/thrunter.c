#include "thrunter/thrunter.h"

#include "thrunter/kernel.h"

static GHashTable *processes = NULL;
static GMutex lock;

int trace_syscall(CPUState *cpu) {
    Process *proc, *curr_proc;

    g_mutex_lock(&lock);
    if (processes == NULL) {
        processes = g_hash_table_new(NULL, g_direct_equal);
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
        CPUX86State *env = cpu_env(cpu);
        printf("PROCESS %ld calls syscall %ld\n", curr_proc->pid, env->regs[R_EAX]);
    }

    g_free(curr_proc);

    g_mutex_unlock(&lock);

    return 0;
}

int hook_new_proc(CPUState *cpu) {
    Process *proc = NULL;
    
    g_mutex_lock(&lock);
    target_ulong new_pid = get_new_procid(cpu);
    proc = (Process*) g_hash_table_lookup(processes, GUINT_TO_POINTER(new_pid));
    if (proc) {
        printf("PROC %ld already exists and removed\n", new_pid);
        g_hash_table_remove(processes, GUINT_TO_POINTER(new_pid));
    }

    proc = g_new0(Process, 1);
    proc->pid = new_pid;

    g_hash_table_insert(processes, GUINT_TO_POINTER(new_pid), (gpointer) proc);
    
    g_mutex_unlock(&lock);

    return 0;
}
