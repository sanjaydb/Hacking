
//  CVE-2017-13130 - BMC Patrol 'mcmnm' - Privilege Escalation via a Vulnerable SUID Binary
//
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_THREADS 10

// Structure to hold the search parameters
typedef struct {
    char* dir;
    pthread_t tid;
} SearchParams;

// Function to check if a file/folder has the SUID bit set
int hasSUIDBitSet(const char* path) {
    struct stat fileStat;
    if (stat(path, &fileStat) == -1) {
        perror("stat");
        return 0;
    }
    return (fileStat.st_mode & S_ISUID);
}

// Function to recursively search directories for files/folders with the SUID bit set
void* searchDirectories(void* arg) {
    SearchParams* params = (SearchParams*)arg;
    DIR* dir = opendir(params->dir);
    if (dir == NULL) {
        perror("opendir");
        pthread_exit(NULL);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char newPath[1024];
            snprintf(newPath, sizeof(newPath), "%s/%s", params->dir, entry->d_name);

            SearchParams newParams;
            newParams.dir = strdup(newPath);
            pthread_create(&newParams.tid, NULL, searchDirectories, &newParams);
            pthread_join(newParams.tid, NULL);
            free(newParams.dir);
        } else {
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", params->dir, entry->d_name);
            if (hasSUIDBitSet(filePath)) {
                printf("Found SUID bit set: %s\n", filePath);

                // Execute readelf -d on the file
                char command[1024];
                snprintf(command, sizeof(command), "readelf -d %s", filePath);
                FILE* fp = popen(command, "r");
                if (fp != NULL) {
                    char line[1024];
                    while (fgets(line, sizeof(line), fp) != NULL) {
                        // Search for "(NEEDED)" or "(RUNPATH)"
                        if (strstr(line, "(NEEDED)") != NULL || strstr(line, "(RUNPATH)") != NULL) {
                            printf("%s", line);
                        }
                    }
                    pclose(fp);
                } else {
                    perror("popen");
                }
            }
        }
    }

    closedir(dir);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    // Create the root search parameters
    SearchParams rootParams;
    rootParams.dir = strdup(argv[1]);

    // Create the initial thread for the root directory
    pthread_create(&rootParams.tid, NULL, searchDirectories, &rootParams);
    pthread_join(rootParams.tid, NULL);

    free(rootParams.dir);

    return 0;

}

