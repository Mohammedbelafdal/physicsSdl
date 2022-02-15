#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <SDL2/SDL2_rotozoom.h>

SDL_Color White;
struct variables
{
    SDL_bool enablePhysics;
    unsigned long numberOfEntities;
    double positiveCharge;
    double negativeCharge;
    unsigned long value;
    SDL_Surface positiveSurface;
    SDL_Surface negativeSurface;

} typedef variables;
struct position
{
    double x;
    double y;
} typedef position;
enum charge
{
    positive = 1,
    neutral = 0,
    negative = -1,
} typedef charge;
struct speed
{
    double x;
    double y;
} typedef speed;
struct acceleration
{
    double x;
    double y;
} typedef acceleration;

struct Entity
{
    SDL_bool isVisible;
    SDL_Rect hitbox;
    char *name;
    __uint64_t id;
    charge charge;
    double swiftness;
    struct position position;
    struct speed speed;
    struct acceleration acceleration;
} typedef Entity;
acceleration newAcceleration(double accelerationx, double accelerationy)
{
    acceleration Instance;
    Instance.x = accelerationx;
    Instance.y = accelerationy;
    return Instance;
}
speed newSpeed(double speedx, double speedy)
{
    speed Instance;
    Instance.x = speedx;
    Instance.y = speedy;
    return Instance;
}
Entity newEntity(char *name, charge charge, SDL_bool isVisible, double positionx, double positiony, variables *internalVariables)
{
    Entity Instance;
    Instance.name = name;
    Instance.isVisible = isVisible;
    Instance.position.x = positionx;
    Instance.position.y = positiony;
    Instance.speed.x = 0;
    Instance.speed.y = 0;
    Instance.acceleration.x = 0;
    Instance.acceleration.y = 0;
    switch (charge)
    {
    case positive:
        Instance.swiftness = 0.001;
        Instance.charge=charge;
        break;
    case negative:
        Instance.swiftness = 0.0001;
        Instance.charge=charge;
    default:
        Instance.swiftness = 2;
        break;
    }
    Instance.id = 0;
    Instance.hitbox.w = 64;
    Instance.hitbox.h = 64;
    internalVariables->numberOfEntities++;
    return Instance;
}
void physics(Entity entities[], SDL_Renderer *screen, variables internalVariables)
{
    for (int i = 0; i < internalVariables.numberOfEntities; i++)
    {
        double speedcoef = 0.9;
        double accelcoef = 0.9;
        double chargeConstant = 0.2;
        Entity *currentEntity = &entities[i];
        acceleration receivedForce = newAcceleration(0, 0);
        for (int k = 0; k < internalVariables.numberOfEntities; k++) //interactions
        {
            if (k != i)
            {
                Entity *otherEntity = &entities[k];
                double otherEntityCharge=0;
                double currentEntityCharge=0;
                double xdistance = currentEntity->position.x - otherEntity->position.x;
                double ydistance = currentEntity->position.y - otherEntity->position.y;
                double distance = sqrt((otherEntity->position.x - currentEntity->position.x) * (otherEntity->position.x - currentEntity->position.x) + (otherEntity->position.y - currentEntity->position.y) * (otherEntity->position.y - currentEntity->position.y));
                switch(otherEntity->charge)
                {
                    case positive:
                    otherEntityCharge=internalVariables.positiveCharge;
                    break;
                    case negative:
                    otherEntityCharge=internalVariables.negativeCharge;
                    default:
                    break;
                }
                switch(currentEntity->charge)
                {
                    case positive:
                    currentEntityCharge=internalVariables.positiveCharge;
                    break;
                    case negative:
                    currentEntityCharge=internalVariables.negativeCharge;
                    default:
                    break;
                }
                receivedForce.x += (otherEntityCharge * currentEntityCharge * chargeConstant * xdistance) / (distance * distance);
                receivedForce.y += (otherEntityCharge * currentEntityCharge * chargeConstant * ydistance) / (distance * distance);
            }
        }

        currentEntity->position.x += currentEntity->speed.x;
        currentEntity->position.y += currentEntity->speed.y;
        currentEntity->speed.x += (currentEntity->acceleration.x);
        currentEntity->speed.y += (currentEntity->acceleration.y);
        currentEntity->speed.x *= speedcoef;
        currentEntity->speed.y *= speedcoef;
        currentEntity->acceleration.x = accelcoef * (currentEntity->acceleration.x) + receivedForce.x * currentEntity->swiftness;
        currentEntity->acceleration.y = accelcoef * (currentEntity->acceleration.y) + receivedForce.y * currentEntity->swiftness;
    }
}
void manageKeyboard(Entity entities[1024], SDL_Event event, SDL_Renderer *screen, variables *internalVariables)
{
    Entity *player = &entities[0];
    int x;
    int y;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            // handling of close button
            SDL_Quit();
            exit(1);
            break;
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&x, &y);
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                Entity particle = newEntity("particle", negative, SDL_TRUE, x, y, internalVariables);
                entities[internalVariables->numberOfEntities - 1] = particle;
                printf("created %d\n particle", entities[internalVariables->numberOfEntities].charge);
                break;
            case SDL_BUTTON_RIGHT:
                particle = newEntity("particle", positive, SDL_TRUE, x, y, internalVariables);
                entities[internalVariables->numberOfEntities - 1] = particle;
                printf("created %d\n particle", entities[internalVariables->numberOfEntities].charge);
                break;
            };
        case SDL_KEYDOWN:
            // keyboard API for key pressed
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                SDL_Quit();
                abort();
                break;
            case SDL_SCANCODE_P:
                internalVariables->enablePhysics = !internalVariables->enablePhysics;
                break;
            case SDL_SCANCODE_K:
                internalVariables->positiveCharge*=2;
                internalVariables->positiveCharge--;
                break;
            case SDL_SCANCODE_J:
                internalVariables->positiveCharge/=2;
                break;
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                player->acceleration.y -= player->swiftness;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                player->acceleration.x -= player->swiftness;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                player->acceleration.y += player->swiftness;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                player->acceleration.x += player->swiftness;
                break;
            default:
                break;
            }
        }
    }
}
void Render(Entity entities[], SDL_Renderer *screen, variables *internalVariables)
{
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("negative.png"));
    //SDL_Texture *neutralTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("neutral.png"));
    SDL_RenderClear(screen);
    
    for (int i = 0; i < internalVariables->numberOfEntities; i++)
    {
        SDL_Rect dest;
        dest.w = entities[i].hitbox.w;
        dest.h = entities[i].hitbox.h;
        dest.x = (entities[i].position.x) - (entities[i].hitbox.w / 2);
        dest.y = (entities[i].position.y) - (entities[i].hitbox.h / 2);
        double accelerationModule = sqrt(entities[i].acceleration.x * entities[i].acceleration.x + entities[i].acceleration.y * entities[i].acceleration.y);
        int intensity = (accelerationModule / 0.73) * 255;
        double xvector = (entities[i].acceleration.x);
        double yvector = (entities[i].acceleration.y);
        SDL_SetRenderDrawColor(screen, intensity, 255 - intensity, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(screen, (entities[i].position.x), (entities[i].position.y), (int)(entities[i].position.x + xvector * 300), (int)(entities[i].position.y + yvector * 300));
        SDL_SetRenderDrawColor(screen, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        int charge=entities[i].charge;
        double angle= atan2(-entities[i].acceleration.y,entities[i].acceleration.x)*(180.0/M_PI);
        switch (charge)
        {
        case positive:
            SDL_RenderCopy(screen,SDL_CreateTextureFromSurface(screen,rotozoomSurface(IMG_Load("positive.png"), angle, 1, 1)),NULL,&(dest));
            break;
        case negative:
            SDL_RenderCopy(screen,SDL_CreateTextureFromSurface(screen,rotozoomSurface(IMG_Load("negative.png"), angle, 1, 1)),NULL,&(dest));
            break;
        default:
            break;
        }
    }
    SDL_RenderPresent(screen);
}
int main(int argc, char *argv[])

{

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window *win = SDL_CreateWindow("charge simulation", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1300, 700, 0);
    SDL_SetWindowResizable(win, SDL_FALSE);
    
    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, render_flags);
    SDL_Event event;
    int close = 0;
    Entity EntityList[1024];
    variables internalVariables = {.numberOfEntities = 0, .enablePhysics = SDL_TRUE,.positiveCharge=1.3,.negativeCharge=-1,.value=0};
    while (!close)
    {
        if(internalVariables.enablePhysics)physics(EntityList, renderer, internalVariables);
        Render(EntityList, renderer, &internalVariables);
        manageKeyboard(EntityList, event, renderer, &internalVariables);
        SDL_Delay(1000 / 128);
    }

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}
