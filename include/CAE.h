#include <allegro5/allegro.h>
#include <stdlib.h>

enum OBJECT_TYPE {
    SOLID,
    SPRITE
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
    int width;
    int height;
    ALLEGRO_COLOR color;
    enum OBJECT_TYPE type;
    GameObject* next;
};

typedef struct GameObjectList GameObjectList;
struct GameObjectList{
    GameObject* first;
    GameObject* last;
    int length;
};

typedef struct GameCamera GameCamera;
struct GameCamera{
    int x;
    int y;
};

typedef struct Scene Scene;
struct Scene{
    GameCamera camera;
    GameObjectList* objects;
    void (*scriptFunction)(Scene*);
};

typedef struct Game Game;
struct Game{
    char isAlive;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *ev_queue;
    ALLEGRO_TIMER *timer;
    void (*eventFunction)(ALLEGRO_EVENT, Scene*);
};

Game* initGame(GameConfig config);
void freeGame(Game* game);
void addEventSource(Game* game, ALLEGRO_EVENT_SOURCE* ev_source);
void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT, Scene*));
void render(Game* game, Scene* scene);
GameObjectList* createGameObjectList();
void freeGameObjects(GameObject* obj);
void freeGameObjectList(GameObjectList* list);
Scene* createScene(void (*scriptFunction)(Scene*));
void freeScene(Scene* scene);
