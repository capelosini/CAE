#include <allegro5/allegro.h>
#include <stdlib.h>

typedef enum OBJECT_TYPE OBJECT_TYPE;
enum OBJECT_TYPE {
    SOLID,
    PLAYER
};

typedef struct GameConfig GameConfig;
struct GameConfig{
    char fullscreen;
    int sizeX;
    int sizeY;
    int posX;
    int posY;
    char* title;
    int fps;
};

typedef struct GameObject GameObject;
struct GameObject{
    int x;
    int y;
    OBJECT_TYPE type;
};

typedef struct GameCamera GameCamera;
struct GameCamera{
    int x;
    int y;    
};

typedef struct Scene Scene;
struct Scene{
    GameCamera camera;
    GameObject* objects;
    void (*scriptFunction)(Scene*);
};

typedef struct Game Game;
struct Game{
    char isAlive;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *ev_queue;
    ALLEGRO_TIMER *timer;
    void (*eventFunction)(ALLEGRO_EVENT);
};

Game* initGame(GameConfig config);
void freeGame(Game* game);
void addEventSource(Game* game, ALLEGRO_EVENT_SOURCE* ev_source);
void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT));
void render(Game* game, Scene* scene);
