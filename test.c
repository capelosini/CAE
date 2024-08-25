#include "include/CAE.h"
#include <stdio.h>

void handleEvent(ALLEGRO_EVENT ev, Scene* scene){
    
}

void mainSceneScript(Scene* self){
    al_clear_to_color(al_map_rgb(255, 255, 255));
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

    printf("LENGTH: %d\n", mainScene->objects->length);
    int square = addNewGameObjectToScene(mainScene, SOLID, 20, 20, 50, 50, al_map_rgb(0,0,0));
    printf("LENGTH: %d\n", mainScene->objects->length);
    int square2 = addNewGameObjectToScene(mainScene, SOLID, 20, 20, 50, 50, al_map_rgb(0,0,0));
    printf("SQUARE1 ID: %d\nSQUARE2 ID: %d\n", square, square2);
    printf("LENGTH: %d\n", mainScene->objects->length);

    while (game->isAlive){
        render(game, mainScene);
    }

    freeScene(mainScene);
    freeGame(game);

    printf("Exited!\n");

    return 0;
}