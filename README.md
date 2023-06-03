# A Blender Style Camera Implemenation for [Raylib](https://github.com/raysan5/raylib)

This project provides a blender style camera (with both orbit and fly modes) that is header only, and can be used in **3 lines of code**!

**Supported Modes/Features**

1. Gimbal Orbit (Pan, Zoom, Orbit Rotate)
2. Free Fly Mode (WASD + QE with normal, fast, and slow mode)


## Keyboard Controls

_Free Fly Mode_

- `LSHIFT + F` to toggle free fly mode
- `WASD` to move
- `QE` to move up and down

_Gimbal Orbit Mode_

- `MIDDLE MOUSE MOVE` Orbit
- `LSHIFT + MIDDLE MOUSE` Pan
- `SCROLL WHEEL` Zoom

## Usage with Raylib

Here's how you can use `BlenderCamera` with Raylib.

```c
#include "raylib.h"
#include "rcamera_blender.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Blender Camera");

    // Initialize the camera (Line 1 of 2!)
    BlenderCamera bcam = CreateBlenderCamera();

    // VSYNC and Hide Cursor
    SetTargetFPS(60);
    DisableCursor();

    // Define the cube position
    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

    while (!WindowShouldClose())
    {
        // Update the camera (Line 2 of 3)
        BlenderCameraUpdate(&bcam);

        BeginDrawing();
            ClearBackground(BLENDER_DARK_GREY);

            // Use the camera in 3D mode (Line 3 of 3!)
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

```

## Changing Camera Options


Here are the default settings from `CreateBlenderCamera()` which you can modify on the instatiated struct.

```c
void BlenderCameraInit(BlenderCamera *bcamera)
{
    bcamera->camera = (Camera){0};
    bcamera->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
    bcamera->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    bcamera->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    bcamera->camera.fovy = 45.0f;
    bcamera->camera.projection = CAMERA_PERSPECTIVE;
    bcamera->previousMousePosition = (Vector2){0};
    bcamera->isMouseDragging = false;
    bcamera->zoomSpeed = 0.3f;
    bcamera->moveSpeed = 0.2f;
    bcamera->moveSpeedFast = 0.4f;
    bcamera->moveSpeedSlow = 0.1f;
    bcamera->panSpeed = 0.01f;
    bcamera->rotationSpeed = 0.01f;
    bcamera->freeFlyRotationSpeed = 0.001f;
    bcamera->freeFly = true;
}
```

Like this:

```c
BlenderCamera bcam = CreateBlenderCamera();
bcam.camera.fovy = 70.0f;
```

## Todo

- [ ] Possibly integrate into `raylib-extras` repo.
- [ ] Add Orthographic Support
- [ ] Add orbit around selection option
