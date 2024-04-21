#include "thrunter/thrunter.h"

int run_thrunter(CPUState *cpu) {
    Process *proc = init_process(cpu);
    if (proc == NULL) {
        fprintf(stderr, "unable to prepare TEB\n");
        return -1;
    }
    g_free(proc);

    return 0;
}
