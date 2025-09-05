#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

/**
 * @brief Write a null-terminated string to the semihosting console
 * @param s Null-terminated string to write
 */
void sh_write0(const char *s);

/**
 * @brief Exit the program with specified exit code
 * @param code Exit code (0 = success, non-zero = error)
 */
void sh_exit(int code);

#endif /* SEMIHOSTING_H */