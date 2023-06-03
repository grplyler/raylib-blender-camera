#include "raylib.h"
#include "rcamera_blender.h"
#include "rlgl.h"

// Utility to draw a grid with colors like blender
static void DrawGridEx(int slices, float spacing)
{
    int halfSlices = slices / 2;

    rlBegin(RL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        if (i == 0)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
            rlColor3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rlColor3f(0.3f, 0.3f, 0.3f);
            rlColor3f(0.3f, 0.3f, 0.3f);
            rlColor3f(0.3f, 0.3f, 0.3f);
            rlColor3f(0.3f, 0.3f, 0.3f);
        }

        rlVertex3f((float)i * spacing, 0.0f, (float)-halfSlices * spacing);
        rlVertex3f((float)i * spacing, 0.0f, (float)halfSlices * spacing);

        rlVertex3f((float)-halfSlices * spacing, 0.0f, (float)i * spacing);
        rlVertex3f((float)halfSlices * spacing, 0.0f, (float)i * spacing);
    }
    rlEnd();
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Blender Camera");

    // Initialize the camera
    BlenderCamera bcam = CreateBlenderCamera();

    SetTargetFPS(60);
    DisableCursor();

    // Define the cube position
    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

    while (!WindowShouldClose())
    {
        // Update the camera
        BlenderCameraUpdate(&bcam);


        BeginDrawing();
            ClearBackground(BLENDER_DARK_GREY);

            BeginMode3D(bcam.camera);

                DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, BLENDER_GREY);
                DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, ORANGE);

                DrawGridEx(20, 1.0f);

            EndMode3D();

            if (bcam.freeFly)
            {
                DrawText("Blender Camera Mode: FREE_FLY", 10, 10, 20, BLENDER_GREY);
            }
            else
            {
                DrawText("Blender Camera Mode: GIMBAL_ORBIT", 10, 10, 20, BLENDER_GREY);
            }


            DrawFPS(10, screenHeight - 30);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
