#ifndef RL_BLENDER_CAMERA_H
#define RL_BLENDER_CAMERA_H

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#define BLENDER_GREY (Color){135, 136, 136, 255}
#define BLENDER_DARK_GREY (Color){58, 58, 58, 255}
#define BLENDER_WIRE (Color){78, 78, 78, 255}


typedef struct BlenderCamera
{
    Camera camera;
    Vector2 previousMousePosition;
    bool isMouseDragging;
    float zoom;
    float zoomSpeed;
    float moveSpeed;
    float moveSpeedFast;
    float moveSpeedSlow;
    float panSpeed;
    float rotationSpeed;
    float freeFlyRotationSpeed;
    bool freeFly;
} BlenderCamera;

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

BlenderCamera CreateBlenderCamera() {
    BlenderCamera bcamera;
    BlenderCameraInit(&bcamera);
    return bcamera;
}

// Matrix implementation (has some issues with gimbal locking, not as full featured)
void BlenderCameraUpdateMAT(BlenderCamera *bcamera)
{

    // Move the camera with the scroll wheel
    float wheelMove = GetMouseWheelMove();
    Vector2 mousePosition = GetMousePosition();
    bool middleMouseDown = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

    if (middleMouseDown || wheelMove != 0)
    {
        Vector3 direction = Vector3Subtract(bcamera->camera.target, bcamera->camera.position);
        direction = Vector3Normalize(direction);
        bcamera->camera.position = Vector3Add(bcamera->camera.position, Vector3Scale(direction, wheelMove * bcamera->zoomSpeed));
    }

    // If the middle mouse is down, orbit the camera around the world origin or pan
    if (middleMouseDown)
    {
        Vector2 mouseDelta = Vector2Subtract(mousePosition, bcamera->previousMousePosition);

        // Invert the direction of rotation
        mouseDelta.x = -mouseDelta.x;
        mouseDelta.y = -mouseDelta.y;

        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            // Pan the camera
            Vector3 direction = Vector3Normalize(Vector3Subtract(bcamera->camera.target, bcamera->camera.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, bcamera->camera.up));
            Vector3 up = Vector3Normalize(Vector3CrossProduct(right, direction));

            // Invert the direction of panning up and down
            Vector3 pan = Vector3Add(Vector3Scale(right, mouseDelta.x * bcamera->panSpeed), Vector3Scale(up, -mouseDelta.y * bcamera->panSpeed));
            bcamera->camera.position = Vector3Add(bcamera->camera.position, pan);
            bcamera->camera.target = Vector3Add(bcamera->camera.target, pan);
        }
        else
        {
            // Rotate the camera
            bcamera->camera.position = Vector3Subtract(bcamera->camera.position, bcamera->camera.target);
            bcamera->camera.position = Vector3Transform(bcamera->camera.position, MatrixRotateY(mouseDelta.x * bcamera->rotationSpeed));
            bcamera->camera.position = Vector3Transform(bcamera->camera.position, MatrixRotateX(mouseDelta.y * bcamera->rotationSpeed));
            bcamera->camera.position = Vector3Add(bcamera->camera.position, bcamera->camera.target);
        }
    }

    bcamera->previousMousePosition = mousePosition; // Update previous mouse position regardless of whether the middle mouse button is down

    // If the P key is pressed, toggle between perspective and orthographic projection
    if (IsKeyPressed(KEY_P))
    {
        bcamera->camera.projection = (bcamera->camera.projection == CAMERA_PERSPECTIVE) ? CAMERA_ORTHOGRAPHIC : CAMERA_PERSPECTIVE;
    }
}

