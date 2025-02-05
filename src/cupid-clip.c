#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cupidconf.h"  

#define TEMP_FILE "/tmp/cupid_clip_temp.txt"
#define DEFAULT_CONFIG "~/.cupidclip.conf"

/**
 * expand_tilde:
 * If the path starts with "~/" or is exactly "~", replace it with the user's HOME.
 * Otherwise, return strdup(path) unchanged.
 * Caller must free() the returned string.
 */
char *expand_tilde(const char *path) {
    if (!path || path[0] == '\0') {
        return strdup("");
    }
    if (path[0] == '~' && (path[1] == '\0' || path[1] == '/')) {
        const char *home = getenv("HOME");
        if (!home) {
            struct passwd *pw = getpwuid(getuid());
            if (pw) {
                home = pw->pw_dir;
            } else {
                return strdup(path);
            }
        }
        size_t home_len = strlen(home);
        size_t remainder_len = strlen(path + 1); // Skip '~'
        char *expanded = malloc(home_len + remainder_len + 2); // +2 for possible '/' and '\0'
        if (!expanded) return NULL;
        strcpy(expanded, home);
        if (path[1] == '/') {
            strcat(expanded, path + 1);
        }
        return expanded;
    }
    return strdup(path);
}

/**
 * build_path:
 * Concatenates directory and entry_name into dest.
 */
char *build_path(char *dest, size_t size, const char *directory, const char *entry_name) {
    dest[0] = '\0';
    if (!directory || directory[0] == '\0') {
        snprintf(dest, size, "%s", entry_name);
        return dest;
    }
    size_t dir_len = strlen(directory);
    if (directory[dir_len - 1] == '/') {
        snprintf(dest, size, "%s%s", directory, entry_name);
    } else {
        snprintf(dest, size, "%s/%s", directory, entry_name);
    }
    return dest;
}

/**
 * remove_trailing_slash:
 * Removes trailing slashes from the path (except when the path is just "/").
 */
void remove_trailing_slash(char *path) {
    size_t len = strlen(path);
    while (len > 1 && path[len - 1] == '/') {
        path[len - 1] = '\0';
        len--;
    }
}

/**
 * copy_to_clipboard:
 * Uses xclip to copy the content of temp_file_path to the clipboard.
 * The verbose parameter is not used.
 */
void copy_to_clipboard(const char *temp_file_path, int verbose) {
    (void)verbose;  /* suppress unused parameter warning */
    char command[256];
    snprintf(command, sizeof(command), "xclip -selection clipboard < %s", temp_file_path);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to copy content to clipboard. Ensure xclip is installed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Content copied to clipboard successfully.\n");
}

/**
 * read_file_and_save_to_temp:
 * Reads a file from 'filepath' and writes its contents to temp_file.
 */
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

/**
 * is_ignored:
 * Returns 1 if 'path' should be ignored (matches any prefix in ignore_list),
 * otherwise returns 0.
 */
