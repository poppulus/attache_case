//#include <GL/glew.h>
//#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#define ALIGN_HORIZONTAL    0
#define ALIGN_VERTICAL      1

#define INVENTORY_SIZE      10

enum ITEM_TYPE
{
    ITEM_NONE,
    ITEM_PISTOL,
    ITEM_RIFLE,
    ITEM_SHOTGUN,
    ITEM_FAID
};

typedef struct
{
    unsigned char id, align, type, x, y;
} case_item;

//unsigned char case_array[INVENTORY_SIZE];

case_item inventory[INVENTORY_SIZE], *clicked_item;

/*
void display()
{
    glDrawArrays(GL_LINES, 0, 32);
}
*/

SDL_Window *w;
SDL_Renderer *r;
SDL_Event e;
SDL_Rect q, inv_item, cur_item, inv_item_err;

int quit = 0, spawn_gun = 0, test_item = 0, mx = 0, my = 0, item_id = 1;

int item_align_check(SDL_Rect item, int x, int y, int w, int h)
{
    if (item.x < (x << 5) + w && item.x + item.w > (x << 5)
    && item.y < (y << 5) + h && item.y + item.h > (y << 5))
    {
        return 0;
    }

    return 1;
}

void inventory_item_reset(case_item *item)
{
    item->align = ALIGN_HORIZONTAL;
    item->id = 0;
    item->type = ITEM_NONE;
    item->x = 0;
    item->y = 0;
}

void inventory_reset(case_item items[])
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        inventory_item_reset(&items[i]);
    }
}

int find_empty_spot(int idx)
{
    for (int y = 0; y < 7; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if ((x << 5) + 96 <= 320 && (y << 5) + 64 <= 224)
            {
                for (int i = 0; i < INVENTORY_SIZE; i++)
                {
                    if (inventory[i].id && i != idx)
                    {
                        if ((x > inventory[i].x && x + 3 < inventory[i].x)
                        && (y > inventory[i].y && y + 2 < inventory[i].y))
                        {
                            return 0;
                        }
                    }
                }
                inventory[idx].align = ALIGN_HORIZONTAL;
                inventory[idx].id = ++item_id;
                inventory[idx].type = ITEM_PISTOL;
                inventory[idx].x = x;
                inventory[idx].y = y;
                return 1;
            }
        }
    }

    return 0;
}

