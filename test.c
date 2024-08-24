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
    Scene* mainScene = createScene(5, mainSceneScript);
    setEventFunction(game, handleEvent);

    while (game->isAlive){
        render(game, mainScene);
    }

    free(mainScene);
    freeGame(game);

    printf("Exited!\n");

    return 0;
}