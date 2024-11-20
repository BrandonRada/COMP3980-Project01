#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    SDL_GameController *controller = NULL;
    SDL_Event           event;
    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            printf("Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("No game controllers connected.\n");
        SDL_Quit();
        return EXIT_FAILURE;
    }

    while(1)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                SDL_GameControllerClose(controller);
                SDL_Quit();
                return EXIT_SUCCESS;
            }
            if(event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
            {
                printf("Button event: button %d %s\n", event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN ? "pressed" : "released");
            }
            if(event.type == SDL_CONTROLLERAXISMOTION)
            {
                printf("Axis event: axis %d position %d\n", event.caxis.axis, event.caxis.value);
            }
        }
    }
}
