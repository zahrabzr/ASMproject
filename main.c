#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 50
#define FILENAME "users.txt"

typedef enum { MENU, LOGIN, SIGNUP, SETTINGS, GAME } GameScreen;
typedef enum { EASY, MEDIUM, HARD } DifficultyLevel;

void UpdateHighestScore(const char *username, int newScore) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;

    char storedUser[MAX_NAME_LEN], storedPass[MAX_PASS_LEN];
    int storedScore;
    char tempFilename[] = "users_temp.txt";
    FILE *tempFile = fopen(tempFilename, "w");
    if (!tempFile) {
        fclose(file);
        return;
    }

    while (fscanf(file, "%s %s %d", storedUser, storedPass, &storedScore) == 3) {
        if (strcmp(username, storedUser) == 0) {
            // If the user is found, update the highest score if it's greater than the current one
            if (newScore > storedScore) {
                storedScore = newScore;
            }
        }
        fprintf(tempFile, "%s %s %d\n", storedUser, storedPass, storedScore);
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated one
    remove(FILENAME);
    rename(tempFilename, FILENAME);
}


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

bool VerifyLogin(const char *username, const char *password, int *highScore) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return false;

    char storedUser[MAX_NAME_LEN], storedPass[MAX_PASS_LEN];
    int storedScore;
    while (fscanf(file, "%s %s %d", storedUser, storedPass, &storedScore) == 3) {
        if (strcmp(username, storedUser) == 0 && strcmp(password, storedPass) == 0) {
            *highScore = storedScore;  // Assign the user's highest score to the passed pointer
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

    // Default highest score is 0 for new users
    fprintf(file, "%s %s %d\n", username, password, 0);
    fclose(file);
    return true;
}


int main() {
    InitWindow(626, 417, "Badminton Game - Login System");
    SetTargetFPS(60);
    
    Texture2D bgMenu = LoadTexture("assets/menu_bg3.jpg");
    Texture2D venueImages[3] = {
        LoadTexture("assets/venue1.png"),
        LoadTexture("assets/venue2.png"),
        LoadTexture("assets/venue3.png")
    };

    Image icon = LoadImage("assets/icon3.jpg"); 
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);
    Font gameFont = LoadFont("assets/game_font.ttf");  // Change to your actual font file

    GameScreen screen = MENU;
    DifficultyLevel difficulty = EASY;
    int venueIndex = 0;

    char username[MAX_NAME_LEN] = {0};
    char password[MAX_PASS_LEN] = {0};
    int userIndex = 0, passIndex = 0;
    bool loginFailed = false;
    int focusedField = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(bgMenu, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        // Declare these variables outside the SETTINGS block
        static DifficultyLevel tempDifficulty;
        static int tempVenueIndex;

        if (screen == MENU) {
            DrawTextEx(gameFont, "1. Login", (Vector2){200, 140}, 30, 2, BLACK);
            DrawTextEx(gameFont, "2. Sign Up", (Vector2){200, 190}, 30, 2, BLACK);
            DrawTextEx(gameFont, "3. Settings", (Vector2){200, 240}, 30, 2, BLACK);
            DrawTextEx(gameFont, "4. Exit", (Vector2){200, 290}, 30, 2, BLACK);
            
            if (IsKeyPressed(KEY_ONE)) screen = LOGIN;
            if (IsKeyPressed(KEY_TWO)) screen = SIGNUP;
            if (IsKeyPressed(KEY_THREE)) screen = SETTINGS;
            if (IsKeyPressed(KEY_FOUR)) break;
        }

        else if (screen == SETTINGS) {
            DrawTextEx(gameFont, "Setting", (Vector2){135, 40}, 24, 2, BLACK);
            DrawTextEx(gameFont, "Select Difficulty:", (Vector2){220, 110}, 16, 2, BLACK);
            DrawTextEx(gameFont, tempDifficulty == EASY ? "Easy" : tempDifficulty == MEDIUM ? "Medium" : "Hard", (Vector2){430, 110}, 16, 2, RED);

            DrawTextEx(gameFont, "Select Venue:", (Vector2){220, 210}, 16, 2, BLACK);

            // Display venue image
            DrawTextureEx(venueImages[tempVenueIndex], (Vector2){425, 210}, 0.0f, 0.2f, WHITE);

            // Display instructions
            DrawTextEx(gameFont, "(Use Left/Right to change difficulty)", (Vector2){220, 140}, 10, 2, DARKGRAY);
            DrawTextEx(gameFont, "(Use A/D to change venue)", (Vector2){220, 240}, 10, 2, DARKGRAY);
            DrawTextEx(gameFont, "Press ENTER to confirm", (Vector2){220, 300}, 12, 2, GREEN);
            DrawTextEx(gameFont, "Press Q to go back", (Vector2){220, 330}, 12, 2, RED);

            // Change difficulty with Left/Right keys
            if (IsKeyPressed(KEY_LEFT)) tempDifficulty = (tempDifficulty == EASY) ? HARD : (DifficultyLevel)(tempDifficulty - 1);
            if (IsKeyPressed(KEY_RIGHT)) tempDifficulty = (tempDifficulty == HARD) ? EASY : (DifficultyLevel)(tempDifficulty + 1);

            // Change venue with A/D keys
            if (IsKeyPressed(KEY_A)) tempVenueIndex = (tempVenueIndex + 2) % 3;  // Wrap around left
            if (IsKeyPressed(KEY_D)) tempVenueIndex = (tempVenueIndex + 1) % 3;  // Wrap around right

            // Confirm selection with ENTER
            if (IsKeyPressed(KEY_ENTER)) {
                difficulty = tempDifficulty;
                venueIndex = tempVenueIndex;
                printf("Settings Saved! Difficulty: %d, Venue: %d\n", difficulty, venueIndex);
            }

            // Go back to menu
            if (IsKeyPressed(KEY_Q)) screen = MENU;
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
            DrawTextEx(gameFont, "Enter Username:", (Vector2){200, 110}, 15, 2, BLACK);
            DrawTextEx(gameFont, username, (Vector2){400, 110}, 15, 2, BLACK);

            DrawTextEx(gameFont, "Enter Password:", (Vector2){200, 160}, 15, 2, BLACK);
            DrawTextEx(gameFont, password, (Vector2){400, 160}, 15, 2, BLACK);  // Shows typed password

            DrawTextEx(gameFont, "Press ENTER to submit, Q to go back", (Vector2){110, 280}, 12, 2, RED);
            DrawTextEx(gameFont, screen == LOGIN ? "LOGIN" : "SIGN-UP", (Vector2){135, 40}, 24, 2, RED);


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
                DrawTextEx(gameFont, "Invalid username or password!", (Vector2){100, 350}, 20, 2, YELLOW);
            }

            // Handle text input for the selected field
            int key = GetCharPressed();
            while (key > 0) {
                if (focusedField == 0 && userIndex < MAX_NAME_LEN - 1 && key >= 32 && key <= 126) {
                    username[userIndex++] = (char)key;
                    username[userIndex] = '\0';
                    loginFailed = false; // Reset error when user starts typing
                }
                if (focusedField == 1 && passIndex < MAX_PASS_LEN - 1 && key >= 32 && key <= 126) {
                    password[passIndex++] = (char)key;
                    password[passIndex] = '\0';
                    loginFailed = false; // Reset error when user starts typing
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
                    int highScore = 0;  // Variable to hold the user's high score
                    if (VerifyLogin(username, password, &highScore)) {
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
// --- Begin GAME branch ---
else if (screen == GAME) {
    CloseWindow(); // Close login window

    // Reinitialize a new window for the game
    InitWindow(1440, 810, "Badminton Game");
    SetTargetFPS(60);

    // Set window icon
    Image icon = LoadImage("assets/icon3.jpg"); 
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    // Select the correct background based on venueIndex
    Texture2D bgGame;
    if (venueIndex == 0) {
        bgGame = LoadTexture("assets/venue1.png");
    } else if (venueIndex == 1) {
        bgGame = LoadTexture("assets/venue2.png");
    } else if (venueIndex == 2) {
        bgGame = LoadTexture("assets/venue3.png");
    } else {
        bgGame = LoadTexture("assets/venue1.png");  // Default fallback
    }

    // Load left player textures (two poses)
    Texture2D LplayerA = LoadTexture("assets/LplayerA (1).png"); // Racket above head
    Texture2D LplayerU = LoadTexture("assets/LplayerU (2).png"); // Racket under body

    // Load right player texture (for response – curve shot)
    Texture2D RplayerU = LoadTexture("assets/RplayerU.png");

    // Load shuttle texture for left-to-right shot (use shuttleR)
    Texture2D shuttleR = LoadTexture("assets/shuttleR.png");

    // Fixed positions for players (set as before)
    Vector2 leftPlayerPos = {210, 310};    // Left player's fixed position
    Vector2 rightPlayerPos = {170, 360};    // Right player's fixed position

    // Initialize variables for left player's shot
    typedef enum { SHOT_NONE, SHOT_STRAIGHT, SHOT_SIN, SHOT_CURVE } ShotType;
    ShotType leftShotType = SHOT_NONE;
    bool shotFired = false;
    float shotTime = 0.0f;
    Vector2 ballPos = {0, 0};     // Current shuttle position
    Vector2 ballStart = {0, 0};   // Starting position for the shot

    // We'll always use shuttleR texture since shots go left-to-right.
    bool useShuttleR = true;

    // On-screen instructions
    const char* instructions = "For left shots: Z = Straight, X = Sin, C = Curve. (Straight & Sin use ABOVE image; Curve uses UNDER image)";

    // Define an enum for left player pose states
    typedef enum { POSE_ABOVE, POSE_UNDER } PlayerPose;
    PlayerPose leftPose = POSE_ABOVE;
    // For right player, we always use the curve response image.
    PlayerPose rightPose = POSE_ABOVE;

    // Main game loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Input Handling for Left Player Movement ---
        if (IsKeyDown(KEY_LEFT)) leftPlayerPos.x -= 5;  // Move left
        if (IsKeyDown(KEY_RIGHT)) leftPlayerPos.x += 5;   // Move right

        // --- Input Handling for Shot Selection (only if a shot is not already fired) ---
        if (!shotFired) {
            // Press Z for straight, X for sin, C for curve shot.
            if (IsKeyPressed(KEY_Z)) {
                leftShotType = SHOT_STRAIGHT;
                shotFired = true;
            } else if (IsKeyPressed(KEY_X)) {
                leftShotType = SHOT_SIN;
                shotFired = true;
            } else if (IsKeyPressed(KEY_C)) {
                leftShotType = SHOT_CURVE;
                shotFired = true;
            }
            // Set the ball starting position relative to the left player's fixed position.
            if (shotFired) {
                shotTime = 0.0f;
                ballStart.x = leftPlayerPos.x + 100;  // Offset to appear in front of the player.
                ballStart.y = leftPlayerPos.y + 50;
                ballPos = ballStart;
            }
        }

        // --- Update Ball Movement if Shot Fired ---
        if (shotFired) {
            shotTime += dt;
            float ballSpeed = 600.0f;  // Horizontal speed
            ballPos.x = ballStart.x + ballSpeed * shotTime;

            // Update vertical displacement based on shot type.
            if (leftShotType == SHOT_STRAIGHT) {
                ballPos.y = ballStart.y;
            } else if (leftShotType == SHOT_SIN) {
                ballPos.y = ballStart.y + sin(shotTime * 4.0f) * 50;
            } else if (leftShotType == SHOT_CURVE) {
                float dx = ballPos.x - ballStart.x;
                ballPos.y = ballStart.y + 0.001f * dx * dx;
            }
        }

        // --- Right Player AI Movement ---
        // Right player moves automatically to track the ball's x-position.
        float aiSpeed = 3.0f;
        if (shotFired) { // Only move if a shot is in progress.
            if (rightPlayerPos.x + 50 < ballPos.x) rightPlayerPos.x += aiSpeed;
            if (rightPlayerPos.x + 50 > ballPos.x) rightPlayerPos.x -= aiSpeed;
        }

        // --- Determine Left Player Pose ---
        // Use the ABOVE image for straight and sin shots; use the UNDER image for curve shots.
        Texture2D leftPlayerTexture;
        if (shotFired) {
            if (leftShotType == SHOT_CURVE)
                leftPlayerTexture = LplayerU;
            else
                leftPlayerTexture = LplayerA;
        } else {
            leftPlayerTexture = LplayerA;
        }

        // For the right player, always use the curve (under) image as response.
        Texture2D rightPlayerTexture = RplayerU;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // --- Draw Background ---
        if (bgGame.id != 0) {
            Vector2 screenSize = { (float)GetScreenWidth(), (float)GetScreenHeight() };
            float scaleX = screenSize.x / bgGame.width;
            DrawTextureEx(bgGame, (Vector2){0, 0}, 0.0f, scaleX, WHITE);
        } else {
            DrawText("Error: Background not loaded!", 100, 100, 20, RED);
        }

        // --- (Optional) Debug: Draw Mouse Coordinates ---
        Vector2 mousePos = GetMousePosition();
        DrawText(TextFormat("Mouse: (%.0f, %.0f)", mousePos.x, mousePos.y), 20, 20, 20, RED);

        // --- Draw Left Player ---
        DrawTexture(leftPlayerTexture, (int)leftPlayerPos.x, (int)leftPlayerPos.y, WHITE);

        // --- Draw Right Player ---
        DrawTexture(rightPlayerTexture, (int)rightPlayerPos.x, (int)rightPlayerPos.y, WHITE);

        // --- Draw Shuttle (Ball) ---
        if (shotFired) {
            DrawTexture(shuttleR, (int)ballPos.x, (int)ballPos.y, WHITE);
        }

        DrawText(instructions, 10, 750, 20, DARKBLUE);

        EndDrawing();
    }

    // --- Cleanup Resources ---
    UnloadTexture(bgGame);
    UnloadTexture(LplayerA);
    UnloadTexture(LplayerU);
    UnloadTexture(RplayerU);
    UnloadTexture(shuttleR);
    CloseWindow();
    return 0;
}

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(bgMenu);
    CloseWindow();

    return 0;
}
