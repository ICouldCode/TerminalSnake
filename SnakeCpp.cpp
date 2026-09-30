#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <conio.h>
#include <random>
#include <thread>
#include <chrono>

struct Snake
{
    int posX = 5;
    int posY = 5;

    int lastX = 0;
    int lastY = 0;
};

struct Apple
{
    int posX = 1;
    int posY = 1;
};

void drawSnake(std::vector<Snake*> &snake);
void hideCursor();
void updateSnakeSegmentsPos(std::vector<Snake*>& snake);
void clearTerminal(std::vector<Snake*>& snake);
void drawApples(std::vector<Apple> &apples);
void spawnApples(std::vector<Snake*>& snake, std::vector<Apple>& apples, int maxApples);
void setCursorPosition(int x, int y);
void moveSnake(std::vector<Snake*>& snake, char last_key_pressed);
void draw(std::string title);

int getMaxX();
int getMaxY();

bool detectCollisions(std::vector<Apple>& apples, Snake& head);
bool snakeAtPos(std::vector<Snake*> &snake, int x, int y);
bool detectSnakeCollision(std::vector<Snake*>& snake);

char handleInput(Snake& origin, std::vector<Snake*>& snake, char last_key_pressed);

bool running = true;
bool gameOver = false;
bool win = false;

int maxX = getMaxX();
int maxY = getMaxY();

int Score = 0;

Snake* incrementBody(std::vector<Snake*>& snake);

std::random_device rd;
std::mt19937 gen(rd());

