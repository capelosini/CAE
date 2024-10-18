#include "include/CAE.h"
#include <stdio.h>

CAEngine* engine;
GameObject* square;
GameObject* square2;
Scene* mainScene;
Scene* mainMenu;
ALLEGRO_AUDIO_STREAM* c418;
ALLEGRO_SAMPLE* sfx;
ProgressBar* playerLifeBar;
Text* textInMainMenu;

void handleEvent(ALLEGRO_EVENT ev, Scene* scene, CAEngine* engine){
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                engine->isAlive = 0;
                break;
            case ALLEGRO_KEY_SPACE:
                square2->visible = !square2->visible;
                scene->camera.zoom-=0.1;
                break;
            default:
                break;
        }
    }
    if (ev.type == ALLEGRO_EVENT_MOUSE_AXES){
        textInMainMenu->width = ev.mouse.x;
    }
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
    pauseAudioStream(c418);
    playAudioSample(sfx, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE);
    changeScene(engine, mainMenu);
}

void startGameButtonClicked(Scene* scene) {
    playAudioSample(sfx, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE);
    playAudioStream(c418);
    playerLifeBar->value-=15.5;
    changeScene(engine, mainScene);
}

void onPlayerCollision(GameObject* self, GameObject* other){
    if (other->type == SOLID){
        printf("\nCOLLISION WITH SOLID!");
    } else if (other->type == SPRITE){
        printf("\nCOLLISION WITH SPRITE!");
    }
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

    engine = initEngine(config);
    mainScene = createScene(engine, mainSceneScript);
    mainScene->camera.maxLimit.x=4800;
    mainScene->camera.maxLimit.y=4800;
    mainMenu = createScene(engine, NULL);
    setEventFunction(engine, handleEvent);

    c418 = loadAudioStream(engine, "./sounds/c418.opus", 2, 2048);
    sfx = loadAudioSample(engine, "./sounds/sfx.wav");
    configureAudioStream(c418, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP);
    playAudioStream(c418);

    ALLEGRO_BITMAP* demoBitmap = loadBitmap(engine, "./images/demo.bmp");
    setBitmapTransparentColor(demoBitmap, al_map_rgb(255,0,255));

    square = createGameObject(ANIMATED_SPRITE, 300, 40, 50, 50, mainScene);
    setOnGameObjectCollisionFunction(square, onPlayerCollision);
    square2 = createGameObject(SOLID, 300, 300, 50, 50, mainScene);
    GameObject* square3 = createGameObject(SPRITE, 20, 20, 150, 150, mainScene);
    GameObject* square4 = createGameObject(SPRITE, 0, 400, 300, 300, mainScene);

    GameObject* map = createGameObject(SOLID, 0, 0, 2000, 2000, mainScene);
    map->color=al_map_rgba(0, 0, 0, 0);
    map->collisionEnabled=1;
    map->invertedCollision=1;

    square->collisionEnabled=1;
    square2->collisionEnabled=1;
    square3->collisionEnabled=1;
    square3->startCollisionOffset.y=100;
    square3->startCollisionOffset.x=50;
    square3->endCollisionOffset.x=-50;
    
    setGameObjectAnimation(square, demoBitmap, 108, 140, 4, 20);
    setGameObjectBitmap(square3, loadBitmap(engine, "./images/demoTree.png"));
    setGameObjectBitmap(square4, createSubBitmap(engine, square3->bitmap, 0, 0, 500, 500));
    square2->color=al_map_rgb(255, 0, 0);

    mainScene->camera.followTarget=square;

    printList(mainScene->objects);
    square->physics.enabled=1;
    square->physics.friction=0.4;
    square->physics.maxSpeed=5;
    //square->physics.gravity=1;

    setupSceneWorld(mainScene, loadBitmap(engine, "./images/tileSheet.png"), 32, 32);
    for (int i=0; i<10; i++){
        for (int j=0; j<10; j++){
            if (i==0 && j==0){
                addWorldTile(mainScene, 0, 0, j, i);
            } else if (i==0){
                addWorldTile(mainScene, 1, 0, j, i);
            } 
            else{
                addWorldTile(mainScene, 1, 1, j, i);
            }
        }
    }

    changeScene(engine, mainScene);
    // MAIN MENYH
    int size=70;
    Font* arialFont = loadTTF(engine, "./fonts/arial.ttf", 20);
    char* title = "Main menu";
    createText(title,engine->displayWidth / 2 - al_get_text_width(arialFont->font, title) / 2, 50, 0, al_map_rgb(255, 255, 255), al_map_rgb(5, 2, 50), NULL, arialFont, 10, 10, mainMenu);
    createText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut ut tincidunt elit. Nunc a magna at nulla tempor iaculis. Curabitur at enim sollicitudin, varius nisi vel, viverra odio. Ut porta metus sed metus gravida elementum. Pellentesque ut mi id quam euismod convallis. Duis vulputate tempus sagittis. Quisque aliquam justo justo, eget lobortis neque tempor non.Integer porta volutpat turpis, nec venenatis ante volutpat sit amet. Proin condimentum vitae augue id tincidunt. Donec tristique lectus non dui pellentesque tincidunt. In sit amet leo suscipit, feugiat leo id, condimentum tellus. Proin vel tempor metus. Mauris in auctor velit. Donec justo justo, iaculis eget pellentesque eget, interdum a nibh. Aenean tincidunt tempor sem. Integer eget elementum metus. Suspendisse non fringilla nunc, sit amet suscipit diam.Suspendisse a justo lorem. Phasellus ac nulla sed arcu fermentum sollicitudin. Suspendisse potenti. Aenean a augue venenatis, rhoncus sapien ut, laoreet felis. Vivamus mi neque, iaculis ac ligula eget, fermentum maximus elit. Praesent at elementum lorem, et tincidunt leo. Nunc ut lacinia ligula. Aliquam eu est finibus, iaculis ipsum vitae, dignissim risus. Proin pulvinar urna sit amet metus pharetra, eget pulvinar nisl sodales. Proin aliquam dolor at urna dignissim maximus. Nulla imperdiet varius pulvinar", 800, 200, 340, al_map_rgb(255, 255, 255), al_map_rgba(0,0,0,100), NULL, arialFont, 50, 50, mainMenu);
    addButtonToScene(mainMenu, createButton(engine, engine->displayWidth / 2 - 50, 100, 100, 50, al_map_rgb(10, 10, 10), al_map_rgb(255, 255, 255), "Play", "./fonts/arial.ttf", createSubBitmap(engine, demoBitmap, 0, 0, 108, 140), startGameButtonClicked));
    textInMainMenu=createText("Hello World!\nDeserunt aliqua duis aliqua magna adipisicing ea ex enim tempor eiusmod reprehenderit officia quis excepteur. Mollit veniam adipisicing incididunt qui est minim.", 200, 500, 400, al_map_rgb(255,255,255), al_map_rgb(0, 0, 0), NULL, arialFont, 50, 50, mainMenu);
    // MAIN SCENE
    createText("Hello WOrld!", 20, 20, 0, al_map_rgb(0,200,0), al_map_rgba(0, 0, 0, 100), NULL, arialFont, 20, 20, mainScene);
    addButtonToScene(mainScene, createButton(engine, 20, 100, 100, 50, al_map_rgb(10, 10, 10), al_map_rgb(255,255,255), "Click me!", "./fonts/arial.ttf", NULL, onTestButtonClick));
    playerLifeBar = createProgressBar(engine->displayWidth-400, 50, 200, 20, 100, al_map_rgba(10,10,20,100), al_map_rgba(255,10,10,255));
    addProgressBarToScene(mainScene, playerLifeBar);

    mainScene->fadeIn.speed=2;

    while (engine->isAlive){
        render(engine);
    }

    freeEngine(engine);

    printf("Exited!\n");

    return 0;
}
