#include <iostream>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <sstream>
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
    
    std::string myMap; // map full of walls, no empty spaces there
    // todo - create empty spaces
    
    

    myMap += "################";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#.......##.....#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "#.........######";
    myMap += "#..............#";
    myMap += "#..............#";
    myMap += "################";
    






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
            if (myMap[((int)player.y * MAP_SIDE + (int)player.x)] == '#')
            {
                player.x -= sinf(player.angle) * (5.0f) * countedStepTime;
                player.y -= cosf(player.angle) * (5.0f) * countedStepTime;
            }
        }
        if (GetAsyncKeyState((unsigned short)'S') && 0x8000)
        {
            player.x -= sinf(player.angle) * (5.0f) * countedStepTime;
            player.y -= cosf(player.angle) * (5.0f) * countedStepTime;
            if (myMap[((int)player.y * MAP_SIDE + (int)player.x)] == '#')
            {
                player.x += sinf(player.angle) * (5.0f) * countedStepTime;
                player.y += cosf(player.angle) * (5.0f) * countedStepTime;
            }
        }
        //lets find distances for each colum on screen(from player)
        for (int i = 0; i < SCREEN_WIDTH; ++i)
        {
            float rayAngle = (player.angle - (player.fieldOfView / 2.0)) + 
                ((float)i / (float)SCREEN_WIDTH) * player.fieldOfView;
            float distanceToWall = 0;

            bool isHitAWall = false;
            bool isBoundary = false;
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

                        std::vector<std::pair<float, float>> p;
                        for (int tx = 0; tx < 2; ++tx)
                        {
                            for (int ty = 0; ty < 2; ++ty)
                            {
                                float vy = (float)testY + ty - player.y;
                                float vx = (float)testX + tx - player.x;
                                float d = sqrt(vy * vy + vx * vx);
                                float dot = ((eyeX * vx / d) + eyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }
                            std::sort(p.begin(), p.end(), [](const std::pair<float, float>& left,
                                const std::pair<float, float>& right)
                                {return left.first < right.first; });
                            float bound = 0.01;
                            if (acos(p.at(0).second) < bound) 
                            { 
                                isBoundary = true; 
                            }
                            if (acos(p.at(1).second) < bound)
                            {
                                isBoundary = true;
                            }
                            //if (acos(p.at(2).second) < bound)isBoundary = true;
                        }
                        
                    }
                }
            }

            //calculate distance to ceiling and floor
            int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / distanceToWall;
            int floor = SCREEN_HEIGHT - ceiling;

            short shade = ' ';

            

        


            for (int j = 0; j < SCREEN_HEIGHT; ++j)
            {
                if (j < ceiling)
                {
                    screen[j * SCREEN_WIDTH + i] = ' ';
                }
                else if(j > ceiling && j <=floor)
                {
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
                    else if (distanceToWall <= MAP_SIDE)
                    {
                        shade = '.';
                    }
                    else
                    {
                        shade = ' ';
                    }
                    if (isBoundary)
                    {
                        shade = ' ';
                    }


                    screen[j * SCREEN_WIDTH + i] = shade;
                }
                else
                {
                    float b = 1.0f - (((float)j - SCREEN_HEIGHT / 2.0f) / (SCREEN_HEIGHT / 2.0f));
                    if (b < 0.25)
                    {
                        shade = '#';
                    }
                    else if (b < 0.5)
                    {
                        shade = 'x';
                    }
                    else if (b < 0.75)
                    {
                        shade = '.';
                    }
                    else if (b < 0.9)
                    {
                        shade = '_';
                    }
                    else
                    {
                        shade = ' ';
                    }
                    screen[j * SCREEN_WIDTH + i] = shade;
                }
            }


        }

        //swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f ", player.x, player.y, player.angle, 1.0f / countedStepTime);
        std::stringstream tmp;
        tmp << "X=" << player.x << ", Y=" << player.y << ", A=" << player.angle <<
            ", FPS=" << 1.0f / countedStepTime;
        std::string tmp2;
        tmp2 = tmp.str();
        for (int k = 0; k < tmp2.size(); ++k)
        {
            screen[k] = tmp2[k];
        }
        

        for (int nx = 0; nx < MAP_SIDE; ++nx)
        {
            for (int ny = 0; ny < MAP_SIDE; ++ny)
            {
                screen[(ny + 1) * SCREEN_WIDTH + nx] = (myMap[ny * MAP_SIDE + nx]);
            }
        }

        screen[(int)(((int)player.y + 1) * SCREEN_WIDTH + player.x)] = 'P';

        //some paints
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, 
            { 0,0 }, & dwBytesWritten);


        
    }

    return 0;




    
}

