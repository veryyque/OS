#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *lib = dlopen("./libdynamic.dylib", RTLD_LAZY);
    if (!lib) {
        printf("DLOPEN ERROR: %s\n", dlerror());
        return 1;
    }

    void (*hello)(void) = dlsym(lib, "hello_from_dynamic_lib");
    if (!hello) {
        printf("DL ERROR: %s\n",dlerror());
        dlclose(lib);
        return 1;
    }

    hello();
    dlclose(lib);
    return 0;
}