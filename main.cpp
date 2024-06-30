#include "raylib.h"
#include <iostream>

//Global variables
const int ScreenWidth = 800; 
const int ScreenHeight = 600;

//Platform class
class Platform {
    private:
        //Stores the platform's dimensions and position
        Rectangle platformRect;
        //Stores the platform color
        Color platformColor;
    public:
        //Initializes a new platform with the values you give it
        Platform(float l, float w, float x, float y, Color p) {
            platformRect.width = l;
            platformRect.height = w;
            platformRect.x = x;
            platformRect.y = y;
            platformColor = p;
        }

        //Draws the platform.
        void DrawPlatform() {
            DrawRectangleRec(platformRect, platformColor);
        }

        Rectangle GetRect() {
            //Lets other classes access the rectangle data
            return platformRect;
        }
};

//Player class, includes movement methods, positional data, etc.
class Player {
    private:
        //Stores the player's dimensions and position
        Rectangle playerRect;
        //Self-explanatory
        Color playerColor;
        //Stores the player's velocity, acceleration, and gravity
        float yVel = 0;
        float xVel = 0;
        float accel;
        float gravity;
        //Whether the player is in a state that they can jump
        bool canJump;
        //The rectangles which are used to detect what side the player is colliding with, stored in the boolean variables
        Rectangle leftCollider, rightCollider, topCollider, bottomCollider;
        bool lCollide, rCollide, tCollide, bCollide;
    public:
        //Initializes the player with the data you input, and sets up the colliders.
        Player(float s, float x, float y, Color p, float a, float g) {
            playerRect.width = playerRect.height = s;
            playerRect.x = x;
            playerRect.y = y;
            playerColor = p;

            leftCollider.width = rightCollider.width = topCollider.height = bottomCollider.height = 50;
            leftCollider.height = rightCollider.height = topCollider.width = bottomCollider.width = playerRect.width - 10;
            accel = a;
            gravity = g;
        }

        //Draws the player to the screen and deals with updates
        void DrawPlayer() {

            DrawRectangleRec(playerRect, playerColor);

            //Moves the colliders to the player position
            leftCollider.x = playerRect.x - leftCollider.width;
            rightCollider.x = playerRect.x + playerRect.width;
            leftCollider.y = rightCollider.y = playerRect.y + (playerRect.height - leftCollider.height)/2;
            topCollider.x = bottomCollider.x = playerRect.x + (playerRect.width - topCollider.width)/2;
            topCollider.y = playerRect.y - topCollider.height;
            bottomCollider.y = playerRect.y + playerRect.height;

            //If you turn this on it looks really goofy, but you'll get to see how this code is probably very stupid
            /*DrawRectangleRec(leftCollider, RED);
            DrawRectangleRec(rightCollider, ORANGE);
            DrawRectangleRec(topCollider, YELLOW);
            DrawRectangleRec(bottomCollider, BLUE);*/

            //As long as you're not touching the ground, enable gravity and turn off jumping
            if(!(playerRect.y >= ScreenHeight-playerRect.height)) {
                yVel += gravity;
                if(yVel > 40) {
                    yVel = 40;
                }
                canJump = false;
            //Otherwise, if you're in the ground and still moving, it'll move you back on the surface.
            } else if(yVel >= 0){
                playerRect.y = ScreenHeight - playerRect.height;
                canJump = true;
            }

            //Move the player based on the calculated velocities
            playerRect.x += xVel;
            playerRect.y += yVel;

        }

        //Checks what keys are pressed
        void GetMovements() {

            //If the keys are down, accelerate the player, otherwise slow them down
            if(IsKeyDown(KEY_LEFT) && xVel > -20) {
                xVel -= accel;
            } else if (IsKeyDown(KEY_RIGHT) && xVel < 20) {
                xVel += accel;
            } else if(xVel < 0) {
                xVel += accel;
            } else if (xVel > 0) {
                xVel -= accel;
            }
            //If up is pressed and the player's on the ground, jump
            if(IsKeyDown(KEY_UP) && canJump == true) {Jump();}

            /*if(IsKeyDown(KEY_ENTER)) {
                std::cout << playerRect.x << ", " << playerRect.y << std::endl;
            }*/

        }

        void Jump() {
            yVel = -20;
        }

        Rectangle GetRect() {
            return playerRect;
        }

        //This was difficult to make, it checks for collisions with the platforms.
        void CheckCollision(Rectangle otherRect) {

            //Checks what side you're being hit on based on the collider rectangles from earlier.
            (CheckCollisionRecs(leftCollider, otherRect)) ? lCollide = true : lCollide = false;
            (CheckCollisionRecs(rightCollider, otherRect)) ? rCollide = true : rCollide = false;
            (CheckCollisionRecs(topCollider, otherRect)) ? tCollide = true : tCollide = false;
            (CheckCollisionRecs(bottomCollider, otherRect)) ? bCollide = true : bCollide = false;

            //If the player goes to the top of the screen, stop them
            if(playerRect.y<0) {
                playerRect.y = 0; 
                yVel = gravity;
            //If the player is on the ground, set their velocity to 0
            } else if(playerRect.y>ScreenHeight-playerRect.height) {
                playerRect.y = ScreenHeight-playerRect.height; 
                yVel = 0;
            }
            //If the player hits the sides of the screen, keep them in the level
            if(playerRect.x<0) {
                playerRect.x = 0;
                xVel = 0;
            } else if(playerRect.x > ScreenWidth-playerRect.height) {
                playerRect.x = ScreenWidth-playerRect.height;
                xVel = 0;
            }

            //Based on what side the hit occurs on, it'll push the player out of the platform. There might be a more effective way to do this but whatever
            if(lCollide) {
                while(CheckCollisionRecs(playerRect, otherRect)) {xVel = 0; playerRect.x++;}
            } 
            if(rCollide) {
                while(CheckCollisionRecs(playerRect, otherRect)) {xVel = 0; playerRect.x--;}
            }
            if(tCollide) {
                while(CheckCollisionRecs(playerRect, otherRect)) {yVel = 0; playerRect.y++;}
            }
            if(bCollide) {
                while(CheckCollisionRecs(playerRect, otherRect)) {yVel = 0; playerRect.y--; canJump = true;}
            }

        }
};

int main() {

    //Initialize the OpenGL backend
    InitWindow(ScreenWidth, ScreenHeight, "Raylib Platformer");
    SetTargetFPS(60);

    //Set up platforms and the player sprite
    Platform platform1(300, 100, 200, 450, BLUE);
    Platform platform2(100, 250, 500, 300, BLUE);
    Platform platforms[] = {platform1, platform2};
    Player player1(50, 450, 350, GREEN, 2, 1);

    //Main loop
    while(!WindowShouldClose()) {

        //Draws sprites to the screen and checks collision
        BeginDrawing();
            ClearBackground(RAYWHITE);
            player1.DrawPlayer();
            for(int i = 0; i < sizeof(platforms)/sizeof(*platforms); i++) {
                platforms[i].DrawPlatform();
                player1.CheckCollision(platforms[i].GetRect());
            }
        EndDrawing();
        //Event handler
        player1.GetMovements();
    }

    //End OpenGL stuff
    CloseWindow();
    return 0;

}