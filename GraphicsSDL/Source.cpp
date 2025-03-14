

#define SDL_MAIN_USE_CALLBACKS 0  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
#include <iostream>

void BresenhamLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        SDL_RenderPoint(renderer, x1, y1);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

//

void Bersenhamtriangle(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    BresenhamLine(renderer, x1, y1, x2, y2);
    BresenhamLine(renderer, x2, y2, x3, y3);
    BresenhamLine(renderer, x3, y3, x1, y1);
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    bool Running = true;

    if (SDL_CreateWindowAndRenderer("Example1", 640, 480, 0, &window, &renderer) != 0)
    {
        std::cerr << "Error at creating window and renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    while (Running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                Running = false;
            }
        }

        double now = (double)SDL_GetTicks() / 1000.0;
        float red = (float)(0.5 + 0.5 * SDL_sin(now));
        float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
        float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

        SDL_SetRenderDrawColor(renderer, red * 255, green * 255, blue * 255, 255.0f);
        SDL_RenderClear(renderer);

        // Llamada a la función BresenhamLine para dibujar una línea
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para la línea

        BresenhamLine(renderer, 0, 0, 640, 480);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


