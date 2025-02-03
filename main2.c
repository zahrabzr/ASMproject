#include "raylib.h"
#include <math.h>

// Define shot types
typedef enum ShotType {
    SHOT_NONE = 0,
    SHOT_SIN,      // For rocket above head (sinusoidal path)
    SHOT_STRAIGHT, // For rocket above head (straight path)
    SHOT_PARABOLIC // For rocket under body (y = (x - start)^2 path)
} ShotType;

// Define player poses
typedef enum PoseState {
    POSE_WAITING = 0,
    POSE_ROCKET_ABOVE,
    POSE_ROCKET_UNDER
} PoseState;

int main(void)
{
    // Initialization
    const int screenWidth = 1440;
    const int screenHeight = 810;
    InitWindow(screenWidth, screenHeight, "Badminton Game Animation");

    // Load player textures (assumed to be created by your AI generator)
    Texture2D playerWaiting = LoadTexture("assets/LplayerW.png");
    Texture2D playerAbove   = LoadTexture("assets/LplayerA.jpg");
    Texture2D playerUnder   = LoadTexture("assets/LplayerU.jpg");

    // Load a ball texture (shuttlecock image)
    Texture2D shuttle = LoadTexture("assets/shuttleR.png");

    // Starting positions
    Vector2 playerPos = {200, 600}; // Player's position
    Vector2 ballPos = {playerPos.x + 100, playerPos.y + 50}; // Start near the player's racket
    Vector2 ballStartPos = ballPos; // Save starting position for parabolic path

    // Define states
    PoseState pose = POSE_WAITING;
    ShotType shot = SHOT_NONE;
    bool shotFired = false;
    float shotTime = 0.0f;

    // Parameters for trajectories
    const float ballSpeed = 600.0f;       // pixels per second (for straight motion)
    const float sinAmplitude = 50.0f;     // amplitude for sinusoidal motion
    const float sinFrequency = 4.0f;      // frequency for sinusoidal motion
    const float parabolaScale = 0.001f;   // scale factor for parabolic curve

    SetTargetFPS(60);

    while (!WindowShouldClose())    // Main game loop
    {
        // Update inputs:
        // Change pose states (waiting, rocket above, rocket under)
        if (IsKeyPressed(KEY_W)) { 
            pose = POSE_WAITING;
            shot = SHOT_NONE;
            shotFired = false;
            ballPos = ballStartPos; // reset ball position
        }
        if (IsKeyPressed(KEY_A)) { 
            pose = POSE_ROCKET_ABOVE;
            shot = SHOT_NONE;
            shotFired = false;
            ballPos = ballStartPos;
        }
        if (IsKeyPressed(KEY_S)) { 
            pose = POSE_ROCKET_UNDER;
            shot = SHOT_NONE;
            shotFired = false;
            ballPos = ballStartPos;
        }

        // When in the "rocket above head" pose, choose between two shot types:
        if (pose == POSE_ROCKET_ABOVE) {
            if (IsKeyPressed(KEY_ONE)) { 
                shot = SHOT_SIN;
                shotFired = true;
                shotTime = 0.0f;
            }
            if (IsKeyPressed(KEY_TWO)) { 
                shot = SHOT_STRAIGHT;
                shotFired = true;
                shotTime = 0.0f;
            }
        }
        // When in the "rocket under body" pose, choose the parabolic shot
        if (pose == POSE_ROCKET_UNDER) {
            if (IsKeyPressed(KEY_THREE)) { 
                shot = SHOT_PARABOLIC;
                shotFired = true;
                shotTime = 0.0f;
            }
        }

        // Update ball position if a shot has been fired
        if (shotFired && shot != SHOT_NONE)
        {
            float dt = GetFrameTime();
            shotTime += dt;

            // All shots move horizontally to the right
            ballPos.x += ballSpeed * dt;

            // Adjust vertical position based on shot type
            if (shot == SHOT_SIN)
            {
                // Sinusoidal vertical movement
                ballPos.y = ballStartPos.y + sin(shotTime * sinFrequency) * sinAmplitude;
            }
            else if (shot == SHOT_STRAIGHT)
            {
                // No vertical change: straight line shot
                ballPos.y = ballStartPos.y;
            }
            else if (shot == SHOT_PARABOLIC)
            {
                // Parabolic curve: y = startY + c*(x - startX)^2
                float dx = ballPos.x - ballStartPos.x;
                ballPos.y = ballStartPos.y + parabolaScale * dx * dx;
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // (Optional) Draw a background or court here...
        DrawText("Press W: Waiting | A: Rocket Above | S: Rocket Under", 10, 10, 20, DARKGRAY);
        DrawText("While in Rocket Above: 1 = Sin Shot, 2 = Straight Shot", 10, 40, 20, DARKGRAY);
        DrawText("While in Rocket Under: 3 = Parabolic Shot", 10, 70, 20, DARKGRAY);

        // Draw player based on pose
        if (pose == POSE_WAITING)
            DrawTexture(playerWaiting, playerPos.x, playerPos.y, WHITE);
        else if (pose == POSE_ROCKET_ABOVE)
            DrawTexture(playerAbove, playerPos.x, playerPos.y, WHITE);
        else if (pose == POSE_ROCKET_UNDER)
            DrawTexture(playerUnder, playerPos.x, playerPos.y, WHITE);

        // Draw the shuttlecock (ball)
        DrawTexture(shuttle, (int)ballPos.x, (int)ballPos.y, WHITE);

        // For debugging: display current state
        DrawText(TextFormat("Pose: %d  Shot: %d", pose, shot), 10, screenHeight - 30, 20, BLACK);

        EndDrawing();
    }

    // Cleanup textures
    UnloadTexture(playerWaiting);
    UnloadTexture(playerAbove);
    UnloadTexture(playerUnder);
    UnloadTexture(shuttle);

    CloseWindow();
    return 0;
}
