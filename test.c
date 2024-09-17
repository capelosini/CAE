#include "include/CAE.h"
#include <stdio.h>

GameObject* square;
GameObject* square2;

void handleEvent(ALLEGRO_EVENT ev, Scene* scene, Game* game){
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
        switch (ev.keyboard.keycode){
            // case ALLEGRO_KEY_A:
            //     square->physics.directions.x=-1;
            //     square->physics.acc.x=1;
            //     square->animation.direction.x=-1;
            //     break;
            // case ALLEGRO_KEY_D:
            //     square->physics.directions.x=1;
            //     square->physics.acc.x=1;
            //     square->animation.direction.x=1;
            //     break;
            // case ALLEGRO_KEY_W:
            //     square->physics.directions.y=-1;
            //     square->physics.acc.y=1;
            //     break;
            // case ALLEGRO_KEY_S:
            //     square->physics.directions.y=1;
            //     square->physics.acc.y=1;
            //     break;
            case ALLEGRO_KEY_ESCAPE:
                game->isAlive=0;
                break;
        }
    } 
    // else if (ev.type == ALLEGRO_EVENT_KEY_UP){
    //     if (ev.keyboard.keycode == ALLEGRO_KEY_A || ev.keyboard.keycode == ALLEGRO_KEY_D){
    //         square->physics.acc.x=0;
    //     }
    //     if (ev.keyboard.keycode == ALLEGRO_KEY_W || ev.keyboard.keycode == ALLEGRO_KEY_S){
    //         square->physics.acc.y=0;
    //     }
    // }
}

void mainSceneScript(Scene* self){
    //self->camera.x+=1;
    //self->camera.y-=1;
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);
    char up = al_key_down(&keyState, ALLEGRO_KEY_W);
    char down = al_key_down(&keyState, ALLEGRO_KEY_S);
    char left = al_key_down(&keyState, ALLEGRO_KEY_A);
    char right = al_key_down(&keyState, ALLEGRO_KEY_D);

    printf("\nUP: %d\nDOWN: %d\nRIGHT: %d\nLEFT: %d\n", up, down, right, left);
    printf("\nACCX: %f\nACCY: %f\n", square->physics.acc.x, square->physics.acc.y);

    if (up && !down){
        square->physics.directions.y=-1;
        square->physics.acc.y=1;
    } else if (down && !up){
        square->physics.directions.y=1;
        square->physics.acc.y=1;
    } else{
        square->physics.acc.y=0;
    }
    if (left && !right){
        square->physics.directions.x=-1;
        square->physics.acc.x=1;
        square->animation.direction.x=-1;
    } else if (right && !left){
        square->physics.directions.x=1;
        square->physics.acc.x=1;
        square->animation.direction.x=1;
    } else{
        square->physics.acc.x=0;
    }
}

void onTestButtonClick(Scene* scene){
    printf("\nButton clicked!");
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
    Scene* mainScene = createScene(game, mainSceneScript);
    setEventFunction(game, handleEvent);

    ALLEGRO_BITMAP* demoBitmap = loadBitmap(game, "./demo.bmp");
    setBitmapTransparentColor(demoBitmap, al_map_rgb(255,0,255));

    square = createGameObject(ANIMATED_SPRITE, 300, 40, 50, 50);
    square2 = createGameObject(SOLID, 300, 300, 50, 50);
    GameObject* square3 = createGameObject(SPRITE, 20, 20, 150, 150);
    
    square->collisionEnabled=1;
    square->collisionType=COLLISION_CIRCLE;
    square2->collisionEnabled=1;
    square3->collisionEnabled=1;
    square3->collisionType=COLLISION_CIRCLE;
    
    setGameObjectAnimation(square, demoBitmap, 108, 140, 4, 20);
    setGameObjectBitmap(square3, loadBitmap(game, "./demoTree.png"));
    
    square2->color=al_map_rgb(255, 0, 0);
    
    addGameObjectToScene(mainScene, square);
    addGameObjectToScene(mainScene, square2);
    addGameObjectToScene(mainScene, square3);

    mainScene->camera.followTarget=square;

    printList(mainScene->objects);
    square->physics.enabled=1;
    square->physics.friction=0.4;
    square->physics.maxSpeed=5;
    //square->physics.gravity=1;

    changeScene(game, mainScene);

    Font* arialFont = loadTTF(game, "./arial.ttf", 20);
    addTextToScene(mainScene, createText("Hello WOrld!", 20, 20, al_map_rgb(0,200,0), arialFont));
    addButtonToScene(mainScene, createButton(20, 100, 100, 50, al_map_rgb(10, 10, 10), createText("Click me!", 400, 400, al_map_rgb(255,255,255), arialFont), onTestButtonClick));

    while (game->isAlive){
        // ALLEGRO_KEYBOARD_STATE state;
        // al_get_keyboard_state(&state);
        // printf("\n%d", al_key_down(&state, ALLEGRO_KEY_0));
        render(game);
    }

    freeGame(game);

    printf("Exited!\n");

    return 0;
}
