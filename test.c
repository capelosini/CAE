#include "include/CAE.h"
#include <stdio.h>

GameObject* square;
GameObject* square2;

void handleEvent(ALLEGRO_EVENT ev, Scene* scene, Game* game){
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
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
    //  if(checkCollision(square,square2))
    //     {
    //      if(square->physics.acc.x!=0)
    //      square->physics.acc.x=-1; 
    //       if(square->physics.acc.y!=0)
    //      square->physics.acc.y=-1; 
    //     } else{
    //         square->physics.acc.x=abs(square->physics.acc.x);
    //         square->physics.acc.y=abs(square->physics.acc.y);
    //     }
}

void mainSceneScript(Scene* self){
    if (dist(square, square2) <= square->width/2. + square2->width/2.){
        //printf("\nColliding");
    } else{
        //printf("\nNot Colliding");
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

    ALLEGRO_BITMAP* demoBitmap = loadBitmap(game, "./demo.bmp");
    setBitmapTransparentColor(demoBitmap, al_map_rgb(255,0,255));

    square = createGameObject(SOLID, 20, 20, 50, 50);
    square->collisionEnabled=1;
    square2 = createGameObject(SOLID, 300, 300, 50, 50);
    square2->collisionEnabled=1;
    GameObject* square3 = createGameObject(ANIMATED_SPRITE, 500, 500, 50, 50);
    setGameObjectAnimation(square3, demoBitmap, 108, 140, 4, 20);

    square->color=al_map_rgb(0, 255, 0);
    square2->color=al_map_rgb(255, 0, 0);
    addGameObjectToScene(mainScene, square);
    addGameObjectToScene(mainScene, square2);
    addGameObjectToScene(mainScene, square3);

    printList(mainScene->objects);

    square->physics.enabled=1;
    square->physics.friction=0.4;
    square->physics.maxSpeed=5;
    //square->physics.gravity=1;

    while (game->isAlive){
        //printf("\nwidth anim: %d", square3->animation.width);
        render(game, mainScene);
    }

    freeScene(mainScene);
    freeGame(game);

    printf("Exited!\n");

    return 0;
}
