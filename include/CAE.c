#include "CAE.h"
#include <stdio.h>

Game* initGame(GameConfig config){
    al_init();
    Game* game = (Game*)malloc(sizeof(Game));
    if (config.fullscreen){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        ALLEGRO_MONITOR_INFO info;
        al_get_monitor_info(0, &info);
        config.sizeX = info.x2 - info.x1;
        config.sizeY = info.y2 - info.y1;
    }

    game->display = al_create_display(config.sizeX, config.sizeY);
    if (!config.fullscreen){
        al_set_window_position(game->display, config.posX, config.posY);
    }
    al_set_window_title(game->display, config.title);
    game->ev_queue = al_create_event_queue();
    game->timer = al_create_timer(1.0 / config.fps);
    game->isAlive = 1;

    al_register_event_source(game->ev_queue, al_get_display_event_source(game->display));
    al_register_event_source(game->ev_queue, al_get_timer_event_source(game->timer));
    
    al_start_timer(game->timer);

    return game;
}

void freeGame(Game* game){
    al_destroy_display(game->display);
    al_destroy_event_queue(game->ev_queue);
    al_destroy_timer(game->timer);
    free(game);
}

void addEventSource(Game* game, ALLEGRO_EVENT_SOURCE* ev_source){
    al_register_event_source(game->ev_queue, ev_source);
}

void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT, Scene*)){
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
        game->eventFunction(ev, scene);
    } while(!al_is_event_queue_empty(game->ev_queue));
    
    // HERE: USER FUNCTION TO MANIPULATE THE SCENE (Objects positions for example)
    // TODO!
    scene->scriptFunction(scene);

    // HERE: MY FUNCTION TO MAKE THE RENDER MAGIC, CAMERA ETC
    // TODO!

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
    scene->camera.x=0;
    scene->camera.y=0;
    scene->scriptFunction=scriptFunction;
    return scene;
}

void freeScene(Scene* scene){
    freeGameObjectList(scene->objects);
    free(scene);
}

int addNewGameObjectToScene(Scene* scene, enum OBJECT_TYPE type, int x, int y, int width, int height, ALLEGRO_COLOR color){
    GameObject* newObj = (GameObject*)malloc(sizeof(GameObject));
    newObj->type=type;
    newObj->x=x;
    newObj->y=y;
    newObj->width=width;
    newObj->height=height;
    newObj->color=color;

    scene->objects->length++;

    // IF FIRST OBJECT IN LIST
    if (scene->objects->first == NULL && scene->objects->last == NULL){
        scene->objects->first=newObj;
        scene->objects->last=newObj;
        return 0;
    }
    // IF IS NOT THE FIRST OBJECT
    scene->objects->last->next=newObj;
    scene->objects->last=newObj;
    return scene->objects->length-1;
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
