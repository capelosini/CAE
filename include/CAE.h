#ifndef CAE_ENGINE_INIT
#define CAE_ENGINE_INIT

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <stdlib.h>

enum OBJECT_TYPE {
    SOLID,
    SPRITE,
    ANIMATED_SPRITE
};

enum COLLISION_TYPE {
    COLLISION_RECT,
    COLLISION_CIRCLE
};

typedef struct Vector2 Vector2;
struct Vector2{
    float x;
    float y;
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

typedef struct PhysicsConfig PhysicsConfig;
struct PhysicsConfig{
    unsigned char enabled;
    unsigned char gravity;
    Vector2 directions;
    Vector2 speed;
    float maxSpeed;
    Vector2 acc;
    float friction;
    float gravitySpeed;
};

typedef struct AnimationProps AnimationProps;
struct AnimationProps{
    int width;
    int height;
    Vector2 index;
    int totalFrames;
    float fps;
    ALLEGRO_BITMAP* bitmap;
    Vector2 direction;
};

typedef struct GameObject GameObject;
struct GameObject{
    float x;
    float y;
    int width;
    int height;
    PhysicsConfig physics;
    ALLEGRO_COLOR color;
    enum OBJECT_TYPE type;
    AnimationProps animation;
    unsigned char collisionEnabled;
    enum COLLISION_TYPE collisionType;
};

typedef struct LinkedItem LinkedItem;
struct LinkedItem{
    LinkedItem* next;
    void* data;
};

typedef struct LinkedList LinkedList;
struct LinkedList{
    int length;
    LinkedItem* first;
    LinkedItem* last;
    void (*onDestroy)(LinkedItem* item);
};

// typedef struct GameObjectList GameObjectList;
// struct GameObjectList{
//     GameObject* first;
//     GameObject* last;
//     int length;
// };

typedef struct GameCamera GameCamera;
struct GameCamera{
    Vector2 offset;
    GameObject* followTarget;
    Vector2 followSpeed;
    float followMaxSpeed;
    float followAcc;
};

typedef struct Scene Scene;
struct Scene{
    GameCamera camera;
    LinkedList* objects;
    void (*scriptFunction)(Scene*);
    float gravityValue;
    ALLEGRO_COLOR backgroundColor;
};

typedef struct Game Game;
struct Game{
    char isAlive;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *ev_queue;
    ALLEGRO_TIMER *timer;
    int windowX;
    int windowY;
    void (*eventFunction)(ALLEGRO_EVENT, Scene*, Game*);
    LinkedList* bitmaps;
    LinkedList* scenes;
    Scene* currentScene;
};

void LLFFDestroyBitmaps(LinkedItem* item);
Game* initGame(GameConfig config);
void freeGame(Game* game);
void addEventSource(Game* game, ALLEGRO_EVENT_SOURCE* ev_source);
void setEventFunction(Game* game, void (*f)(ALLEGRO_EVENT, Scene*, Game*));
void render(Game* game);
// GameObjectList* createGameObjectList();
LinkedList* createLinkedList(void (*onDestroy)(LinkedItem* item));
void freeLinkedItem(LinkedItem* item);
void freeLinkedListItems(LinkedItem* item, LinkedList* list);
void freeLinkedList(LinkedList* list);
void addItemToLinkedList(LinkedList* list, void* data);
void removeItemLinkedList(LinkedList* list, void* searchData);
void printList(LinkedList* list);
// void freeGameObjects(GameObject* obj);
// void freeGameObjectList(GameObjectList* list);
Scene* createScene(Game* game, void (*scriptFunction)(Scene*));
void freeScene(Scene* scene);
GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height);
ALLEGRO_BITMAP* loadBitmap(Game* game, char* pathToBitmap);
void setGameObjectAnimation(GameObject* obj, ALLEGRO_BITMAP* bitmap, int frameWidth, int frameHeight, int totalFrames, float fps);
void setBitmapTransparentColor(ALLEGRO_BITMAP* bm, ALLEGRO_COLOR color);
void addGameObjectToScene(Scene* scene, GameObject* obj);
float dist(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void changeScene(Game* game, Scene* scene);

#endif