void input()
{
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    if (clicked_item)
                    {
                        if (cur_item.x + cur_item.w <= 320 
                        && cur_item.y + cur_item.h <= 224)
                        {
                            unsigned char ok = 1;
                            for (int i = 0; i < INVENTORY_SIZE; i++)
                            {
                                if (inventory[i].id && inventory[i].id != clicked_item->id)
                                {
                                    switch (inventory[i].type)
                                    {
                                        default: break;
                                        case ITEM_FAID:
                                            if (inventory[i].align == ALIGN_HORIZONTAL)
                                            {
                                                ok = item_align_check(cur_item, 
                                                                    inventory[i].x, 
                                                                    inventory[i].y,
                                                                    64, 32);
                                            }
                                            else if (inventory[i].align == ALIGN_VERTICAL)
                                            {
                                                ok = item_align_check(cur_item, 
                                                                    inventory[i].x, 
                                                                    inventory[i].y,
                                                                    32, 64);
                                            }
                                            break;
                                    }
                                    if (!ok) break;
                                }
                            }
                            if (ok)
                            {
                                clicked_item->x = cur_item.x >> 5;
                                clicked_item->y = cur_item.y >> 5;
                                clicked_item = NULL;
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < INVENTORY_SIZE; i++)
                        {
                            if (inventory[i].id)
                            {
                                switch (inventory[i].type)
                                {
                                    default: break;
                                    case ITEM_PISTOL:
                                        if (inventory[i].align == ALIGN_HORIZONTAL)
                                        {
                                            if (e.motion.x > (inventory[i].x << 5) 
                                            && e.motion.x < (inventory[i].x << 5) + 96)
                                            {
                                                if (e.motion.y > (inventory[i].y << 5) 
                                                && e.motion.y < (inventory[i].y << 5) + 64)
                                                {
                                                    clicked_item = &inventory[i];
                                                }
                                            }
                                        }
                                        else if (inventory[i].align == ALIGN_VERTICAL)
                                        {
                                            if (e.motion.x > (inventory[i].x << 5) 
                                            && e.motion.x < (inventory[i].x << 5) + 64)
                                            {
                                                if (e.motion.y > (inventory[i].y << 5) 
                                                && e.motion.y < (inventory[i].y << 5) + 96)
                                                {
                                                    clicked_item = &inventory[i];
                                                }
                                            }
                                        }
                                        break;
                                    case ITEM_RIFLE:
                                        break;
                                    case ITEM_SHOTGUN:
                                        break;
                                    case ITEM_FAID:
                                        break;
                                }
                            }
                        }
                    }
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    if (clicked_item)
                        clicked_item->align = !clicked_item->align;
                }
                break;
            case SDL_MOUSEMOTION:
                mx = e.motion.x;
                my = e.motion.y;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    if (!spawn_gun)
                    {
                        for (int i = 0; i < INVENTORY_SIZE; i++)
                        {
                            if (!inventory[i].id)
                            {
                                if (find_empty_spot(i))
                                {
                                    spawn_gun = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    for (int i = 0; i < INVENTORY_SIZE - 1; i++)
                    {
                        inventory_item_reset(&inventory[i]);
                    }
                    spawn_gun = 0;
                    clicked_item = NULL;
                }
                break;
        }
    }
}

void render()
{
    SDL_SetRenderDrawColor(r, 0x10, 0x10, 0x10, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(r);

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (inventory[i].id && (&inventory[i] != clicked_item))
        {
            switch (inventory[i].type)
            {
                default: 
                    inv_item.w = 32;
                    inv_item.h = 32;
                    SDL_SetRenderDrawColor(r, 0x80, 0x80, 0x80, SDL_ALPHA_OPAQUE);
                break;
                case ITEM_PISTOL:
                    if (inventory[i].align == ALIGN_HORIZONTAL)
                    {
                        inv_item.w = 96;
                        inv_item.h = 64;
                    }
                    else if (inventory[i].align == ALIGN_VERTICAL)
                    {
                        inv_item.w = 64;
                        inv_item.h = 96;
                    }
                    SDL_SetRenderDrawColor(r, 0, 0x80, 0, SDL_ALPHA_OPAQUE);
                    break;
                case ITEM_FAID:
                    if (inventory[i].align == ALIGN_HORIZONTAL)
                    {
                        inv_item.w = 64;
                        inv_item.h = 32;
                    }
                    else if (inventory[i].align == ALIGN_VERTICAL)
                    {
                        inv_item.w = 32;
                        inv_item.h = 64;
                    }
                    SDL_SetRenderDrawColor(r, 0, 0, 0x80, SDL_ALPHA_OPAQUE);
                    break;
            }
            inv_item.x = inventory[i].x << 5;
            inv_item.y = inventory[i].y << 5;
            SDL_RenderFillRect(r, &inv_item);
        }
    }

    if (clicked_item)
    {
        switch (clicked_item->type)
        {
            default: 
                cur_item.w = 32;
                cur_item.h = 32;
                break;
            case ITEM_PISTOL:
                if (clicked_item->align == ALIGN_HORIZONTAL)
                {
                    cur_item.w = 96;
                    cur_item.h = 64;
                }
                else if (clicked_item->align == ALIGN_VERTICAL)
                {
                    cur_item.w = 64;
                    cur_item.h = 96;
                }

                if (((mx >> 5) << 5) + cur_item.w <= 320)
                    cur_item.x = (mx >> 5) << 5;
                else
                    cur_item.x = 320 - cur_item.w;

                if (((my >> 5) << 5) + cur_item.h <= 224)
                    cur_item.y = (my >> 5) << 5;
                else
                    cur_item.y = 224 - cur_item.h;
                break;
        }

        SDL_SetRenderDrawColor(r, 0, 0x80, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(r, &cur_item);

        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            if (inventory[i].id && inventory[i].id != clicked_item->id)
            {
                switch (inventory[i].type)
                {
                    default: break;
                    case ITEM_FAID:
                        for (int y = cur_item.y >> 5; y < (cur_item.y >> 5) + (cur_item.h >> 5); y++)
                        {
                            for (int x = cur_item.x >> 5; x < (cur_item.x >> 5) + (cur_item.w >> 5); x++)
                            {
                                if (inventory[i].align == ALIGN_HORIZONTAL)
                                {
                                    if ((x == inventory[i].x || x == inventory[i].x + 1)
                                    && y == inventory[i].y)
                                    {
                                        inv_item_err.x = x << 5;
                                        inv_item_err.y = y << 5;
                                        SDL_SetRenderDrawColor(r, 0x80, 0x00, 0x00, SDL_ALPHA_OPAQUE);
                                        SDL_RenderFillRect(r, &inv_item_err);
                                    }
                                }
                                else if (inventory[i].align == ALIGN_VERTICAL)
                                {
                                    if (x == inventory[i].x
                                    && (y == inventory[i].y || y == inventory[i].y + 1))
                                    {
                                        inv_item_err.x = x << 5;
                                        inv_item_err.y = y << 5;
                                        SDL_SetRenderDrawColor(r, 0x80, 0x00, 0x00, SDL_ALPHA_OPAQUE);
                                        SDL_RenderFillRect(r, &inv_item_err);
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
    }

    SDL_SetRenderDrawColor(r, 0x90, 0x90, 0x90, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(r, &q);

    for (int i = 1; i < 10; i++)
        SDL_RenderDrawLine(r, i << 5, 0, i << 5, 224);
    
    for (int i = 1; i < 7; i++)
        SDL_RenderDrawLine(r, 0, i << 5, 320, i << 5);

    if (clicked_item)
    {
        SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(r, &cur_item);
    }
    
    SDL_RenderPresent(r);
}

int main(int argc, const char *argv[])
{
    /*
    glutInit(&argc, (char**)argv);
    glutInitDisplayMode(GLUT_SINGLE);
    int window = glutCreateWindow("Attache Case Test");
    glutSetWindow(window);
    glutInitWindowSize(640, 480);

    glutDisplayFunc(display);

    glutMainLoop();
    */

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize, %s\n", SDL_GetError());
        return 1;
    }

    w = SDL_CreateWindow(
        "Attache Case Test", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        320, 224, SDL_WINDOW_OPENGL);

    if (w == NULL) 
    {
        printf("SDL could not create a window, %s\n", SDL_GetError());
        goto quit;
    }

    if ((r = SDL_CreateRenderer(w, 0, SDL_RENDERER_ACCELERATED)) == NULL)
    {
        printf("SDL could not create a renderer, %s\n", SDL_GetError());
        goto quit;
    }

    q.w = 320;
    q.h = 224;
    q.x = 0;
    q.y = 0;

    inv_item_err.w = 32;
    inv_item_err.h = 32;

    inventory_reset(inventory);

    inventory[9].align = ALIGN_VERTICAL;
    inventory[9].id = 1;
    inventory[9].type = ITEM_FAID;
    inventory[9].x = 9;
    inventory[9].y = 5;

    while (!quit)
    {
        input();
        render();
    }

quit:
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}
