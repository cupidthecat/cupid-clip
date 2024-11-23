#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEMP_FILE "/tmp/cupid_clip_temp.txt"

void copy_to_clipboard(const char *temp_file_path) {
    char command[256];
    snprintf(command, sizeof(command), "xclip -selection clipboard < %s", temp_file_path);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to copy content to clipboard. Ensure xclip is installed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Content copied to clipboard successfully.\n");
}

void read_file_and_save_to_temp(const char *filepath, FILE *temp_file) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes_read, temp_file);
    }

    fclose(file);
}

void read_directory_and_save_to_temp(const char *directory, FILE *temp_file, int recursive) {
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

        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (stat(filepath, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                // Process regular files
                read_file_and_save_to_temp(filepath, temp_file);
            } else if (recursive && S_ISDIR(file_stat.st_mode)) {
                // Process subdirectories recursively if -R is enabled
                read_directory_and_save_to_temp(filepath, temp_file, recursive);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-R] <directory_or_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int recursive = 0;
    const char *path;

    // Check for the -R flag
    if (argc == 3 && strcmp(argv[1], "-R") == 0) {
        recursive = 1;
        path = argv[2];
    } else if (argc == 2) {
        path = argv[1];
    } else {
        fprintf(stderr, "Usage: %s [-R] <directory_or_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check if the input path is valid
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        fprintf(stderr, "Error: '%s' does not exist.\n", path);
        return EXIT_FAILURE;
    }

    // Open a temporary file to store the content
    FILE *temp_file = fopen(TEMP_FILE, "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        return EXIT_FAILURE;
    }

    // Process based on whether it's a file or directory
    if (S_ISREG(path_stat.st_mode)) {
        printf("Processing file: %s\n", path);
        read_file_and_save_to_temp(path, temp_file);
    } else if (S_ISDIR(path_stat.st_mode)) {
        printf("Processing directory: %s%s\n", path, recursive ? " (recursively)" : "");
        read_directory_and_save_to_temp(path, temp_file, recursive);
    } else {
        fprintf(stderr, "Error: '%s' is not a valid file or directory.\n", path);
        fclose(temp_file);
        return EXIT_FAILURE;
    }

    fclose(temp_file);

    // Copy content to clipboard
    copy_to_clipboard(TEMP_FILE);

    // Clean up temporary file
    if (remove(TEMP_FILE) != 0) {
        perror("Error removing temporary file");
    }

    return EXIT_SUCCESS;
}
