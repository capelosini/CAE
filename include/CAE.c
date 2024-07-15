#include "CAE.h"

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

void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT)){
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
        game->eventFunction(ev);
    } while(!al_is_event_queue_empty(game->ev_queue));
    
    // HERE: USER FUNCTION TO MANIPULATE THE SCENE (Objects positions for example)
    // TODO!
    scene->scriptFunction(scene);

    // HERE: MY FUNCTION TO MAKE THE RENDER MAGIC, CAMERA ETC
    // TODO!

    al_flip_display();

}
