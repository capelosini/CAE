#include "CAE.h"
#include <allegro5/color.h>
#include <stdio.h>
#include <math.h>

void destroyBitmaps(LinkedItem* item){
    al_destroy_bitmap((ALLEGRO_BITMAP*)item->data);
    item->data=NULL;
    printf("\nFreed a bitmap!");
}

Game* initGame(GameConfig config){
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();
    Game* game = (Game*)malloc(sizeof(Game));
    if (config.fullscreen){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        ALLEGRO_MONITOR_INFO info;
        al_get_monitor_info(0, &info);
        config.sizeX = info.x2 - info.x1;
        config.sizeY = info.y2 - info.y1;
    }

    game->display = al_create_display(config.sizeX, config.sizeY);
    game->windowX = config.sizeX;
    game->windowY = config.sizeY;
    if (!config.fullscreen){
        al_set_window_position(game->display, config.posX, config.posY);
    }
    al_set_window_title(game->display, config.title);
    game->ev_queue = al_create_event_queue();
    game->timer = al_create_timer(1.0 / config.fps);
    game->isAlive = 1;
    game->bitmaps = createLinkedList(destroyBitmaps);

    al_register_event_source(game->ev_queue, al_get_display_event_source(game->display));
    al_register_event_source(game->ev_queue, al_get_keyboard_event_source());
    al_register_event_source(game->ev_queue, al_get_mouse_event_source());
    al_register_event_source(game->ev_queue, al_get_timer_event_source(game->timer));

    al_start_timer(game->timer);

    return game;
}

void freeGame(Game* game){
    al_destroy_display(game->display);
    al_destroy_event_queue(game->ev_queue);
    al_destroy_timer(game->timer);
    al_shutdown_primitives_addon();
    al_uninstall_keyboard();
    al_uninstall_mouse();
    freeLinkedList(game->bitmaps);
    free(game);
}

void addEventSource(Game* game, ALLEGRO_EVENT_SOURCE* ev_source){
    al_register_event_source(game->ev_queue, ev_source);
}

void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT, Scene*, Game* game)){
    game->eventFunction = f;
}