int main()
{
    std::clock_t prevTime = std::clock();
    

    std::vector<Snake*> snake;
    std::vector<Apple> apples;

    Snake origin;
    Snake origin_v;
    snake.push_back(&origin);
    snake.push_back(&origin_v);

    hideCursor();

    char last_key_pressed = 'd';
    
    while (running)
    {
        std::clock_t currTime = std::clock();
        double deltaTime = double(currTime - prevTime) / CLOCKS_PER_SEC;
        prevTime = currTime;
        
        for (volatile int i = 0; i < 10000000; ++i)
        {
            last_key_pressed = handleInput(*snake.front(), snake, last_key_pressed);
            moveSnake(snake, last_key_pressed);
            updateSnakeSegmentsPos(snake);
            drawSnake(snake);
            if (detectCollisions(apples, *snake.front()))
            {
                Snake* new_seg = incrementBody(snake);
                if (new_seg != nullptr) snake.push_back(new_seg);
                Score += (maxX + 1) * (maxY + 1) / 100;
            }
            if (detectSnakeCollision(snake))
            {
                running = false;
                gameOver = true;
            }
            spawnApples(snake,apples, 10);
            drawApples(apples);
            clearTerminal(snake);

            if (snake.size() == (maxX + 1) * (maxY + 1))
            {
                win = true;
                running = false;
            }

            if (!running) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
     
    }

    system("cls");

    if (gameOver)
    {
        draw("GAME OVER");
    }

    if (win)
    {
        draw("YOU WON");
    }

    return 0;
}

void draw(std::string Ttitle)
{
    setCursorPosition(0, 0);
    std::cout << "Score: " << Score;

    std::string title = Ttitle;
    std::string prompt = "Press Q to quit";

    int titleX = (maxX - static_cast<int>(title.size())) / 2;
    int promptX = (maxX - static_cast<int>(prompt.size())) / 2;
    int centerY = maxY / 2;

    setCursorPosition(titleX, centerY);
    std::cout << title;

    setCursorPosition(promptX, centerY + 2);
    std::cout << prompt;
}

void setCursorPosition(int x, int y)
{
    COORD pos;
    pos.X = x;
    pos.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

void drawApples(std::vector<Apple> &apples)
{
    for (auto& apple : apples) {
        setCursorPosition(apple.posX, apple.posY);
        std::cout << "*";
    }
}

void drawSnake(std::vector<Snake*> &snake) {
    for (auto& segment : snake){
        setCursorPosition(segment->posX, segment->posY);
        std::cout << "#";
    }
}

bool detectCollisions(std::vector<Apple>& apples, Snake& head)
{
    for (auto it = apples.begin(); it != apples.end();)
    {
        if (head.posX == it->posX && head.posY == it->posY)
        {
            it = apples.erase(it);
            return true;
        }
        else
        {
            ++it;
        }
    }

    return false;
}

bool detectSnakeCollision(std::vector<Snake*>& snake)
{
    Snake* head = snake.front();
    for (auto& seg : snake)
    {
        if (seg == head) continue;

        if (seg->posX == head->posX && seg->posY == head->posY) return true;
    }

    return false;
}

Snake* incrementBody(std::vector<Snake*>& snake)
{
    if (snake.size() >= maxX * maxY)
        return nullptr;

    Snake* last_seg = snake.back();

    Snake* new_seg = new Snake();

    new_seg->posX = last_seg->lastX;
    new_seg->posY = last_seg->lastY;

    return new_seg;
}

void updateSnakeSegmentsPos(std::vector<Snake*>& snake)
{

    for (std::size_t i = 0; i < snake.size() - 1; ++i)
    {
        Snake* currSeg = snake.at(i);
        Snake* nextSeg = snake.at(i + 1);

        nextSeg->lastX = nextSeg->posX;
        nextSeg->lastY = nextSeg->posY;

        nextSeg->posX = currSeg->lastX;
        nextSeg->posY = currSeg->lastY;
    }
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    if (GetConsoleCursorInfo(consoleHandle, &cursorInfo)) {
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }
}

bool snakeAtPos(std::vector<Snake*> &snake, int x, int y)
{
    for (auto& seg : snake)
    {
        if (seg->posX == x && seg->posY == y) return true;
    }
    return false;
}

void spawnApples(std::vector<Snake*>& snake, std::vector<Apple>& apples, int maxApples)
{
    if (apples.size() >= maxApples) return;

    for (int i = 0; i < maxApples - apples.size(); ++i)
    {
        std::uniform_int_distribution<> disX(0, maxX);
        std::uniform_int_distribution<> disY(0, maxY);

        int ranX = disX(gen);
        int ranY = disY(gen);

        if (snakeAtPos(snake, ranX, ranY)) continue;

        Apple apple;
        apple.posX = ranX;
        apple.posY = ranY;

        apples.push_back(apple);
    }
}

char handleInput(Snake& origin, std::vector<Snake*>& snake, char last_key_pressed)
{
    origin.lastY = origin.posY;
    origin.lastX = origin.posX;

    if (_kbhit()) {
        char ch = _getch();

        if (ch == 'q') running = false;

        //if (ch == 'w' && origin.posY > 0)
        //{     
        //    origin.posY-=1;
        //}
        //if (ch == 's' && origin.posY < maxY)
        //{
        //    origin.posY += 1;
        //}
        //if (ch == 'a' && origin.posX > 0)
        //{
        //    origin.posX -= 1;
        //}
        //if (ch == 'd' && origin.posX < maxX)
        //{
        //    origin.posX += 1;
        //}

        if (ch == 'w' && last_key_pressed != 's' ||
            ch == 'a' && last_key_pressed != 'd' ||
            ch == 's' && last_key_pressed != 'w' ||
            ch == 'd' && last_key_pressed != 'a' )
        {
            return ch;
        }
    }

    return last_key_pressed;
}

void moveSnake(std::vector<Snake*>& snake, char last_key_pressed)
{
    if (last_key_pressed == 'w')
    {
        if (snake.front()->posY <= 0)
        {
            snake.front()->posY = maxY;           
        }
        else 
        {
            snake.front()->posY -= 1;
        } 
    }
    else if (last_key_pressed == 's')
    {
        if (snake.front()->posY >= maxY)
        {
            snake.front()->posY = 0;
        }
        else
        {
            snake.front()->posY += 1;
        }
    }
    else if (last_key_pressed == 'a')
    {
        if (snake.front()->posX <= 0)
        {
            snake.front()->posX = maxX;
        }
        else
        {
            snake.front()->posX -= 1;
        }
    }
    else if (last_key_pressed == 'd')
    {
        if (snake.front()->posX >= maxX)
        {
            snake.front()->posX = 0;
        }
        else 
        {
            snake.front()->posX += 1;
        }      
    }
}

int getMaxX()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) 
    {
        return csbi.srWindow.Right - csbi.srWindow.Left;
    }
    
    return 0;
}

int getMaxY()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {    
        return csbi.srWindow.Bottom - csbi.srWindow.Top;
    }

    return 0;
}

void clearTerminal(std::vector<Snake*>& snake)
{
    //system("cls");

    Snake* tail = snake.back();

    int oldTailX = tail->posX;
    int oldTailY = tail->posY;

    setCursorPosition(oldTailX, oldTailY);
    std::cout << ' ';
}