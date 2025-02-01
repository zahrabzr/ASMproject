#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 50

bool VerifyLogin(const char *username, const char *password);
bool RegisterUser(const char *username, const char *password);
bool UsernameExists(const char *username);

#endif
