#include "auth.h"
#include <stdio.h>
#include <string.h>

const char *FILENAME = "users.txt";  // File for storing user data

// Check if a username exists
bool UsernameExists(const char *username) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return false;

    char storedUser[MAX_NAME_LEN], storedPass[MAX_PASS_LEN];
    while (fscanf(file, "%s %s", storedUser, storedPass) == 2) {
        if (strcmp(username, storedUser) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

// Verify username & password for login
bool VerifyLogin(const char *username, const char *password) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return false;

    char storedUser[MAX_NAME_LEN], storedPass[MAX_PASS_LEN];
    while (fscanf(file, "%s %s", storedUser, storedPass) == 2) {
        if (strcmp(username, storedUser) == 0 && strcmp(password, storedPass) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

// Register a new user
bool RegisterUser(const char *username, const char *password) {
    if (UsernameExists(username)) return false;

    FILE *file = fopen(FILENAME, "a");
    if (!file) return false;

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    return true;
}