void render(Game* game, Scene* scene){
    do {
        ALLEGRO_EVENT ev;
        al_wait_for_event(game->ev_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            game->isAlive=0;
            return;
        }
        game->eventFunction(ev, scene, game);
    } while(!al_is_event_queue_empty(game->ev_queue));

    // HERE: USER FUNCTION TO MANIPULATE THE SCENE (Objects positions for example)
    scene->scriptFunction(scene);

    // HERE: MY FUNCTION TO MAKE THE RENDER MAGIC, CAMERA ETC
    LinkedItem* item=scene->objects->first;
    while (item!=NULL){
        GameObject* obj=item->data;
        float x;
        float y;
        // PHYSICS PROCESS
        if (obj->physics.enabled){
            // if (obj->physics.speed <= obj->physics.maxSpeed){
            //     obj->physics.speed+=obj->physics.acc-obj->physics.friction;
            //     if (obj->physics.speed < 0){
            //         obj->physics.speed=0;
            //         obj->physics.directions.x=0;
            //         obj->physics.directions.y=0;
            //     }
            // }
            // else{
            //     obj->physics.speed=obj->physics.maxSpeed;
            // }
            // if (obj->physics.acc == 0.){
            //     obj->physics.speed-=obj->physics.friction;
            // }

            // SPEED BY ACCELERATION X
            if (obj->physics.speed.x <= obj->physics.maxSpeed && obj->physics.speed.x >= -obj->physics.maxSpeed){
                obj->physics.speed.x+=obj->physics.acc.x*obj->physics.directions.x-obj->physics.friction*obj->physics.directions.x;
                if (obj->physics.acc.x == 0 && round(obj->physics.speed.x) == 0){
                    obj->physics.speed.x=0;
                    obj->physics.directions.x=0;
                }
            } else if (obj->physics.speed.x < 0){
                obj->physics.speed.x=-obj->physics.maxSpeed;
            } else{
                obj->physics.speed.x=obj->physics.maxSpeed;
            }

            // SPEED BY ACCELERATION Y
            if (obj->physics.speed.y <= obj->physics.maxSpeed && obj->physics.speed.y >= -obj->physics.maxSpeed){
                obj->physics.speed.y+=obj->physics.acc.y*obj->physics.directions.y-obj->physics.friction*obj->physics.directions.y;
                if (obj->physics.acc.y == 0 && round(obj->physics.speed.y) == 0){
                    obj->physics.speed.y=0;
                    obj->physics.directions.y=0;
                }
            } else if (obj->physics.speed.y < 0){
                obj->physics.speed.y=-obj->physics.maxSpeed;
            } else{
                obj->physics.speed.y=obj->physics.maxSpeed;
            }

            // GRAVITY
            if (obj->physics.gravity){
                obj->physics.gravitySpeed+=scene->gravityValue;
                obj->y+=obj->physics.gravitySpeed;
            }

            // obj->x+=obj->physics.speed*obj->physics.directions.x;
            // obj->y+=obj->physics.speed*obj->physics.directions.y;
            obj->x+=obj->physics.speed.x;
            obj->y+=obj->physics.speed.y;
        }

        // IS OBJECT NOT VISIBLE IN CAMERA
        if (!((obj->x+obj->width > scene->camera.x && obj->x < scene->camera.x+game->windowX) && (obj->y+obj->height > scene->camera.y && obj->y < scene->camera.y+game->windowY))){
            item=item->next;
            continue;
        }
        // TRANSFORM THE GLOBAL POSITION OF EVERYTHING IN LOCAL POSITION
        if (obj->x < 0 && scene->camera.x < 0)
            x=fabs(obj->x) - fabs(scene->camera.x);
        else
            x=obj->x - scene->camera.x;
        if (obj->y < 0 && scene->camera.y < 0)
            y=fabs(obj->y) - fabs(scene->camera.y);
        else
            y=obj->y - scene->camera.y;

        if (obj->x > scene->camera.x)
            x=fabs(x);
        if (obj->y > scene->camera.y)
            y=fabs(y);

        //printf("Rendered cube at (%f, %f)\n", x, y);

        switch (obj->type){
            case SOLID:
                al_draw_filled_rectangle(x, y, x+obj->width, y+obj->height, obj->color);
                break;
            case ANIMATED_SPRITE:
                if ((int)obj->animation.index.x > obj->animation.totalFrames-1)
                    obj->animation.index.x=0;
                al_draw_bitmap_region(obj->animation.bitmap, ((int)obj->animation.index.x)*obj->animation.width, ((int)obj->animation.index.y)*obj->animation.height, obj->animation.width, obj->animation.height, x, y, 0);
                obj->animation.index.x+=obj->animation.fps;
                break;
            default:
                break;
        }
        item=item->next;
    }
    al_flip_display();
}

// GameObjectList* createGameObjectList(){
//     GameObjectList* list = (GameObjectList*)malloc(sizeof(GameObjectList));
//     list->first=NULL;
//     list->last=NULL;
//     list->length=0;
//     return list;
// }

LinkedList* createLinkedList(void (*onDestroy)(LinkedItem* item)){
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->length=0;
    list->first=NULL;
    list->last=NULL;
    list->onDestroy=onDestroy;
    return list;
}

void freeLinkedItem(LinkedItem* item){
    free(item->data);
    free(item);
}

void freeLinkedListItems(LinkedItem* item, LinkedList* list){
    if (item == NULL){
        return;
    }
    freeLinkedListItems(item->next, list);
    if (list->onDestroy != NULL)
        list->onDestroy(item);
    freeLinkedItem(item);
    printf("\nFreed item!");
}

void freeLinkedList(LinkedList* list){
    freeLinkedListItems(list->first, list);
    free(list);
    printf("\n");
}

