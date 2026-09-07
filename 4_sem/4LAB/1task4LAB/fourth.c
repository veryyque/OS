#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

int global_not_inited_var;
int global_inited_var = 1;
const int global_const_var = 2;

void ChangeEnvVar() {
    int ret = 0;
    char *env_var_name = "ENV_VAR";
    char *env_var_value;
    char *new_env_var_value = "response";
    env_var_value = getenv(env_var_name);
    printf("env_var: %s\n", env_var_value);
    ret = setenv(env_var_name, new_env_var_value, true);
    if (ret == -1) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }
    env_var_value = getenv(env_var_name);
    printf("env_var: %s\n", env_var_value);
}

int main() {
    int local_var;
    static int static_var;
    const int local_const_var = 3;
    printf("pid: %d\n\n", getpid());
    printf("global_not_inited_var:  %p, %d\n", &global_not_inited_var, global_not_inited_var);
    printf("global_inited_var:      %p, %d\n", &global_inited_var, global_inited_var);
    printf("global_const_var:       %p, %d\n", &global_const_var, global_const_var);
    printf("local_var:              %p, %d\n", &local_var, local_var);
    printf("static_var:             %p, %d\n", &static_var, static_var);
    printf("local_const_var:        %p, %d\n", &local_const_var, local_const_var);

    ChangeEnvVar();
    return 0;
}