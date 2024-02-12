#include <iostream>
#include <Windows.h>
#include <chrono>

// main task - create a first point of view 3d text graphics ( some kind of maze, that we can walk through)


int main()
{
    //consts
    const int SCREEN_WIDTH = 120;
    const int SCREEN_HEIGHT = 40;



    //we need a person
    
    
    struct Player
    {
        float x = 5;
        float y = 5;
        float angle = 0;// 0 - top, clockwise
        const float fieldOfView = 3.1415 / 4.0;
    };

    Player player;



    // a map
    // # - wall
    // . - empty
    // P - player
    const int MAP_SIDE = 16;
    
    char myMap[MAP_SIDE*MAP_SIDE] { 35 }; // map full of walls, no empty spaces there
    // todo - create empty spaces
    for (int i = 0; i < MAP_SIDE * MAP_SIDE ; ++i)
    {
        
            myMap[i] = 35;
        
    }
    for (int i = 1; i < MAP_SIDE - 1; ++i)
    {
        for (int j = 1; j < MAP_SIDE - 1; ++j)
        {
            myMap[i* MAP_SIDE + j] = '.';
        }
    }




    //create screen buffer
    wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT] {0};
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    //game loop

    while (true)
    {
        //chrono stuff
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float countedStepTime = elapsedTime.count();

 
        //control
        if (GetAsyncKeyState((unsigned short)'A') && 0x8000)
        {
            player.angle -= (0.8f) * countedStepTime;
        }
        if (GetAsyncKeyState((unsigned short)'D') && 0x8000)
        {
            player.angle += (0.8f) * countedStepTime;
        }
        if (GetAsyncKeyState((unsigned short)'W') && 0x8000)
        {
            player.x += sinf(player.angle) * (5.0f) * countedStepTime;
            player.y += cosf(player.angle) * (5.0f) * countedStepTime;
        }
        if (GetAsyncKeyState((unsigned short)'S') && 0x8000)
        {
            player.x -= sinf(player.angle) * (5.0f) * countedStepTime;
            player.y -= cosf(player.angle) * (5.0f) * countedStepTime;
        }
        //lets find distances for each colum on screen(from player)
        for (int i = 0; i < SCREEN_WIDTH; ++i)
        {
            float rayAngle = (player.angle - (player.fieldOfView / 2.0)) + 
                ((float)i / (float)SCREEN_WIDTH) * player.fieldOfView;
            float distanceToWall = 0;

            bool isHitAWall = false;
            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            while (!isHitAWall && distanceToWall <  MAP_SIDE)
            {
                distanceToWall += 0.1;

                int testX = (int)(player.x + eyeX * distanceToWall);
                int testY = (int)(player.y + eyeY * distanceToWall);
                if (testX < 0 || testX >= MAP_SIDE ||
                    testY < 0 || testY >= MAP_SIDE)
                {
                    isHitAWall = true;
                    distanceToWall = (float)MAP_SIDE;
                }
                else
                {
                    if (myMap[testY * MAP_SIDE + testX] == '#')
                    {
                        isHitAWall = true;
                    }
                }
            }

            //calculate distance to ceiling and floor
            int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / distanceToWall;
            int floor = SCREEN_HEIGHT - ceiling;

            short shade = ' ';

            if (distanceToWall <= MAP_SIDE / 4.0)
            {
                shade = 'W';
            }
            else if (distanceToWall <= MAP_SIDE / 3.0)
            {
                shade = 'z';
            }
            else if (distanceToWall <= MAP_SIDE / 2.0)
            {
                shade = 178;
            }
            else if (distanceToWall <= MAP_SIDE )
            {
                shade = '.';
            }
            else
            {
                shade = ' ';
            }


        


            for (int j = 0; j < SCREEN_HEIGHT; ++j)
            {
                if (j < ceiling)
                {
                    screen[j * SCREEN_WIDTH + i] = ' ';
                }
                else if(j > ceiling && j <=floor)
                {
                    screen[j * SCREEN_WIDTH + i] = shade;
                }
                else
                {
                    screen[j * SCREEN_WIDTH + i] = ' ';
                }
            }


        }



        //some paints
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, 
            { 0,0 }, & dwBytesWritten);


        
    }

    return 0;




    
}

