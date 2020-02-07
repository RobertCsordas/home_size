#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <dirent.h>
#include <string.h>

static uint64_t hack = 0;

int sum(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    hack += sb->st_size;
    return 0;
}

uint64_t get_dir_size(const char *fpath){
    hack = 0;
    if (!fpath || access(fpath, R_OK)) {
        return 0;
    }
    if (nftw(fpath, &sum, 1, FTW_PHYS)) {
        perror("ftw");
        return 0;
    }
    return hack;
}

char* readable_fs(double size) {
    int i = 0;
    const char* units[] = {"", "k", "M", "G", "T", "P", "E", "Z", "Y", NULL};
    while (size > 1024 && units[i+1]!=NULL) {
        size /= 1024;
        i++;
    }

    char * buf = malloc(128);
    int n = (uint64_t)size < 10 ? 1 : 0;

    snprintf(buf, 128, "%.*f%s", n, size, units[i]);
    return buf;
}

void list_dir(const char *dir){
    struct dirent *de;  
    DIR *dr = opendir(dir); 
  
    if (dr == NULL) { 
        printf("Could not open current directory" ); 
        return; 
    } 

    size_t dir_len=strlen(dir);

    while ((de = readdir(dr)) != NULL){
        if (de->d_type != DT_DIR || strcmp(de->d_name, ".")==0 || strcmp(de->d_name, "..")==0)
            continue;

        size_t buflen = dir_len + strlen(de->d_name) + 2;
        char *full_name = malloc(buflen);
        snprintf(full_name, buflen, "%s/%s", dir, de->d_name);

        uint64_t size = get_dir_size(full_name);
        char *fs = readable_fs(size);

        printf("%s\t%s\n", fs, de->d_name);
        
        free(full_name);
        free(fs);
    }
            
    closedir(dr);     
}

int main(int argc, char **argv) {
    list_dir("/home/");
}
