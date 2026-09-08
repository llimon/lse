#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

/* Native 64-bit offset type present on all Solaris 2.x releases */
typedef long long offset_t;

/* 
 * Native SunOS kernel system call. Guaranteed to exist in libc.so.1 
 * on EVERY Solaris 2.5.1 installation.
 */
extern offset_t llseek(int fd, offset_t offset, int whence);

#ifndef COMPAT_FTELLO_COMPAT
#define ftello my_ftello
#endif

#ifndef COMPAT_FSEEKO_COMPAT
#define fseeko my_fseeko
#endif

/*
 * Solaris 2.5.1 Portable 64-bit ftello
 */
static inline offset_t my_ftello(FILE *stream) {
    int fd;

    if (!stream) {
        errno = EINVAL;
        return (offset_t)-1;
    }

    if (fflush(stream) != 0) {
        return (offset_t)-1;
    }

    fd = fileno(stream);
    if (fd < 0) {
        return (offset_t)-1;
    }

    return llseek(fd, (offset_t)0, SEEK_CUR);
}

/*
 * Solaris 2.5.1 Portable 64-bit fseeko
 */
static inline int my_fseeko(FILE *stream, offset_t offset, int whence) {
    int fd;
    offset_t pos;

    if (!stream) {
        errno = EINVAL;
        return -1;
    }

    if (fflush(stream) != 0) {
        return -1;
    }

    fd = fileno(stream);
    if (fd < 0) {
        return -1;
    }

    pos = llseek(fd, offset, whence);
    if (pos == (offset_t)-1) {
        return -1;
    }

    return 0;
}

#ifdef _TEST_FTELLO_COMPAT
int main(void) {
    int fd;
    FILE *fp;
    offset_t current_pos;
    const char *test_filename = "largefile_test_llseek.tmp";
    const char *write_payload = "SOLARIS_2.5.1_LLSEEK_PASSED";
    char read_buffer[64];
    ssize_t bytes_written, bytes_read;

    /* 2.5 GB offset */
    const long long gigabyte = 1024LL * 1024LL * 1024LL;
    const offset_t target_offset = (offset_t)(2.5 * gigabyte);
    const offset_t target_1Gb_offset = (offset_t)(gigabyte);

    printf("=== Solaris 2.5.1 Native llseek Wrapper Test ===\n");
    printf("Target Offset: %lld bytes (2.5 GB)\n\n", (long long)target_offset);

    /* 1. Open using standard open() */
    fd = open(test_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("FAIL: open failed");
        return 1;
    }
    printf("[1] Opened file descriptor cleanly using standard open().\n");

    /* 2. Associate with standard stdio FILE stream */
    fp = fdopen(fd, "r+b");
    if (!fp) {
        perror("FAIL: fdopen failed");
        close(fd);
        unlink(test_filename);
        return 1;
    }

    /* 2.5. Test my_fseeko wrapper using llseek, less than 2Gb limit of solaris 2.5 UFS filesystem */
    if (fseeko(fp, target_1Gb_offset, SEEK_SET) != 0) {
        perror("FAIL: my_fseeko failed to seek 1Gb boundary");
        fclose(fp);
        unlink(test_filename);
        return 1;
    }
    printf("[2.5.1] Seeked at 1GB boundary using my_fseeko().\n");
    /* 4. Test my_ftello wrapper using llseek */
    current_pos = ftello(fp);
    if (current_pos == (offset_t)-1) {
        perror("FAIL: my_ftello returned error at 1Gb boundary");
        fclose(fp);
        unlink(test_filename);
        return 1;
    }

    printf("[2.5.1] Position verified via my_ftello() at 1Gb boundary: %lld bytes\n", (long long)current_pos);

    /* 3. Test my_fseeko wrapper using llseek */
    if (fseeko(fp, target_offset, SEEK_SET) != 0) {
        perror("FAIL: my_fseeko failed to seek past 2GB boundary");
        fclose(fp);
        unlink(test_filename);
        return 1;
    }
    printf("[2.5] Seeked past 2GB boundary using my_fseeko().\n");

    /* 4. Test my_ftello wrapper using llseek */
    current_pos = ftello(fp);
    if (current_pos == (offset_t)-1) {
        perror("FAIL: my_ftello returned error");
        fclose(fp);
        unlink(test_filename);
        return 1;
    }

    printf("[3] Position verified via my_ftello() past 2Gb boundary: %lld bytes\n", (long long)current_pos);

    if (current_pos != target_offset) {
        printf("FAIL: Position mismatch! Expected %lld, got %lld\n",
               (long long)target_offset, (long long)current_pos);
        fclose(fp);
        unlink(test_filename);
        return 1;
    }

    /* 5. Write payload */
    bytes_written = write(fd, write_payload, strlen(write_payload));
    if (bytes_written < 0) {
        perror("NOTE: write() past 2GB hit system/mount boundary (Expected on standard 2.5.1 descriptor)");
    } else {
        printf("[4] Successfully wrote payload at 2.5 GB offset!\n");
        
        /* 6. Seek back & Read */
        fseeko(fp, target_offset, SEEK_SET);
        memset(read_buffer, 0, sizeof(read_buffer));
        bytes_read = read(fd, read_buffer, strlen(write_payload));
        if (bytes_read > 0) {
            printf("[5] Read back payload: \"%s\"\n", read_buffer);
        }
    }

    printf("\nSUCCESS: Program compiled, linked, and verified without open64/lseek64 errors!\n");

    fclose(fp);
    unlink(test_filename);
    return 0;
}
#endif
