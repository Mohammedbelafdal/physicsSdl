#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <gcrypt.h>
#include <math.h>

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

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
    Instance.hitbox.w = 64;
    Instance.hitbox.h = 64;
    SDL_QueryTexture(texture, NULL, NULL, &Instance.hitbox.w, &Instance.hitbox.h);
    numberOfEntities++;
    return Instance;
}
void physics(Entity entities[], SDL_Renderer *screen)
{
    for (int i = 0; i < numberOfEntities; i++)
    {
        double speedcoef = 0.9;
        double accelcoef = 0.9;
        double chargeConstant = 0.2;
        Entity *currentEntity = &entities[i];
        acceleration receivedForce = newAcceleration(0, 0);
        for (int k = 0; k < numberOfEntities; k++) //interactions
        {
            if (k != i)
            {
                Entity *otherEntity = &entities[k];
                double xdistance = currentEntity->position.x - otherEntity->position.x;
                double ydistance = currentEntity->position.y - otherEntity->position.y;
                double distance = sqrt((otherEntity->position.x - currentEntity->position.x) * (otherEntity->position.x - currentEntity->position.x) + (otherEntity->position.y - currentEntity->position.y) * (otherEntity->position.y - currentEntity->position.y));
                receivedForce.x += (otherEntity->charge * currentEntity->charge * chargeConstant * xdistance) / (distance * distance);
                receivedForce.y += (otherEntity->charge * currentEntity->charge * chargeConstant * ydistance) / (distance * distance);
            }
        }

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
void manageKeyboard(Entity entities[1024], SDL_Event event, SDL_Renderer *screen)
{
    Entity *player = &entities[0];
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("negative.png"));
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
            SDL_GetMouseState(&x, &y);
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                Entity particle=newEntity("particle", negative, SDL_TRUE, x, y, negativeTexture);
                entities[numberOfEntities-1]=particle;
                printf("created %d\n particle", entities[numberOfEntities].charge);
                break;
            case SDL_BUTTON_RIGHT:
                particle=newEntity("particle", positive, SDL_TRUE, x, y, positiveTexture);
                entities[numberOfEntities-1]=particle;
                printf("created %d\n particle", entities[numberOfEntities].charge);
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
void Render(Entity entities[], SDL_Renderer *screen)
{
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("negative.png"));
    //SDL_Texture *neutralTexture = SDL_CreateTextureFromSurface(screen, IMG_Load("neutral.png"));
    SDL_RenderClear(screen);
    for (int i = 0; i < numberOfEntities; i++)
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

        switch (entities[i].charge)
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
    Entity EntityList[1024];
    SDL_Texture *positiveTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("positive.png"));
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("negative.png"));
    Entity particle;
    for (int x = 0; x < 6; x++)
    {
         particle=newEntity("particle", negative, SDL_TRUE, 1300*x/6, 700 / 3, negativeTexture);
         EntityList[x*2]=particle;
         particle=newEntity("particle", positive, SDL_TRUE, 1300*x/6, 700 *2/ 3, positiveTexture);
         EntityList[x*2+1]=particle;
    }
    while (!close)
    {
        physics(EntityList, renderer);
        Render(EntityList, renderer);
        manageKeyboard(EntityList, event, renderer);
        SDL_Delay(1000 / 128);
    }

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}
