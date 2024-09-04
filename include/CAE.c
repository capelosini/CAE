#include "CAE.h"
#include <stdio.h>
#include <math.h>

Game* initGame(GameConfig config){
    al_init();
    al_init_primitives_addon();
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
    GameObject* obj=scene->objects->first;
    while (obj!=NULL){
        // IS OBJECT NOT VISIBLE IN CAMERA
        if (!((obj->x+obj->width > scene->camera.x && obj->x < scene->camera.x+game->windowX) && (obj->y+obj->height > scene->camera.y && obj->y < scene->camera.y+game->windowY))){
            obj=obj->next;
            continue;
        }
        float x;
        float y;
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

        // PHYSICS PROCESS
        if (obj->physics.enabled){
            if (obj->physics.speed <= obj->physics.maxSpeed){
                obj->physics.speed+=obj->physics.acc-obj->physics.friction;
                if (obj->physics.speed < 0){ obj->physics.speed=0; }
            }
            else{
                obj->physics.speed=obj->physics.maxSpeed;
            }
            // if (obj->physics.acc == 0.){
            //     obj->physics.speed-=obj->physics.friction;
            // }
            
            // GRAVITY
            if (obj->physics.gravity){
                obj->physics.gravitySpeed+=scene->gravityValue;
                obj->y+=obj->physics.gravitySpeed;
            }

            obj->x+=obj->physics.speed*obj->physics.directions.x;
            obj->y+=obj->physics.speed*obj->physics.directions.y;
        }

        switch (obj->type){
            case SOLID:
                al_draw_filled_rectangle(x, y, x+obj->width, y+obj->height, obj->color);
                break;
            default:
                break;
        }
        obj=obj->next;
    }
    al_flip_display();
}

GameObjectList* createGameObjectList(){
    GameObjectList* list = (GameObjectList*)malloc(sizeof(GameObjectList));
    list->first=NULL;
    list->last=NULL;
    list->length=0;
    return list;
}

void freeGameObjects(GameObject* obj){
    if (obj == NULL)
        return;
    freeGameObjects(obj->next);
    free(obj);
    printf("Freed 1 Game Object!\n");
}

void freeGameObjectList(GameObjectList* list){
    freeGameObjects(list->first);
    free(list);
}

Scene* createScene(void (*scriptFunction)(Scene*)){
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    scene->objects = createGameObjectList();
    scene->camera.x=0.;
    scene->camera.y=0.;
    scene->scriptFunction=scriptFunction;
    scene->gravityValue=0.1;
    return scene;
}

void freeScene(Scene* scene){
    freeGameObjectList(scene->objects);
    free(scene);
}

GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height, ALLEGRO_COLOR color){
    GameObject* newObj = (GameObject*)malloc(sizeof(GameObject));
    newObj->type=type;
    newObj->x=x;
    newObj->y=y;
    newObj->width=width;
    newObj->height=height;
    newObj->color=color;
    newObj->next=NULL;
    newObj->physics.acc=0;
    newObj->physics.enabled=0;
    newObj->physics.gravity=0;
    newObj->physics.directions.x=0;
    newObj->physics.directions.y=0;
    newObj->physics.friction=0;
    newObj->physics.maxSpeed=0;
    newObj->physics.speed=0;
    newObj->physics.gravitySpeed=0;
    return newObj;
}

void addGameObjectToScene(Scene* scene, GameObject* obj){
    scene->objects->length++;

    // IF FIRST OBJECT IN LIST
    if (scene->objects->first == NULL && scene->objects->last == NULL){
        scene->objects->first=obj;
        scene->objects->last=obj;
        return;
    }
    // IF IS NOT THE FIRST OBJECT
    scene->objects->last->next=obj;
    scene->objects->last=obj;
    if (obj->next != NULL){
        printf("WARNING: Added maybe an object from other list or the same, this is strange!");
        free(obj->next);
        obj->next=NULL;
    }
    //printf("First: %p\nLast: %p\nOBJ: %p\nOBJ->NEXT: %p\n\n", scene->objects->first, scene->objects->last, obj, obj->next);
}

void removeGameObjectFromScene(Scene* scene, int id){
    if (id<0 || id >= scene->objects->length){
        return;
    } else if (id == 0){
        GameObject* tmp = scene->objects->first;
        scene->objects->first=tmp->next;
        free(tmp);
        if (scene->objects->first == NULL){
            scene->objects->last=NULL;
        }
    } else{
        int i=0;
        GameObject* obj = scene->objects->first;
        while (obj != NULL){
            if (i == id-1){
                GameObject* toDel = obj->next;
                if (toDel->next == NULL)
                    scene->objects->last=obj;
                obj->next=toDel->next;
                free(toDel);
                break;
            }
            i++;
            obj=obj->next;
        }
    }
    scene->objects->length--;
}
