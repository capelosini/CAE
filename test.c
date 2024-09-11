#include "include/CAE.h"
#include <allegro5/color.h>
#include <stdio.h>

GameObject* square;
GameObject* square2;

void handleEvent(ALLEGRO_EVENT ev, Scene* scene, Game* game){
    if (ev.type == ALLEGRO_EVENT_KEY_CHAR){
        switch (ev.keyboard.keycode){
            case ALLEGRO_KEY_LEFT:
                square->physics.directions.x=-1;
                square->physics.acc.x=1;
                break;
            case ALLEGRO_KEY_RIGHT:
                square->physics.directions.x=1;
                square->physics.acc.x=1;
                break;
            case ALLEGRO_KEY_UP:
                square->physics.directions.y=-1;
                square->physics.acc.y=1;
                break;
            case ALLEGRO_KEY_DOWN:
                square->physics.directions.y=1;
                square->physics.acc.y=1;
                break;
            case ALLEGRO_KEY_ESCAPE:
                game->isAlive=0;
                break;
        }
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP){
        if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT || ev.keyboard.keycode == ALLEGRO_KEY_RIGHT){
            square->physics.acc.x=0;
        }
        if (ev.keyboard.keycode == ALLEGRO_KEY_UP || ev.keyboard.keycode == ALLEGRO_KEY_DOWN){
            square->physics.acc.y=0;
        }
    }
}

void mainSceneScript(Scene* self){
    al_clear_to_color(al_map_rgb(255, 255, 255));
    if (dist(square, square2) <= square->width/2 + square2->width/2){
        printf("\nColliding");
    } else{
        printf("\nNot Colliding");
    }
    //self->camera.x+=1;
    //self->camera.y-=1;
}

int main(){
    GameConfig config;
    config.fps=60;
    config.fullscreen=1;
    config.posX=20;
    config.posY=20;
    config.sizeX=1920;
    config.sizeY=1080;
    config.title="Test CAE";

    Game* game = initGame(config);
    Scene* mainScene = createScene(mainSceneScript);
    setEventFunction(game, handleEvent);

    square = createGameObject(SOLID, 20, 20, 50, 50);
    square2 = createGameObject(SOLID, 300, 300, 50, 50);
    square->color=al_map_rgb(0, 255, 0);
    square2->color=al_map_rgb(255, 0, 0);
    printf("LENGTH: %d\n", mainScene->objects->length);
    addGameObjectToScene(mainScene, square);
    printf("LENGTH: %d\n", mainScene->objects->length);
    addGameObjectToScene(mainScene, square2);
    printf("LENGTH: %d\n", mainScene->objects->length);

    printList(mainScene->objects);

    square->physics.enabled=1;
    square->physics.friction=0.4;
    square->physics.maxSpeed=5;
    //square->physics.gravity=1;

    while (game->isAlive){
        render(game, mainScene);
    }

    freeScene(mainScene);
    freeGame(game);

    printf("Exited!\n");

    return 0;
}
