#include "raylib.h"
#include <math.h>
#include <xmmintrin.h> 
#include <stdio.h>
#include <string.h>
#include <time.h>


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
            Texture2D LplayerA = LoadTexture("assets/LplayerA (1).png"); // Racket above head (for straight & sin shots)
            Texture2D LplayerU = LoadTexture("assets/LplayerU (2).png"); // Racket under body (for curve shot)

            // Load right player texture (for response – curve shot)
            Texture2D RplayerU = LoadTexture("assets/RplayerU.png");

            // Load shuttle textures
            Texture2D shuttleR = LoadTexture("assets/shuttleR.png"); // Ball for left-to-right shot
            Texture2D shuttleL = LoadTexture("assets/shuttleL.png"); // Ball for return (right-to-left) shot

            // Fixed positions for players
            Vector2 leftPlayerPos = {210, 330};    // Left player's fixed position
            Vector2 rightPlayerPos = {700, 400};     // Right player's fixed position

            // Define movement boundaries for players
            const float leftMin = 210.0f;
            const float leftMax = 560.0f;
            const float rightMin = 700.0f;
            const float rightMaxBound = 990.0f;

            // Initialize variables for left player's shot
            typedef enum { SHOT_NONE, SHOT_STRAIGHT, SHOT_SIN, SHOT_CURVE } ShotType;
            bool ballReturning = false;
            ShotType leftShotType = SHOT_NONE;
            bool shotFired = false;
            float shotTime = 0.0f;
            Vector2 ballPos = {0, 0};     // Current shuttle position
            Vector2 ballStart = {0, 0};   // Starting position for the shot


            // On-screen instructions
            const char* instructions = "For left shots: Z = Straight, H = Sin, C = Curve.\n(Straight & Sin use ABOVE image; Curve uses UNDER image)";

            // --- Difficulty multipliers based on the global difficulty level ---  
            // (Assuming 'difficulty' is a global variable of type DifficultyLevel set earlier)
            float ballSpeedMultiplier = 1.0f;
            float aiSpeedMultiplier = 1.0f;
            if (difficulty == EASY) {
                ballSpeedMultiplier = 0.8f;
                aiSpeedMultiplier = 0.8f;
            } else if (difficulty == MEDIUM) {
                ballSpeedMultiplier = 1.0f;
                aiSpeedMultiplier = 1.0f;
            } else if (difficulty == HARD) {
                ballSpeedMultiplier = 1.2f;
                aiSpeedMultiplier = 1.2f;
            }
            const float groundLevel = 750.0f; // Define ground level

            // Lower the ball's base speed for a more visible trajectory.
            float baseBallSpeed = 300.0f; // Reduced from 600

            clock_t start = clock();

            // Main game loop
            while (!WindowShouldClose()) {
                float dt = GetFrameTime();

                // --- Input Handling for Left Player Movement ---
                if (IsKeyDown(KEY_LEFT)) leftPlayerPos.x -= 5;
                if (IsKeyDown(KEY_RIGHT)) leftPlayerPos.x += 5;
                // Clamp left player's x-position
                if (leftPlayerPos.x < leftMin) leftPlayerPos.x = leftMin;
                if (leftPlayerPos.x > leftMax) leftPlayerPos.x = leftMax;

                // --- Right Player AI Movement (tracking the ball) ---
                float baseAISpeed = 3.0f;
                float aiSpeed = baseAISpeed * aiSpeedMultiplier;
                if (shotFired && !ballReturning) { // Only move if a shot is in progress.
                    if (rightPlayerPos.x + 50 < ballPos.x) rightPlayerPos.x += aiSpeed;
                    if (rightPlayerPos.x + 50 > ballPos.x) rightPlayerPos.x -= aiSpeed;
                }
                // Clamp right player's x-position
                if (rightPlayerPos.x < rightMin) rightPlayerPos.x = rightMin;
                if (rightPlayerPos.x > rightMaxBound) rightPlayerPos.x = rightMaxBound;

                // --- Input Handling for Shot Selection (only if no shot is in progress) ---
                if (!shotFired) {
                    // Press Z for straight, H for sin, C for curve shot.
                    if (IsKeyPressed(KEY_Z)) {
                        leftShotType = SHOT_STRAIGHT;
                        shotFired = true;
                        ballReturning = false;
                    } else if (IsKeyPressed(KEY_X)) {
                        leftShotType = SHOT_SIN;
                        shotFired = true;
                        ballReturning = false;
                    } else if (IsKeyPressed(KEY_C)) {
                        leftShotType = SHOT_CURVE;
                        shotFired = true;
                        ballReturning = false;
                    }
                    // Set the ball starting position relative to the left player's hand.
                    if (shotFired && !ballReturning) {
                        shotTime = 0.0f;
                        ballStart.x = leftPlayerPos.x + 100;  // Offset so the ball appears in front of the player.
                        ballStart.y = leftPlayerPos.y + 50;
                        ballPos = ballStart;
                    }
                }

                // --- Update Ball Movement if Shot Fired (Left-to-Right) ---
                if (shotFired && !ballReturning) {
                    shotTime += dt;
                    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
                    // ballPos.x = ballStart.x + ballSpeed * shotTime;   this part changed to assembly

                    // Inline assembly to compute ballPos.x = ballStart.x + ballSpeed * shotTime;
                    __asm__ volatile (
                        "flds %2;"         // Load ballSpeed into st(0)
                        "fmuls %3;"        // Multiply st(0) by shotTime
                        "flds %1;"         // Load ballStart.x (push previous value to st(1))
                        "faddp;"           // Add st(0) and st(1); result in st(0)
                        "fstps %0;"        // Store result into ballPos.x
                        : "=m" (ballPos.x)
                        : "m" (ballStart.x), "m" (ballSpeed), "m" (shotTime)
                        : "st"
                    );

                    // Update vertical displacement based on shot type.
                    if (leftShotType == SHOT_STRAIGHT) {
                        // Using inline assembly to compute:
                        // ballPos.y = ballStart.y + verticalSpeed * shotTime * ballSpeedMultiplier;
                        float verticalSpeed = 100.0f; // vertical drop speed
                        __asm__ volatile (
                            "flds %2\n\t"            // Load verticalSpeed into st(0)
                            "fmuls %3\n\t"           // Multiply st(0) by shotTime -> st(0) = verticalSpeed * shotTime
                            "flds %4\n\t"            // Load ballSpeedMultiplier into st(0), push previous value to st(1)
                            "fmulp %%st, %%st(1)\n\t" // Multiply st(1) by st(0) and pop: st(0) = verticalSpeed * shotTime * ballSpeedMultiplier
                            "flds %1\n\t"            // Load ballStart.y into st(0), push previous value to st(1)
                            "faddp %%st, %%st(1)\n\t" // Add st(0) (ballStart.y) to st(1) and pop: st(0) = ballStart.y + (verticalSpeed * shotTime * ballSpeedMultiplier)
                            "fstps %0\n\t"           // Store the result from st(0) into ballPos.y
                            : "=m" (ballPos.y)        // Output: store the result in ballPos.y
                            : "m" (ballStart.y),      // Input operand 1: ballStart.y
                            "m" (verticalSpeed),    // Input operand 2: verticalSpeed (100.0f)
                            "m" (shotTime),         // Input operand 3: shotTime
                            "m" (ballSpeedMultiplier) // Input operand 4: ballSpeedMultiplier
                            : "st"                   // Clobbered: FPU register
                        );
                    }

                    // else if (leftShotType == SHOT_SIN) {
                    //     // Sin shot: downward drift plus sine oscillation.
                    //     float verticalSpeed = 100.0f;
                    //     float frequency = 4.0f;
                    //     float amplitude = 50.0f;
                    //     ballPos.y = ballStart.y + verticalSpeed * shotTime * ballSpeedMultiplier + sin(shotTime * frequency) * amplitude;
                    // } 

                    // In the SHOT_SIN branch:
                    else if (leftShotType == SHOT_SIN) {
                        // Define constants
                        float verticalSpeed = 100.0f;  // vertical drop speed
                        float frequency = 4.0f;        // sine frequency
                        float amplitude = 50.0f;       // sine amplitude

                        // Compute the drift component using inline assembly:
                        float driftResult;
                        __asm__ volatile (
                            "flds %1\n\t"         // Load verticalSpeed into st(0)
                            "fmuls %2\n\t"        // Multiply st(0) by shotTime, now st(0) = verticalSpeed * shotTime
                            "fmuls %3\n\t"        // Multiply st(0) by ballSpeedMultiplier
                            "fstps %0\n\t"        // Store the result into driftResult and pop the stack
                            : "=m" (driftResult)
                            : "m" (verticalSpeed), "m" (shotTime), "m" (ballSpeedMultiplier)
                            : "st"
                        );

                        // Compute the sine component using FSIN in inline assembly:
                        float angle = shotTime * frequency;  // Compute the angle in radians in C
                        float sineResult;
                        __asm__ volatile (
                            "flds %1\n\t"         // Load 'angle' into st(0)
                            "fsin\n\t"            // Compute sin(angle), result in st(0)
                            "fmul %2\n\t"         // Multiply the result by 'amplitude'
                            "fstps %0\n\t"        // Store the result into sineResult and pop the stack
                            : "=m" (sineResult)
                            : "m" (angle), "m" (amplitude)
                            : "st"
                        );

                        // Combine the components with ballStart.y:
                        ballPos.y = ballStart.y + driftResult + sineResult;
                    }


                    // else if (leftShotType == SHOT_CURVE) {
                    //     float dx = ballPos.x - ballStart.x;
                    //     ballPos.y = ballStart.y + 0.001f * dx * dx;
                    // }
                    
                    
                    else if (leftShotType == SHOT_CURVE) {
                        // For SHOT_CURVE: Compute ballPos.y = ballStart.y + 0.001f * (ballPos.x - ballStart.x)²;
                        static const float factor = 0.001f;
                        __asm__ volatile (
                            "flds %1\n\t"            // Load ballPos.x into st(0)
                            "flds %2\n\t"            // Load ballStart.x into st(0), push ballPos.x to st(1)
                            "fsubrp %%st, %%st(1)\n\t"// Compute: st(0) = ballPos.x - ballStart.x (dx); pop st(1)
                            "fmul %%st, %%st\n\t"     // Square dx: st(0) = dx * dx
                            "flds %3\n\t"            // Load factor (0.001f) into st(0), push dx^2 to st(1)
                            "fmulp %%st, %%st(1)\n\t" // Multiply: st(0) = dx^2 * factor; pop st(1)
                            "flds %4\n\t"            // Load ballStart.y into st(0), push product to st(1)
                            "faddp %%st, %%st(1)\n\t" // Add: st(0) = ballStart.y + (dx^2*factor); pop st(1)
                            "fstps %0\n\t"           // Store st(0) into ballPos.y
                            : "=m" (ballPos.y)        // Output: result stored in ballPos.y
                            : "m" (ballPos.x),        // Operand %1: ballPos.x
                            "m" (ballStart.x),      // Operand %2: ballStart.x
                            "m" (factor),           // Operand %3: our factor (0.001f)
                            "m" (ballStart.y)       // Operand %4: ballStart.y
                            : "st"                   // Clobbered: FPU register stack
                        );
                    }

                    // net:
                    // float netX = 720.0f;
                    // float netY = 450.0f;
                    // float netThreshold = 10.0f;
                    // if (fabs(ballPos.x - netX) < netThreshold && ballPos.y >= netY) {
                    //     ballPos.y = netY - 10;
                    // }


                    // --- Check if ball is near the right player's hand to trigger return ---
                    float hitThreshold = 25.0f;  // Increased threshold for easier triggering
                    if (fabs(ballPos.x - (rightPlayerPos.x + 125)) < hitThreshold &&
                        fabs(ballPos.y - (rightPlayerPos.y + 150)) < hitThreshold) {
                        ballReturning = true;
                        shotTime = 0.0f;
                        // Set the ball starting position for the return shot relative to the right player's hand.
                        ballStart.x = rightPlayerPos.x + 125;  // Offset so it appears in the right player's hand.
                        ballStart.y = rightPlayerPos.y + 150;
                        ballPos = ballStart;
                        // Optionally, print debug info:
                        printf("Return triggered! ballPos: (%.2f, %.2f)\n", ballPos.x, ballPos.y);
                    }

                    // If the ball goes past the right boundary or reaches the ground, reset.
                    if (ballPos.x >= rightMaxBound || ballPos.y >= groundLevel) {
                        shotFired = false;
                        ballReturning = false;
                        ballPos.x = leftPlayerPos.x + 100;
                        ballPos.y = leftPlayerPos.y + 50;
                    }
                }

                // --- Update Ball Movement for Return Shot (Right-to-Left) ---
                if (shotFired && ballReturning) {
                    shotTime += dt;
                    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
                    // Ball now moves from right to left.
                    ballPos.x = ballStart.x - ballSpeed * shotTime;
                    // Use a parabolic curve for return.
                    float dx = ballStart.x - ballPos.x;
                    ballPos.y = ballStart.y + 0.001f * dx * dx;
                    // When the ball returns near the left player's hand, reset the shot.
                    if (ballPos.x <= leftPlayerPos.x + 100) {
                        shotFired = false;
                        ballReturning = false;
                        ballPos.x = leftPlayerPos.x + 100;
                        ballPos.y = leftPlayerPos.y + 50;
                    }
                }

                // --- Determine Left Player Pose ---
                // Use the ABOVE image for straight and sin shots; use the UNDER image for curve shot.
                Texture2D leftPlayerTexture;
                if (shotFired && !ballReturning) {
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
                    // When returning, use shuttleL; otherwise use shuttleR.
                    if (ballReturning)
                        DrawTexture(shuttleL, (int)ballPos.x, (int)ballPos.y, WHITE);
                    else
                        DrawTexture(shuttleR, (int)ballPos.x, (int)ballPos.y, WHITE);
                }

                // --- Display Score & On-screen Instructions ---
                DrawText(TextFormat("Score: %d", 0), 650, 50, 30, BLACK);
                DrawText(TextFormat("Highest Score: %d", 0), 1100, 50, 30, DARKGRAY);
                DrawText(instructions, 10, 750, 20, DARKBLUE);

                EndDrawing();
            }

            // End timing after the hot section
            clock_t end = clock();
            double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
            printf("Hot section time: %f seconds\n", elapsed);

            // --- Cleanup Resources ---
            UnloadTexture(bgGame);
            UnloadTexture(LplayerA);
            UnloadTexture(LplayerU);
            UnloadTexture(RplayerU);
            UnloadTexture(shuttleR);
            UnloadTexture(shuttleL);
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
