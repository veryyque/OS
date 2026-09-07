#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

void ReverseString(char *string) {
    for (int i = 0, j = strlen(string)-1; i < j; i++, j--) {
        char temp = string[i];
        string[i] = string[j];
        string[j] = temp;
    }
}

void ReverseFileContent(const char *src, const char *dst) {
    int src_fd = open(src, O_RDONLY);
    if (src_fd == -1) return;
    
    int dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        close(src_fd);
        return;
    }
    
    off_t size = lseek(src_fd, 0, SEEK_END);
    char buf[1024];
    
    for (off_t pos = size; pos > 0; ) {
        off_t chunk = (pos > sizeof(buf)) ? sizeof(buf) : pos;
        pos -= chunk;
        
        lseek(src_fd, pos, SEEK_SET);
        ssize_t n = read(src_fd, buf, chunk);
        if (n <= 0) break;
        
        // Переворачиваем буфер
        for (int i = 0; i < n/2; i++) {
            char t = buf[i];
            buf[i] = buf[n-1-i];
            buf[n-1-i] = t;
        }
        
        write(dst_fd, buf, n);
    }
    
    close(src_fd);
    close(dst_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <каталог>\n", argv[0]);
        return 1;
    }
    
    char *path_copy = strdup(argv[1]);
    char *parent = dirname(path_copy);
    
    char *name_copy = strdup(argv[1]);
    char *dir_name = basename(name_copy);
    
    //переворачиваю имя каталога (не всего пути)
    char rev_dir_name[256];
    strcpy(rev_dir_name, dir_name);
    ReverseString(rev_dir_name);
    
    //строка формирует путь к файлу
    char dst_dir[512];
    snprintf(dst_dir, sizeof(dst_dir), "%s/%s", parent, rev_dir_name);
    
    
    mkdir(dst_dir, 0755); //rwxr-xr-x
    printf("Создан каталог: %s\n", dst_dir);
    
    free(path_copy);
    free(name_copy);
    
    //открыв исходный каталог - DIR внутренняя структура хранения каталогов
    DIR *dir = opendir(argv[1]);
    if (!dir) {
        perror("opendir");
        return 1;
    }
    
    struct dirent *entry; //структура предств одну запись в каталоге
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;
        
        char src_path[512];
        char dst_path[512];
        char rev_name[256];
        
        // Путь к исходному файлу
        snprintf(src_path, sizeof(src_path), "%s/%s", argv[1], entry->d_name);
        
        // Переворачиваем имя файла
        strcpy(rev_name, entry->d_name);
        ReverseString(rev_name);
        
        // Путь к целевому файлу
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, rev_name);
        
        // Копируем
        ReverseFileContent(src_path, dst_path);
        printf("%s -> %s\n", entry->d_name, rev_name);
        count++;
    }
    
    closedir(dir);
    printf("Скопировано файлов: %d\n", count);
    printf("Целевой каталог: %s\n", dst_dir);
    
    return 0;
}
