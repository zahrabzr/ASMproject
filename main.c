#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 50
#define FILENAME "users.txt"

typedef enum { MENU, LOGIN, SIGNUP, GAME } GameScreen;

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

bool RegisterUser(const char *username, const char *password) {
    if (UsernameExists(username)) {
        return false; // Username already taken
    }

    FILE *file = fopen(FILENAME, "a");
    if (!file) return false;

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    return true;
}

int main() {
    InitWindow(626, 417, "Badminton Game - Login System");
    SetTargetFPS(60);
    
    Texture2D bgMenu = LoadTexture("assets/menu_bg3.jpg");

    Image icon = LoadImage("assets/icon3.jpg"); 
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    GameScreen screen = MENU;

    char username[MAX_NAME_LEN] = {0};
    char password[MAX_PASS_LEN] = {0};
    int userIndex = 0, passIndex = 0;

    bool loginFailed = false; 

    int focusedField = 0;  // 0 = Username, 1 = Password

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(bgMenu, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);

        if (screen == MENU) {
            DrawText("1. Login", 230, 140, 30, BLACK);
            DrawText("2. Sign Up", 230, 190, 30, BLACK);
            DrawText("3. Exit", 230, 240, 30, BLACK);

            if (IsKeyPressed(KEY_ONE)) screen = LOGIN;
            if (IsKeyPressed(KEY_TWO)) screen = SIGNUP;
            if (IsKeyPressed(KEY_THREE)) break;
        }
        else if (screen == LOGIN || screen == SIGNUP) {
            // Define text box positions
            Rectangle usernameBox = {380, 105, 170, 30};
            Rectangle passwordBox = {380, 155, 170, 30};

            // Check mouse hover for color change
            Color usernameColor = CheckCollisionPointRec(GetMousePosition(), usernameBox) ? LIGHTGRAY : WHITE;
            Color passwordColor = CheckCollisionPointRec(GetMousePosition(), passwordBox) ? LIGHTGRAY : WHITE;

            // Draw the input boxes
            DrawRectangleRec(usernameBox, usernameColor);
            DrawRectangleRec(passwordBox, passwordColor);

            // Labels
            DrawText("Enter Username:", 200, 110, 20, BLACK);
            DrawText(username, 400, 110, 20, BLACK);

            DrawText("Enter Password:", 200, 160, 20, BLACK);
            DrawText(password, 400, 160, 20, BLACK);  // Shows typed password

            DrawText("Press ENTER to submit, Q to go back", 110, 280, 18, RED);
            DrawText(screen == LOGIN ? "LOGIN MENU" : "SIGN-UP MENU", 110, 310, 15, RED);

            // Handle mouse click for focus switching
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(GetMousePosition(), usernameBox)) {
                    focusedField = 0; // Focus on username
                }
                if (CheckCollisionPointRec(GetMousePosition(), passwordBox)) {
                    focusedField = 1; // Focus on password
                }
            }

            if (loginFailed) {
                DrawText("Invalid username or password!", 100, 350, 20, RED);
            }


            // Handle text input for the selected field
            int key = GetCharPressed();
            while (key > 0) {
                if (focusedField == 0 && userIndex < MAX_NAME_LEN - 1 && key >= 32 && key <= 126) {
                    username[userIndex++] = (char)key;
                    username[userIndex] = '\0';
                }
                if (focusedField == 1 && passIndex < MAX_PASS_LEN - 1 && key >= 32 && key <= 126) {
                    password[passIndex++] = (char)key;
                    password[passIndex] = '\0';
                }
                key = GetCharPressed();
            }

            // Handle backspace
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (focusedField == 1 && passIndex > 0) password[--passIndex] = '\0';
                else if (focusedField == 0 && userIndex > 0) username[--userIndex] = '\0';
            }

            // Handle enter key
            if (IsKeyPressed(KEY_ENTER)) {
                if (screen == LOGIN) {
                    if (VerifyLogin(username, password)) {
                        screen = GAME;  // Login successful → Go to game
                        loginFailed = false;  // Reset error state
                    } else {
                        loginFailed = true;  // Set error flag
                    }
                } else if (screen == SIGNUP) {
                    if (RegisterUser(username, password)) {
                        screen = LOGIN;  // Sign-up successful → Go to login screen
                        loginFailed = false;  // Reset error state
                    } else {
                        loginFailed = true;  // Set error flag
                    }
                }
            }


            // Q to go back
            if (IsKeyPressed(KEY_Q)) {
                printf("Q Pressed! Returning to MENU\n"); // Debugging
                screen = MENU;

                // Reset input fields
                memset(username, 0, sizeof(username));
                memset(password, 0, sizeof(password));
                userIndex = 0;
                passIndex = 0;

                // Ensure mouse interaction works properly
                focusedField = -1; // No field is focused now
            }

        }
        else if (screen == GAME) {
            // Close the current login window
            CloseWindow();

            // Create a new, larger window for the game
            InitWindow(1024, 768, "Badminton Game"); // Adjust size as needed
            SetTargetFPS(60);

            // Load the game background
            Texture2D bgGame = LoadTexture("assets/game_bg.jpg"); // Change to your actual game board background

            while (!WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(RAYWHITE);

                // Draw new game background
                DrawTextureEx(bgGame, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);

                DrawText("Welcome to the Game!", 350, 50, 30, BLACK);

                EndDrawing();
            }

            // Cleanup
            UnloadTexture(bgGame);
            CloseWindow();
        }


        EndDrawing();
    }

    // Cleanup
    UnloadTexture(bgMenu);
    CloseWindow();

    return 0;
}

