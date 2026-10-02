#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>

/*
 * Create all parent directories needed for file_path.
 *
 * Example:
 *   file_path = "output/logs/result.txt"
 *
 * This function creates:
 *   output
 *   output/logs
 *
 * It does not create result.txt; main() creates that as a file.
 *
 * Returns 0 on success and -1 on failure.
 */
static int create_parent_directories(const char *file_path)
{
    char *path_copy;
    char *current_character;

    /*
     * Make a writable copy because each '/' is temporarily replaced with '\0'
     * while calling mkdir().
     */
    path_copy = malloc(strlen(file_path) + 1);
    if (path_copy == NULL) {
        perror("malloc");
        return -1;
    }

    strcpy(path_copy, file_path);

    /*
     * Visit each slash in the path.  At a slash, temporarily terminate the
     * string so mkdir() receives the directory path before that slash.
     *
     * Starting at path_copy + 1 avoids treating '/' in an absolute path as an
     * empty directory name.
     */
    for (current_character = path_copy + 1;
         *current_character != '\0';
         current_character++) {
        if (*current_character != '/') {
            continue;
        }

        *current_character = '\0';

        /*
         * mkdir() fails with EEXIST when the directory is already present.
         * That is acceptable because the required directory already exists.
         */
        if (mkdir(path_copy, 0777) == -1 && errno != EEXIST) {
            perror(path_copy);
            free(path_copy);
            return -1;
        }

        /* Restore the slash before checking the rest of the path. */
        *current_character = '/';
    }

    free(path_copy);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *writefile;
    const char *writestr;
    FILE *file;

    /*
     * The program needs:
     *   argv[1] - file path
     *   argv[2] - text to write
     */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file-path> <text>\n", argv[0]);
        syslog(LOG_ERR, "Usage: %s <file-path> <text>\n", argv[0]);
        return EXIT_FAILURE;
    }

    writefile = argv[1];
    writestr = argv[2];

    openlog(NULL, 0, LOG_USER);
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    /* Create missing parent directories before opening the output file. */
    if (create_parent_directories(writefile) != 0) {
        return EXIT_FAILURE;
    }

    /*
     * "w" creates the file if absent, or replaces its contents if it exists.
     */
    file = fopen(writefile, "w");
    if (file == NULL) {
        perror(writefile);
        syslog(LOG_ERR, "FILE is NULL");
        return EXIT_FAILURE;
    }

    /* Write the supplied text, followed by a newline. */
    if (fputs(writestr, file) == EOF || fputc('\n', file) == EOF) {
        perror(writefile);
        fclose(file);
        syslog(LOG_ERR, "Write Failed");
        return EXIT_FAILURE;
    }

    /*
     * fclose() flushes buffered data. Its result is checked because a write
     * error can occur while the stream is being closed.
     */
    if (fclose(file) == EOF) {
        perror(writefile);
        return EXIT_FAILURE;
    }

    closelog();
    return EXIT_SUCCESS;
}
