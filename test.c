#include "include/CAE.h"
#include <stdio.h>

void handleEvent(ALLEGRO_EVENT ev, Scene* scene){
    
}

void mainSceneScript(Scene* self){
    al_clear_to_color(al_map_rgb(255, 255, 255));
    self->camera.x+=1;
    self->camera.y-=1;
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

    GameObject* square = createGameObject(SOLID, 20, 20, 50, 50, al_map_rgb(0,0,0));
    GameObject* square2 = createGameObject(SOLID, 300, 300, 50, 50, al_map_rgb(0,0,0));
    printf("LENGTH: %d\n", mainScene->objects->length);
    addGameObjectToScene(mainScene, square);
    printf("LENGTH: %d\n", mainScene->objects->length);
    addGameObjectToScene(mainScene, square2);
    printf("LENGTH: %d\n", mainScene->objects->length);

    while (game->isAlive){
        render(game, mainScene);
    }

    freeScene(mainScene);
    freeGame(game);

    printf("Exited!\n");

    return 0;
}