void addItemToLinkedList(LinkedList* list, void* data){
    list->length++;
    LinkedItem* newItem = (LinkedItem*)malloc(sizeof(LinkedItem));
    newItem->next=NULL;
    newItem->data=data;
    // IF FIRST OBJECT IN LIST
    if (list->first == NULL && list->last == NULL){
        list->first=newItem;
        list->last=newItem;
        return;
    }
    // IF IS NOT THE FIRST OBJECT
    list->last->next=newItem;
    list->last=newItem;
}

void removeItemLinkedList(LinkedList* list, void* searchData){
    LinkedItem* item = list->first;
    // EMPTY LIST
    if (list->first == NULL && list->last == NULL)
        return;
    // FIRST ITEM
    else if (item->data == searchData){
        list->first=item->next;
        if (list->last == item)
            list->last=NULL;
        freeLinkedItem(item);
        list->length--;
        return;
    }
    while (item != NULL){
        if (item->next->data == searchData){
            LinkedItem* itemToExclude = item->next;
            item->next=itemToExclude->next;
            freeLinkedItem(itemToExclude);
            if (item->next == NULL){
                list->last=item;
            }
            list->length--;
            return;
        }
        item=item->next;
    }
}

void printList(LinkedList* list){
    printf("\n_LENGTH: %d_", list->length);
    LinkedItem* i = list->first;
    while (i != NULL){
        printf("\n(%p) -> %p", i, i->next);
        i=i->next;
    }
    printf("\n");
}

// void freeGameObjects(GameObject* obj){
//     if (obj == NULL)
//         return;
//     freeGameObjects(obj->next);
//     free(obj);
//     printf("Freed 1 Game Object!\n");
// }

// void freeGameObjectList(GameObjectList* list){
//     freeGameObjects(list->first);
//     free(list);
// }

Scene* createScene(void (*scriptFunction)(Scene*)){
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    scene->objects = createLinkedList(NULL);
    scene->camera.x=0.;
    scene->camera.y=0.;
    scene->scriptFunction=scriptFunction;
    scene->gravityValue=0.1;
    return scene;
}

void freeScene(Scene* scene){
    freeLinkedList(scene->objects);
    free(scene);
}

GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height){
    GameObject* newObj = (GameObject*)malloc(sizeof(GameObject));
    newObj->type=type;
    newObj->x=x;
    newObj->y=y;
    newObj->width=width;
    newObj->height=height;
    newObj->color=al_map_rgb(0, 0, 0);
    newObj->physics.acc.x=0;
    newObj->physics.acc.y=0;
    newObj->physics.enabled=0;
    newObj->physics.gravity=0;
    newObj->physics.directions.x=0;
    newObj->physics.directions.y=0;
    newObj->physics.friction=0;
    newObj->physics.maxSpeed=0;
    newObj->physics.speed.x=0;
    newObj->physics.speed.y=0;
    newObj->physics.gravitySpeed=0;
    newObj->animation.bitmap=NULL;
    return newObj;
}

ALLEGRO_BITMAP* loadBitmap(Game* game, char* pathToBitmap){
    ALLEGRO_BITMAP* bm = al_load_bitmap(pathToBitmap);
    addItemToLinkedList(game->bitmaps, bm);
    return bm;
}

void setGameObjectAnimation(GameObject* obj, ALLEGRO_BITMAP* bitmap, int frameWidth, int frameHeight, int totalFrames, float fps){
    if (obj->type != ANIMATED_SPRITE)
       return;
    obj->animation.bitmap = bitmap;
    obj->animation.index.x=0;
    obj->animation.index.y=0;
    obj->width=frameWidth;
    obj->height=frameHeight;
    obj->animation.totalFrames=totalFrames;
    obj->animation.fps=fps;
    if (fps > 0)
        obj->animation.fps/=100;
}

void addGameObjectToScene(Scene* scene, GameObject* obj){
    addItemToLinkedList(scene->objects, obj);
}

float dist(GameObject* a, GameObject* b){
    return sqrt(pow((a->x+a->width/2.) - (b->x+b->width/2.), 2) + pow((a->y+a->height/2.) - (b->y+b->height/2.), 2));
}
