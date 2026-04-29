//
// Created by Kiril on 4/24/2026.
//

#ifndef DATABASE_ERRORS_H
#define DATABASE_ERRORS_H

/**
 * @brief Prints an error message to stderr and terminates the program.
 * 
 * @param msg The message to print.
 */
#define DIE(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif //DATABASE_ERRORS_H