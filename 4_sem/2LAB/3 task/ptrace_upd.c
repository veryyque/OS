#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <errno.h>
#include <libaudit.h>

const char* get_syscall_name_audit(int syscall_num) {
    int machine = audit_elf_to_machine(AUDIT_ARCH_X86_64);
    const char* name = audit_syscall_to_name(syscall_num, machine);
    if (!name) {
        return "unknown";
    }
    return name;
}

void trace_syscalls(pid_t child_pid) {
    int status;
    int in_syscall = 0;
    struct user_regs_struct regs;

    waitpid(child_pid, &status, 0);
    ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACESYSGOOD); //установка опция для различия SIGTRAP 

    while (1) {
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0); //запуск процесса с остановкой перед входом в сист вызов и после выхода
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            break;
        }
        if (WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80)) { //остановлен на syscall (133 & 128 = 1)
            ptrace(PTRACE_GETREGS, child_pid, NULL, &regs); //чтение регистров
            if (in_syscall == 0) { //вход
                printf("\n[Syscall Enter] %s (%lld)(", get_syscall_name_audit(regs.orig_rax), regs.orig_rax);
                printf("rdi: 0x%llx, rsi: 0x%llx, rdx: 0x%llx, r10: 0x%llx, r8: 0x%llx, r9: 0x%llx)\n",regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9);
                in_syscall = 1;
            } else { //выход
                printf("[Syscall Exit] %s return: 0x%llx (%lld)\n", get_syscall_name_audit(regs.orig_rax), regs.rax, regs.rax);
                in_syscall = 0;
            }
        }
    }
}

int main(void) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL); //дочерний будет отслеживаться родительским
        raise(SIGSTOP); //останавливаю чтобы родитель выполнялся
        execl("/bin/ls", "ls", NULL);  //чтобы заменить текущий процесс на /bin/ls.
        perror("execl");
        exit(1);
    } else {
        trace_syscalls(child_pid);
    }
    return 0;
}