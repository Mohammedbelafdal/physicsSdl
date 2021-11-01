#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <gcrypt.h>
#include <math.h>
int numberOfEntities = 0;
SDL_Color White;
struct position
{
    double x;
    double y;
} typedef position;
enum charge
{
    positive = 1,
    neutral = 1,
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
Entity newEntity(char *name, charge charge, SDL_bool isVisible, double positionx, double positiony, SDL_Texture *texture)
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
    Instance.swiftness = 2;
    Instance.charge = charge;
    Instance.id = 0;
    Instance.hitbox.w = 100;
    Instance.hitbox.h = 100;
    SDL_QueryTexture(texture, NULL, NULL, &Instance.hitbox.w, &Instance.hitbox.h);
    numberOfEntities++;
    return Instance;
}
void physics(Entity *entities[], SDL_Renderer *screen)
{
    for (int i = 0; i < numberOfEntities; i++)
    {
        double speedcoef = 0.9;
        double accelcoef = 0.9;
        double chargeConstant = 0.1;
        Entity *currentEntity = entities[i];
        acceleration receivedForce = newAcceleration(0, 0);
        for (int k = 0; k < numberOfEntities; k++)
        {
            Entity *otherEntity = entities[k];
            if (k == i)
                continue;
            double xdistance = currentEntity->position.x - otherEntity->position.x;
            double ydistance = currentEntity->position.y - otherEntity->position.y;
            double distance = sqrt((otherEntity->position.x - currentEntity->position.x) * (otherEntity->position.x - currentEntity->position.x) + (otherEntity->position.y - currentEntity->position.y) * (otherEntity->position.y - currentEntity->position.y));
            receivedForce.x += (otherEntity->charge * currentEntity->charge * chargeConstant * xdistance) / distance;
            receivedForce.y += (otherEntity->charge * currentEntity->charge * chargeConstant * ydistance) / distance;
        }
        //double speedVector = sqrt(currentEntity->speed.x * currentEntity->speed.x + currentEntity->speed.y * currentEntity->speed.y);
        // SDL_SetRenderDrawColor(screen, 255, 255, 255, SDL_ALPHA_OPAQUE);
        // SDL_RenderDrawLine(screen,currentEntity->position.x,currentEntity->position.y,currentEntity->position.x+receivedForce.x,currentEntity->position.y+receivedForce.y);
        // SDL_SetRenderDrawColor(screen, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        currentEntity->position.x += currentEntity->speed.x;
        currentEntity->position.y += currentEntity->speed.y;
        currentEntity->speed.x += (currentEntity->acceleration.x);
        currentEntity->speed.y += (currentEntity->acceleration.y);
        currentEntity->speed.x *= speedcoef;
        currentEntity->speed.y *= speedcoef;
        currentEntity->acceleration.x = accelcoef * (currentEntity->acceleration.x) + receivedForce.x;
        currentEntity->acceleration.y = accelcoef * (currentEntity->acceleration.y) + receivedForce.y;
    }
}
void manageKeyboard(Entity *entities[], SDL_Event event)
{
    Entity *player = entities[0];
    int x;
    int y;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            // handling of close button
            SDL_Quit();
            abort();
            break;
        case SDL_MOUSEBUTTONDOWN:
        switch (event.button.button)
        {
        case SDL_BUTTON_LEFT:

            SDL_GetMouseState(&x, &y);
            entities[0]->position.x = x;
            entities[0]->position.y = y;
            break;
        case SDL_BUTTON_RIGHT:
            SDL_GetMouseState(&x, &y);
            entities[1]->position.x = x;
            entities[1]->position.y = y;
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
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                player->acceleration.y -= player->swiftness * 0.5;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                player->acceleration.x -= player->swiftness * 0.5;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                player->acceleration.y += player->swiftness * 0.5;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                player->acceleration.x += player->swiftness * 0.5;
                break;
            default:
                break;
            }
        }
    }
}
void Render(Entity *entities[], SDL_Renderer *screen)
{
    TTF_Font *defaultFont = TTF_OpenFont("Roboto-Medium.ttf", 240);
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("negative.png"));
    //SDL_Texture *neutralTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("neutral.png"));
    SDL_RenderClear(screen);
    for (int i = 0; i < numberOfEntities; i++)
    {
        SDL_Rect dest;
        SDL_Surface *surfaceMessage = TTF_RenderText_Solid(defaultFont, "Test lazy", White);
        SDL_Texture *Message = SDL_CreateTextureFromSurface(screen, surfaceMessage);
        SDL_Rect Message_rect;
        dest.w = entities[i]->hitbox.w;
        dest.h = entities[i]->hitbox.h;
        dest.x = (entities[i]->position.x) - (entities[i]->hitbox.w / 2);
        dest.y = (entities[i]->position.y) - (entities[i]->hitbox.h / 2);
        Message_rect.x = 0;
        Message_rect.y = 0;
        Message_rect.w = 100;
        Message_rect.h = 100;
        double accelerationModule = sqrt(entities[i]->acceleration.x * entities[i]->acceleration.x + entities[i]->acceleration.y * entities[i]->acceleration.y);
        double g=(accelerationModule + 1);
        int intensity=(accelerationModule/0.73)*255;
        double xvector = (entities[i]->acceleration.x) * g;
        double yvector = (entities[i]->acceleration.y) * g;
        SDL_SetRenderDrawColor(screen, intensity, 255-intensity, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderCopy(screen, Message, NULL, &(Message_rect));
        SDL_RenderDrawLine(screen, dest.x + 16, dest.y + 16, (int)(dest.x + 16 + xvector * 200), (int)(dest.y + 16 + yvector * 200));
        SDL_SetRenderDrawColor(screen, 0, 0, 0, SDL_ALPHA_TRANSPARENT);

        switch (entities[i]->charge)
        {
        case positive:
            SDL_RenderCopy(screen, positiveTexture, NULL, &(dest));
            break;
        case negative:
            SDL_RenderCopy(screen, negativeTexture, NULL, &(dest));
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
    Entity *EntityList[20];
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("negative.png"));
    Entity player = newEntity("player", positive, SDL_TRUE, 1300 / 3, 700 / 2, positiveTexture);
    EntityList[0] = &player;
    Entity particle = newEntity("particle", negative, SDL_TRUE, 1300 * 2 / 3, 700 / 2, positiveTexture);
    EntityList[1] = &particle;
    //Entity particle2 = newEntity("particle", negative, SDL_TRUE, (700), 700 / 3, positiveTexture);
    //EntityList[2] = &particle2;
    while (!close)
    {

        physics(EntityList, renderer);
        Render(EntityList, renderer);
        manageKeyboard(EntityList, event);
        SDL_Delay(1000 / 128);
    }

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}