int is_ignored(const char *path, const char **ignore_list, int ignore_count) {
    char real_candidate[PATH_MAX];
    if (!realpath(path, real_candidate)) {
        strncpy(real_candidate, path, sizeof(real_candidate));
        real_candidate[sizeof(real_candidate) - 1] = '\0';
    }
    for (int i = 0; i < ignore_count; i++) {
        char real_ignore[PATH_MAX];
        if (!realpath(ignore_list[i], real_ignore)) {
            strncpy(real_ignore, ignore_list[i], sizeof(real_ignore));
            real_ignore[sizeof(real_ignore) - 1] = '\0';
        }
        size_t len = strlen(real_ignore);
        if (strncmp(real_candidate, real_ignore, len) == 0) {
            char next_char = real_candidate[len];
            if (next_char == '\0' || next_char == '/') {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * read_directory_and_save_to_temp:
 * Recursively (if requested) reads a directory, ignoring paths found in ignore_list,
 * and saves the content of regular files to temp_file.
 */
void read_directory_and_save_to_temp(const char *directory, FILE *temp_file,
                                       int recursive, int verbose, 
                                       const char **ignore_list, int ignore_count) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        return;
    }
    struct dirent *entry;
    struct stat file_stat;
    while ((entry = readdir(dir)) != NULL) {
        char filepath[1024];
        build_path(filepath, sizeof(filepath), directory, entry->d_name);

        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (is_ignored(filepath, ignore_list, ignore_count)) {
            if (verbose) {
                printf("Ignoring: %s\n", filepath);
            }
            continue;
        }

        if (stat(filepath, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                read_file_and_save_to_temp(filepath, temp_file, verbose);
            } else if (recursive && S_ISDIR(file_stat.st_mode)) {
                if (verbose) {
                    printf("Entering directory: %s\n", filepath);
                }
                read_directory_and_save_to_temp(filepath, temp_file, recursive, verbose,
                                                  ignore_list, ignore_count);
            }
        } else {
            fprintf(stderr, "Skipping non-existent or invalid file: %s\n", filepath);
        }
    }
    closedir(dir);
}

/**
 * main:
 * Parses command-line arguments, loads optional configuration, and processes
 * specified files/directories.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-R] [-V] [-I <file_or_dir>]... <path1> [path2] ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int recursive = 0, verbose = 0;
    const char **ignore_list = malloc(sizeof(char *) * argc);
    if (!ignore_list) {
        fprintf(stderr, "Memory allocation failed for ignore_list.\n");
        return EXIT_FAILURE;
    }
    int ignore_count = 0;
    const char **paths = malloc(sizeof(char *) * argc);
    if (!paths) {
        fprintf(stderr, "Memory allocation failed for paths.\n");
        free(ignore_list);
        return EXIT_FAILURE;
    }
    int path_count = 0;

    /* Load configuration from DEFAULT_CONFIG if available */
    char *config_path = expand_tilde(DEFAULT_CONFIG);
    cupidconf_t *config = cupidconf_load(config_path);
    free(config_path);
    if (config) {
        int count = 0;
        char **ignores = cupidconf_get_list(config, "ignore", &count);
        if (ignores) {
            for (int i = 0; i < count; i++) {
                char *dup_ignore = strdup(ignores[i]);
                if (dup_ignore) {
                    ignore_list[ignore_count++] = dup_ignore;
                }
            }
            free(ignores);  /* Free the temporary list array */
        }
        /* Optionally, load other settings (e.g., recursive, verbose) from the config */
        const char *cfg_recursive = cupidconf_get(config, "recursive");
        if (cfg_recursive && strcmp(cfg_recursive, "true") == 0) {
            recursive = 1;
        }
        const char *cfg_verbose = cupidconf_get(config, "verbose");
        if (cfg_verbose && strcmp(cfg_verbose, "true") == 0) {
            verbose = 1;
        }
        cupidconf_free(config);
    }

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-R") == 0) {
            recursive = 1;
        } else if (strcmp(argv[i], "-V") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-I") == 0) {
            if (i + 1 < argc) {
                i++;
                char *expanded = expand_tilde(argv[i]);
                if (!expanded) {
                    fprintf(stderr, "Error expanding path: %s\n", argv[i]);
                    return EXIT_FAILURE;
                }
                ignore_list[ignore_count++] = expanded;
            } else {
                fprintf(stderr, "Error: -I option requires an argument.\n");
                return EXIT_FAILURE;
            }
        } else {
            char *expanded = expand_tilde(argv[i]);
            if (!expanded) {
                fprintf(stderr, "Error expanding path: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
            paths[path_count++] = expanded;
        }
    }

    if (path_count == 0) {
        fprintf(stderr, "Error: No file or directory specified.\n");
        free(ignore_list);
        free(paths);
        return EXIT_FAILURE;
    }

    /* Remove trailing slashes from ignored paths and main paths */
    for (int i = 0; i < ignore_count; i++) {
        remove_trailing_slash((char *)ignore_list[i]);
    }
    for (int i = 0; i < path_count; i++) {
        remove_trailing_slash((char *)paths[i]);
    }

    /* Create a temporary file */
    FILE *temp_file = fopen(TEMP_FILE, "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        return EXIT_FAILURE;
    }
    fclose(temp_file);

    temp_file = fopen(TEMP_FILE, "a");
    if (!temp_file) {
        perror("Error opening temporary file for appending");
        return EXIT_FAILURE;
    }

    /* Process each specified path */
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
            read_directory_and_save_to_temp(path, temp_file, recursive, verbose,
                                              ignore_list, ignore_count);
        } else {
            fprintf(stderr, "Error: '%s' is not a valid file or directory.\n", path);
        }
    }
    fclose(temp_file);

    copy_to_clipboard(TEMP_FILE, verbose);

    if (remove(TEMP_FILE) != 0) {
        perror("Error removing temporary file");
    }
    printf("Operation completed successfully.\n");

    /* Cleanup allocated memory */
    for (int i = 0; i < ignore_count; i++) {
        free((void *)ignore_list[i]);
    }
    for (int i = 0; i < path_count; i++) {
        free((void *)paths[i]);
    }
    free(ignore_list);
    free(paths);

    return EXIT_SUCCESS;
}
