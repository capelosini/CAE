#include "include/CAE.h"
#include <stdio.h>

Game* game;
GameObject* square;
GameObject* square2;
Scene* mainScene;
Scene* mainMenu;

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
    changeScene(game, mainMenu);
}

void startGameButtonClicked(Scene* scene) {
    changeScene(game, mainScene);
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

    game = initGame(config);
    mainScene = createScene(game, mainSceneScript);
    mainMenu = createScene(game, NULL);
    setEventFunction(game, handleEvent);

    ALLEGRO_BITMAP* demoBitmap = loadBitmap(game, "./demo.bmp");
    setBitmapTransparentColor(demoBitmap, al_map_rgb(255,0,255));

    square = createGameObject(ANIMATED_SPRITE, 300, 40, 50, 50);
    square2 = createGameObject(SOLID, 300, 300, 50, 50);
    GameObject* square3 = createGameObject(SPRITE, 20, 20, 150, 150);
    GameObject* square4 = createGameObject(SPRITE, 0, 400, 300, 300);
    
    square->collisionEnabled=1;
    square->collisionType=COLLISION_CIRCLE;
    square2->collisionEnabled=1;
    square3->collisionEnabled=1;
    square3->collisionType=COLLISION_CIRCLE;
    
    setGameObjectAnimation(square, demoBitmap, 108, 140, 4, 20);
    setGameObjectBitmap(square3, loadBitmap(game, "./demoTree.png"));
    setGameObjectBitmap(square4, createSubBitmap(game, square3->bitmap, 0, 0, 500, 500));
    square2->color=al_map_rgb(255, 0, 0);
    
    addGameObjectToScene(mainScene, square);
    addGameObjectToScene(mainScene, square2);
    addGameObjectToScene(mainScene, square3);
    addGameObjectToScene(mainScene, square4);

    mainScene->camera.followTarget=square;

    printList(mainScene->objects);
    square->physics.enabled=1;
    square->physics.friction=0.4;
    square->physics.maxSpeed=5;
    //square->physics.gravity=1;

    changeScene(game, mainScene);
    // MAIN MENYH
    int size=70;
    Font* arialFont = loadTTF(game, "./arial.ttf", 20);
    char* title = "Main menu";
    addTextToScene(mainMenu, createText(title,game->displayWidth / 2 - al_get_text_width(arialFont->font, title) / 2, 50, al_map_rgb(255, 255, 255), arialFont));
    addButtonToScene(mainMenu, createButton(game, game->displayWidth / 2 - 50, 100, 100, 50, al_map_rgb(10, 10, 10), al_map_rgb(255, 255, 255), "Play", "./arial.ttf", createSubBitmap(game, demoBitmap, 0, 0, 108, 140), startGameButtonClicked));
    // MAIN SCENE
    addTextToScene(mainScene, createText("Hello WOrld!", 20, 20, al_map_rgb(0,200,0), arialFont));
    addButtonToScene(mainScene, createButton(game, 20, 100, 100, 50, al_map_rgb(10, 10, 10), al_map_rgb(255,255,255), "Click me!", "./arial.ttf", NULL, onTestButtonClick));

    while (game->isAlive){
        render(game);
    }

    freeGame(game);

    printf("Exited!\n");

    return 0;
}
