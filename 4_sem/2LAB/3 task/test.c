#include <elf.h> 
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pid = fork();
    int status;
    int count = 0;
    
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execl("./hello_asm", "./hello_asm", NULL);
    } else {
        wait(&status);
        
        while (!WIFEXITED(status)) {
            count++;
            
            struct user_regs_struct regs;
            struct iovec iov = {
                .iov_base = &regs,
                .iov_len = sizeof(regs)
            };
            
            if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) == 0) {
           
                long syscall_num = regs.regs[8];
                
                char cmd[100];
                snprintf(cmd, sizeof(cmd), "ausyscall %ld 2>/dev/null", syscall_num);
                FILE *fp = popen(cmd, "r");
                
                if (fp) {
                    char name[50];
                    if (fgets(name, sizeof(name), fp)) {
                     
                        name[strcspn(name, "\n")] = 0;
                        printf("syscall #%d: %ld (%s)\n", count, syscall_num, name);
                    } else {
                        printf("syscall #%d: %ld (unknown)\n", count, syscall_num);
                    }
                    pclose(fp);
                } else {
                    printf("syscall #%d: %ld\n", count, syscall_num);
                }
            } else {
                printf("syscall #%d\n", count);
            }
            
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
            wait(&status);
        }
        
        printf("Всего системных вызовов: %d\n", count);
    }
    return 0;
}
