#include "thrunter/thrunter.h"

#include "exec/cpu_ldst.h"

static inline int init_peb(CPUState *cpu, PEB *peb) {
    CPUX86State *env = cpu_env(cpu);
    hwaddr teb = cpu_ldq_data(env, env->segs[R_GS].base + 0x30);
    hwaddr peb_base = cpu_ldq_data(env, teb + 0x60);
    return cpu_memory_rw_debug(cpu, peb_base, peb, sizeof(PEB), 0);
}

static inline int init_teb(CPUState *cpu, TEB *teb) {
    CPUX86State *env = cpu_env(cpu);
    hwaddr teb_base = cpu_ldq_data(env, env->segs[R_GS].base + 0x30);
    return cpu_memory_rw_debug(cpu, teb_base, teb, sizeof(TEB), 0);
}

static int init_process_info(CPUState *cpu, Process *proc) {
    int ret;
    PEB peb;

    ret = init_peb(cpu, &peb);
    if (ret) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to prepare PEB\n");
#endif
        return ret;
    }

    RTL_USER_PROCESS_PARAMETERS user_proc_params;
    if(cpu_memory_rw_debug(cpu,
                        (hwaddr)peb.ProcessParameters,
                        &user_proc_params,
                        sizeof(RTL_USER_PROCESS_PARAMETERS),
                        0)) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to retrieve process parameters\n");
#endif
        return -1;
    }

    uint16_t path_name_len = user_proc_params.ImagePathName.MaximumLength;
    uint8_t *path_name = g_malloc(path_name_len);
    if(cpu_memory_rw_debug(cpu,
                        (hwaddr)user_proc_params.ImagePathName.Buffer,
                        path_name,
                        path_name_len,
                        0)) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to retrieve executable path name\n");
#endif
        ret = -1;
        goto err;
    }

err:
    g_free(path_name);

    return ret;
}

Process *get_curr_proc(CPUState *cpu) {
    int ret;
    Process *proc;
    TEB teb;

    proc = g_malloc0(sizeof(Process));

    ret = init_teb(cpu, &teb);
    if (ret) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to prepare TEB\n");
#endif
        g_free(proc);
        return NULL;
    }
    proc->pid = (uintptr_t)teb.ClientId.UniqueProcess;

    ret = init_process_info(cpu, proc);
    if (ret < 0) {
#ifdef THRUNTER_DEBUG
        fprintf(stderr, "Unable to prepare process information\n");
#endif
        g_free(proc);
        return NULL;
    }

    return proc;
}
