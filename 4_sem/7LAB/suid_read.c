#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("\nFile content:\n");

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s\n", buffer);
    }

    if (ferror(file)) {
        fprintf(stderr, "\nRead error: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}
