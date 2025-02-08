#include <stdio.h>
#include <math.h>
#include <windows.h>  // For QueryPerformanceCounter

// Define global variables
float ballStartX = 0.0f, ballStartY = 0.0f, ballPosX = 0.0f, ballPosY = 0.0f;
float shotTime = 0.0f, ballSpeedMultiplier = 1.0f, baseBallSpeed = 10.0f;
int leftShotType;

// Define shot types
#define SHOT_STRAIGHT 1
#define SHOT_SIN 2
#define SHOT_CURVE 3

// Function to simulate Shot - Straight (in C)
void simulateShotC_Straight() {
    shotTime += 0.016f;  // Assuming dt = 0.016s for each frame
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
    ballPosX = ballStartX + ballSpeed * shotTime;

    // Update vertical displacement for SHOT_STRAIGHT (C version)
    if (leftShotType == SHOT_STRAIGHT) {
        float verticalSpeed = 100.0f;
        ballPosY = ballStartY + verticalSpeed * shotTime * ballSpeedMultiplier;
    }
}

// Function to simulate Shot - Straight (using inline assembly)
void simulateShotASM_Straight() {
    shotTime += 0.016f;  // Assuming dt = 0.016s for each frame
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;

    // Inline assembly to compute ballPos.x = ballStart.x + ballSpeed * shotTime
    __asm__ volatile (
        "flds %2;"         // Load ballSpeed into st(0)
        "fmuls %3;"        // Multiply st(0) by shotTime
        "flds %1;"         // Load ballStart.x (push previous value to st(1))
        "faddp;"           // Add st(0) and st(1); result in st(0)
        "fstps %0;"        // Store result into ballPos.x
        : "=m" (ballPosX)
        : "m" (ballStartX), "m" (ballSpeed), "m" (shotTime)
        : "st"
    );

    // Update vertical displacement for SHOT_STRAIGHT (Assembly version)
    if (leftShotType == SHOT_STRAIGHT) {
        float verticalSpeed = 100.0f;
        __asm__ volatile (
            "flds %2\n\t"
            "fmuls %3\n\t"
            "flds %4\n\t"
            "fmulp %%st, %%st(1)\n\t"
            "flds %1\n\t"
            "faddp %%st, %%st(1)\n\t"
            "fstps %0\n\t"
            : "=m" (ballPosY)
            : "m" (ballStartY), "m" (verticalSpeed), "m" (shotTime), "m" (ballSpeedMultiplier)
            : "st"
        );
    }
}

// Function to measure execution time for C vs Assembly version of Shot - Straight
void measureExecutionTime_Straight() {
    LARGE_INTEGER start, end, frequency;
    double cpu_time_used;

    // Get the frequency of the performance counter
    QueryPerformanceFrequency(&frequency);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for C version of simulateShotStraight
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotC_Straight();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for C version of SHOT_STRAIGHT: %f seconds\n", cpu_time_used);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for ASM version of simulateShotStraight
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotASM_Straight();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for ASM version of SHOT_STRAIGHT: %f seconds\n", cpu_time_used);
}

// Function to simulate Shot - Sin (in C)
void simulateShotC_Sin() {
    shotTime += 0.016f;
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
    ballPosX = ballStartX + ballSpeed * shotTime;

    // Sin shot: downward drift plus sine oscillation.
    if (leftShotType == SHOT_SIN) {
        float verticalSpeed = 100.0f;
        float frequency = 4.0f;
        float amplitude = 50.0f;
        ballPosY = ballStartY + verticalSpeed * shotTime * ballSpeedMultiplier + sin(shotTime * frequency) * amplitude;
    }
}

