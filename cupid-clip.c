#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEMP_FILE "/tmp/cupid_clip_temp.txt"

void copy_to_clipboard(const char *temp_file_path, int verbose) {
    char command[256];
    snprintf(command, sizeof(command), "xclip -selection clipboard < %s", temp_file_path);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to copy content to clipboard. Ensure xclip is installed.\n");
        exit(EXIT_FAILURE);
    }
    // Always print this message, regardless of the verbose flag
    printf("Content copied to clipboard successfully.\n");
}

void read_file_and_save_to_temp(const char *filepath, FILE *temp_file, int verbose) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return;
    }
    if (verbose) {
        printf("Reading file: %s\n", filepath);
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes_read, temp_file);
    }

    fclose(file);
}

void read_directory_and_save_to_temp(const char *directory, FILE *temp_file, int recursive, int verbose) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (stat(filepath, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                read_file_and_save_to_temp(filepath, temp_file, verbose);
            } else if (recursive && S_ISDIR(file_stat.st_mode)) {
                if (verbose) {
                    printf("Entering directory: %s\n", filepath);
                }
                read_directory_and_save_to_temp(filepath, temp_file, recursive, verbose);
            }
        } else {
            fprintf(stderr, "Skipping non-existent or invalid file: %s\n", filepath);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-R] [-V] <directory_or_file> [more_files_or_directories...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int recursive = 0, verbose = 0;
    // Create an array to hold paths
    const char **paths = malloc(sizeof(char *) * argc); // Max possible paths is argc
    int path_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-R") == 0) {
            recursive = 1;
        } else if (strcmp(argv[i], "-V") == 0) {
            verbose = 1;
        } else {
            paths[path_count++] = argv[i];
        }
    }

    if (path_count == 0) {
        fprintf(stderr, "Error: No file or directory specified.\n");
        free(paths);
        return EXIT_FAILURE;
    }

    FILE *temp_file = fopen(TEMP_FILE, "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        free(paths);
        return EXIT_FAILURE;
    }
    fclose(temp_file);

    temp_file = fopen(TEMP_FILE, "a");
    if (!temp_file) {
        perror("Error opening temporary file for appending");
        free(paths);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < path_count; i++) {
        const char *path = paths[i];
        struct stat path_stat;
        if (stat(path, &path_stat) != 0) {
            fprintf(stderr, "Error: '%s' does not exist.\n", path);
            continue;
        }

        if (S_ISREG(path_stat.st_mode)) {
            if (verbose) {
                printf("Processing file: %s\n", path);
            }
            read_file_and_save_to_temp(path, temp_file, verbose);
        } else if (S_ISDIR(path_stat.st_mode)) {
            if (verbose) {
                printf("Processing directory: %s%s\n", path, recursive ? " (recursively)" : "");
            }
            read_directory_and_save_to_temp(path, temp_file, recursive, verbose);
        } else {
            fprintf(stderr, "Error: '%s' is not a valid file or directory.\n", path);
            continue;
        }
    }

    fclose(temp_file);

    copy_to_clipboard(TEMP_FILE, verbose);

    if (remove(TEMP_FILE) != 0) {
        perror("Error removing temporary file");
    }

    // Always print this message, regardless of the verbose flag
    printf("Operation completed successfully.\n");

    free(paths);
    return EXIT_SUCCESS;
}