// Quaternion implementation (no gimbal locking, preferred, more features)
void BlenderCameraUpdate(BlenderCamera *bcamera)
{

    // Get inputs up front and some some calculations we'll need throughout
    float wheelMove = GetMouseWheelMove();
    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseDelta = Vector2Subtract(mousePosition, bcamera->previousMousePosition);
    bool middleMouseDown = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);
    bool leftShiftDown = IsKeyDown(KEY_LEFT_SHIFT);


    // If shift + F is pressed, toggle between free fly and orbit mode
    if (IsKeyPressed(KEY_F) && leftShiftDown)
    {
       bcamera->freeFly = !bcamera->freeFly;
    }

    // Camera movement for gimbal orbit mode
    if (!bcamera->freeFly)
    {

        Vector3 direction = Vector3Subtract(bcamera->camera.target, bcamera->camera.position);
        direction = Vector3Normalize(direction);

        if (middleMouseDown || wheelMove != 0)
        {
            bcamera->camera.position = Vector3Add(bcamera->camera.position, Vector3Scale(direction, wheelMove * bcamera->zoomSpeed));
        }

        // If the middle mouse is down, orbit the camera around the world origin or pan
        if (middleMouseDown)
        {
            // Invert the direction of rotation
            mouseDelta.x = -mouseDelta.x;
            mouseDelta.y = -mouseDelta.y;

            if (leftShiftDown)
            {

                // Pan the camera
                Vector3 direction = Vector3Normalize(Vector3Subtract(bcamera->camera.target, bcamera->camera.position));
                Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, bcamera->camera.up));
                Vector3 up = Vector3Normalize(Vector3CrossProduct(right, direction));

                // Invert the direction of panning up and down
                Vector3 pan = Vector3Add(Vector3Scale(right, mouseDelta.x * bcamera->panSpeed), Vector3Scale(up, -mouseDelta.y * bcamera->panSpeed));
                bcamera->camera.position = Vector3Add(bcamera->camera.position, pan);
                bcamera->camera.target = Vector3Add(bcamera->camera.target, pan);
            }
            else
            {
                // Rotate the camera
                Quaternion qYaw = QuaternionFromAxisAngle((Vector3){0.0f, 1.0f, 0.0f}, mouseDelta.x * bcamera->rotationSpeed);
                direction = Vector3Subtract(bcamera->camera.position, bcamera->camera.target);
                Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, bcamera->camera.up));
                Quaternion qPitch = QuaternionFromAxisAngle(right, -mouseDelta.y * bcamera->rotationSpeed);
                Quaternion q = QuaternionMultiply(qPitch, qYaw);

                Vector3 directionNew = Vector3Transform(direction, QuaternionToMatrix(q));
                bcamera->camera.position = Vector3Add(bcamera->camera.target, directionNew);
            }
        }

        bcamera->previousMousePosition = mousePosition;
    }

    // Camera movement for free fly mode. (There is some code duplication here, but it's easier to read this way)
    else
    {
        
        float flySpeed = bcamera->moveSpeed;

        // If LSHIFT is pressed, move faster
        if (leftShiftDown)
            flySpeed = bcamera->moveSpeedFast;
        
        // If LCTRL is pressed, move slower
        if (IsKeyDown(KEY_LEFT_CONTROL))
            flySpeed = bcamera->moveSpeedSlow;

        // Rotation
        Quaternion qYaw = QuaternionFromAxisAngle(bcamera->camera.up, -mouseDelta.x * bcamera->freeFlyRotationSpeed);
        Quaternion qPitch = QuaternionFromAxisAngle(Vector3CrossProduct(bcamera->camera.up, Vector3Normalize(Vector3Subtract(bcamera->camera.target, bcamera->camera.position))), mouseDelta.y * bcamera->freeFlyRotationSpeed);
        Quaternion q = QuaternionMultiply(qPitch, qYaw);

        // Position
        Vector3 forward = Vector3Normalize(Vector3Subtract(bcamera->camera.target, bcamera->camera.position));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(bcamera->camera.up, forward));

        if (IsKeyDown(KEY_W))
        {
            bcamera->camera.position = Vector3Add(bcamera->camera.position, Vector3Scale(forward, flySpeed));
        }
        else if (IsKeyDown(KEY_S))
        {
            bcamera->camera.position = Vector3Subtract(bcamera->camera.position, Vector3Scale(forward, flySpeed));
        }

        // Pan the camera left and right relative to the screen (inverted)
        if (IsKeyDown(KEY_A))
        {
            bcamera->camera.position = Vector3Add(bcamera->camera.position, Vector3Scale(right, flySpeed));
        }
        else if (IsKeyDown(KEY_D))
        {
            bcamera->camera.position = Vector3Subtract(bcamera->camera.position, Vector3Scale(right, flySpeed));
        }

        // Move up or down on Q/E keys relative to the screen (inverted)
        if (IsKeyDown(KEY_E))
        {
            bcamera->camera.position = Vector3Add(bcamera->camera.position, Vector3Scale(bcamera->camera.up, flySpeed));
        }
        else if (IsKeyDown(KEY_Q))
        {
            bcamera->camera.position = Vector3Subtract(bcamera->camera.position, Vector3Scale(bcamera->camera.up, flySpeed));
        }

        // Update target position
        Vector3 direction = Vector3Subtract(bcamera->camera.target, bcamera->camera.position);
        direction = Vector3Transform(direction, QuaternionToMatrix(q));
        bcamera->camera.target = Vector3Add(bcamera->camera.position, direction);

        bcamera->previousMousePosition = mousePosition;
    }
}


#endif