// Function to simulate Shot - Sin (using inline assembly)
void simulateShotASM_Sin() {
    shotTime += 0.016f;
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
    ballPosX = ballStartX + ballSpeed * shotTime;

    // Sin shot (Assembly version)
    if (leftShotType == SHOT_SIN) {
        float verticalSpeed = 100.0f;
        float frequency = 4.0f;
        float amplitude = 50.0f;

        // Compute the drift component using inline assembly
        float driftResult;
        __asm__ volatile (
            "flds %1\n\t"
            "fmuls %2\n\t"
            "fmuls %3\n\t"
            "fstps %0\n\t"
            : "=m" (driftResult)
            : "m" (verticalSpeed), "m" (shotTime), "m" (ballSpeedMultiplier)
            : "st"
        );

        // Compute the sine component using FSIN in inline assembly
        float angle = shotTime * frequency;
        float sineResult;
        __asm__ volatile (
            "flds %1\n\t"
            "fsin\n\t"
            "fmul %2\n\t"
            "fstps %0\n\t"
            : "=m" (sineResult)
            : "m" (angle), "m" (amplitude)
            : "st"
        );

        // Combine the components with ballStart.y
        ballPosY = ballStartY + driftResult + sineResult;
    }
}

// Function to measure execution time for C vs Assembly version of Shot - Sin
void measureExecutionTime_Sin() {
    LARGE_INTEGER start, end, frequency;
    double cpu_time_used;

    // Get the frequency of the performance counter
    QueryPerformanceFrequency(&frequency);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for C version of simulateShotSin
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotC_Sin();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for C version of SHOT_SIN: %f seconds\n", cpu_time_used);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for ASM version of simulateShotSin
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotASM_Sin();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for ASM version of SHOT_SIN: %f seconds\n", cpu_time_used);
}

// Function to simulate Shot - Curve (in C)
void simulateShotC_Curve() {
    shotTime += 0.016f;
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
    ballPosX = ballStartX + ballSpeed * shotTime;

    // Curve shot (C version)
    if (leftShotType == SHOT_CURVE) {
        float dx = ballPosX - ballStartX;
        ballPosY = ballStartY + 0.001f * dx * dx;
    }
}

// Function to simulate Shot - Curve (using inline assembly)
void simulateShotASM_Curve() {
    shotTime += 0.016f;
    float ballSpeed = baseBallSpeed * ballSpeedMultiplier;
    ballPosX = ballStartX + ballSpeed * shotTime;

    // Curve shot (Assembly version)
    if (leftShotType == SHOT_CURVE) {
        static const float factor = 0.001f;
        __asm__ volatile (
            "flds %1\n\t"
            "flds %2\n\t"
            "fsubrp %%st, %%st(1)\n\t"
            "fmul %%st, %%st\n\t"
            "flds %3\n\t"
            "fmulp %%st, %%st(1)\n\t"
            "flds %4\n\t"
            "faddp %%st, %%st(1)\n\t"
            "fstps %0\n\t"
            : "=m" (ballPosY)
            : "m" (ballPosX), "m" (ballStartX), "m" (factor), "m" (ballStartY)
            : "st"
        );
    }
}

// Function to measure execution time for C vs Assembly version of Shot - Curve
void measureExecutionTime_Curve() {
    LARGE_INTEGER start, end, frequency;
    double cpu_time_used;

    // Get the frequency of the performance counter
    QueryPerformanceFrequency(&frequency);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for C version of simulateShotCurve
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotC_Curve();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for C version of SHOT_CURVE: %f seconds\n", cpu_time_used);

    // Reset shotTime before starting the simulation
    shotTime = 0.0f;

    // Measure time for ASM version of simulateShotCurve
    QueryPerformanceCounter(&start);
    for (int i = 0; i < 50000; i++) {  // Increased iterations
        simulateShotASM_Curve();
    }
    QueryPerformanceCounter(&end);
    cpu_time_used = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time for ASM version of SHOT_CURVE: %f seconds\n", cpu_time_used);
}

int main() {
    // Initialize ball starting position
    ballStartX = 0.0f;
    ballStartY = 0.0f;

    // Set shot type and measure execution time for all shots
    leftShotType = SHOT_STRAIGHT;
    measureExecutionTime_Straight();

    leftShotType = SHOT_SIN;
    measureExecutionTime_Sin();

    leftShotType = SHOT_CURVE;
    measureExecutionTime_Curve();

    return 0;
}