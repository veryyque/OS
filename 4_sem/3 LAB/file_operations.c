#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void CreateDirectory(const char *path) { 
    if (mkdir(path, 0755) == -1) { //rwxr-xr-x
        perror("Ошибка при создании каталога");
    } else {
        printf("Каталог '%s' успешно создан.\n", path);
    }
}

void ListDirectory(const char *path) {
    DIR *dir = opendir(path); //открыть каталог
    if (dir == NULL) {
        perror("Ошибка при открытии каталога");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) { 
        printf("%s\n", entry->d_name); 
    }
    closedir(dir);
}

void RemoveDirectory(const char *path) { 
    if (rmdir(path) == -1) { 
        perror("Ошибка при удалении каталога");
    } else {
        printf("Каталог '%s' успешно удален.\n", path);
    }
}

void CreateFile(const char *path) { 
    int fd = open(path, O_CREAT | O_WRONLY, 0644); //rw-r--r--
    if (fd == -1) {
        perror("Ошибка при создании файла");
    } else {
        close(fd); 
        printf("Файл '%s' успешно создан.\n", path);
    }
}

void ReadFile(const char *path) {
    char buffer[1024];
    int fd = open(path, O_RDONLY); 
    if (fd == -1) {
        perror("Ошибка при открытии файла");
        return;
    }
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer)))) { //частями читаю файл
        if (bytes_read == -1) {
            perror("Ошибка при чтении файла");
            break;
        }
        write(STDOUT_FILENO, buffer, bytes_read); 
    }
    close(fd); // Закрываем файл
}
    
void RemoveFile(const char *path) {
    if (unlink(path) == -1) { 
        perror("Ошибка при удалении файла"); 
    } else {
        printf("Файл '%s' успешно удален.\n", path); 
    }
}

void CreateSymlink(const char *target, const char *link_path) { 
    if (symlink(target, link_path) == -1) { //таргет - на что указ ссылка, линк пас - путь самого файла ссылкти
        perror("Ошибка при создании символьной ссылки");
    } else {
        printf("Символьная ссылка '%s' -> '%s' успешно создана.\n", link_path, target);
    }
}
    
void ReadSymlink(const char *path) {//вывод содержимого сим ссылки
    char buffer[4096];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("Ошибка при чтении символьной ссылки"); 
    } else {
        buffer[len] = '\0';
        printf("Символьная ссылка '%s' указывает на: %s\n", path, buffer); // Выводим содержимое ссылки
    }
}
        
void ReadSymlinkTarget(const char *path) { //вывод содержимого файла, на который указ симв ссылка
    char buffer[4096];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1); 
    if (len == -1) {
        perror("Ошибка при чтении символьной ссылки"); 
        return;
    }
    buffer[len] = '\0'; 
    ReadFile(buffer); 
}
        
void RemoveSymlink(const char *path) { 
    if (unlink(path) == -1) { 
        perror("Ошибка при удалении символьной ссылки");
    } else {
        printf("Символьная ссылка '%s' успешно удалена.\n", path); 
    }
}
        
void CreateHardlink(const char *target, const char *link_path) { 
    if (link(target, link_path) == -1) { 
        perror("Ошибка при создании жесткой ссылки");
    } else {
        printf("Жесткая ссылка '%s' -> '%s' успешно создана.\n", link_path, target);
    }
}
        
void RemoveHardlink(const char *path) {
    if (unlink(path) == -1) {
        perror("Ошибка при удалении жесткой ссылки"); 
    } else {
        printf("Жесткая ссылка '%s' успешно удалена.\n", path); 
    }
}
        
void PrintFileInfo(const char *path) { 
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Ошибка при получении информации о файле");
        return;
    }
    printf("Права доступа: %o\n", st.st_mode & 0777); 
    printf("Количество жестких ссылок: %hu\n", st.st_nlink);
}
            
void ChangePermissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) { 
        perror("Ошибка при изменении прав доступа");
    } else {
        printf("Права доступа к файлу '%s' успешно изменены.\n", path);
    }
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <аргументы>\n", argv[0]); // Выводим сообщение об ошибке, если аргументов недостаточно
        return 1;
    }

    const char *action = strrchr(argv[0], '/'); //указатель на /create_dir
    action = action ? action + 1 : argv[0]; //имя без пути

    if (strcmp(action, "create_dir") == 0) {
        CreateDirectory(argv[1]);
    } else if (strcmp(action, "list_dir") == 0) {
        ListDirectory(argv[1]); 
    } else if (strcmp(action, "remove_dir") == 0) {
        RemoveDirectory(argv[1]); 
    } else if (strcmp(action, "create_file") == 0) {
        CreateFile(argv[1]); 
    } else if (strcmp(action, "read_file") == 0) {
        ReadFile(argv[1]); 
    } else if (strcmp(action, "remove_file") == 0) {
        RemoveFile(argv[1]); 
    } else if (strcmp(action, "create_symlink") == 0) {
        CreateSymlink(argv[1], argv[2]); 
    } else if (strcmp(action, "read_symlink") == 0) {
        ReadSymlink(argv[1]); 
    } else if (strcmp(action, "read_symlink_target") == 0) {
        ReadSymlinkTarget(argv[1]); 
    } else if (strcmp(action, "remove_symlink") == 0) {
        RemoveSymlink(argv[1]); 
    } else if (strcmp(action, "create_hardlink") == 0) {
        CreateHardlink(argv[1], argv[2]); 
    } else if (strcmp(action, "remove_hardlink") == 0) {
        RemoveHardlink(argv[1]); 
    } else if (strcmp(action, "print_file_info") == 0) {
        PrintFileInfo(argv[1]); 
    } else if (strcmp(action, "change_permissions") == 0) {
        ChangePermissions(argv[1], strtol(argv[2], NULL, 8)); 
    } else {
        fprintf(stderr, "Неизвестное действие: %s\n", action);
        return 1;
    }
    return 0;
}