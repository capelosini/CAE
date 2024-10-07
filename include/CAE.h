#ifndef CAE_ENGINE_INIT
#define CAE_ENGINE_INIT
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define CAE_DEBUG 1
#define CAE_RESERVE_SAMPLES 18

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_video.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "b64/b64.h"

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
    Vector2 direction;
};

typedef struct GameObject GameObject;
struct GameObject{
    Vector2 position;
    int width;
    int height;
    PhysicsConfig physics;
    ALLEGRO_COLOR color;
    enum OBJECT_TYPE type;
    AnimationProps animation;
    ALLEGRO_BITMAP* bitmap;
    unsigned char collisionEnabled;
    enum COLLISION_TYPE collisionType;
    void (*onCollision)(GameObject*, GameObject*);
    Vector2 startCollisionOffset;
    Vector2 endCollisionOffset;
    unsigned char invertedCollision;
    unsigned char visible;
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
    Vector2 minLimit;
    Vector2 maxLimit;
};

typedef struct UI UI;
struct UI{
    unsigned char visible;
    LinkedList* buttons;
    LinkedList* texts;
    LinkedList* progressBars;
};

typedef struct Tile Tile;
struct Tile{
    int x;
    int y;
    int idX;
    int idY;
};

typedef struct World World;
struct World{
    ALLEGRO_BITMAP* tileSheet;
    int tileWidth;
    int tileHeight;
    LinkedList* tiles;
};

typedef struct FadeEffect FadeEffect;
struct FadeEffect{
    float value;
    float speed;
};

typedef struct Scene Scene;
struct Scene{
    GameCamera camera;
    LinkedList* objects;
    void (*scriptFunction)(Scene*);
    float gravityValue;
    ALLEGRO_COLOR backgroundColor;
    UI ui;
    World* world;
    FadeEffect fadeIn;
    LinkedList* triggers;
};

typedef struct Font Font;
struct Font{
    ALLEGRO_FONT* font;
    int size;
};

typedef struct Text Text;
struct Text{
    Vector2 position;
    char* text;
    ALLEGRO_COLOR color;
    Font* font;
    unsigned char visible;
};

typedef struct Button Button;
struct Button{
    Vector2 position;
    int width;
    int height;
    ALLEGRO_COLOR backgroundColor;
    ALLEGRO_COLOR foregroundColor;
    void (*onClick)(Scene*);
    char* text;
    Font* font;
    ALLEGRO_BITMAP* bitmap;
    unsigned char visible;
};

typedef struct ProgressBar ProgressBar;
struct ProgressBar{
    Vector2 position;
    int width;
    int height;
    ALLEGRO_COLOR backgroundColor;
    ALLEGRO_COLOR foregroundColor;
    float value;
    unsigned char visible;
};

typedef struct CAEngine CAEngine;
struct CAEngine{
    char isAlive;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *ev_queue;
    ALLEGRO_TIMER *timer;
    int displayWidth;
    int displayHeight;
    float fps;
    void (*eventFunction)(ALLEGRO_EVENT, Scene*, CAEngine*);
    LinkedList* bitmaps;
    LinkedList* scenes;
    LinkedList* fonts;
    LinkedList* audioSamples;
    LinkedList* audioStreams;
    LinkedList* audioMixers;
    Scene* currentScene;
    time_t startTime;
};

void LLFFFreeButtons(LinkedItem* item);
void LLFFFreeTexts(LinkedItem* item);
void LLFFDestroyBitmaps(LinkedItem* item);
void LLFFDestroyFonts(LinkedItem* item);
void LLFFFreeScenes(LinkedItem* item);
void LLFFDestroyAudioSamples(LinkedItem* item);
void LLFFDestroyAudioStreams(LinkedItem* item);
void LLFFDestroyAudioMixers(LinkedItem* item);
void assertInit(char result, const char* module);
CAEngine* initEngine(GameConfig config);
void freeEngine(CAEngine* engine);
void addEventSource(CAEngine* engine, ALLEGRO_EVENT_SOURCE* ev_source);
void setEventFunction(CAEngine* engine, void (*f)(ALLEGRO_EVENT, Scene*, CAEngine*));
void renderButton(Button* button);
void renderText(Text* text);
void renderProgressBar(ProgressBar* bar);
void globalToLocal(Scene* scene, float* x, float* y);
void render(CAEngine* engine);
LinkedList* createLinkedList(void (*onDestroy)(LinkedItem* item));
void freeLinkedItem(LinkedItem* item);
void freeLinkedListItems(LinkedItem* item, LinkedList* list);
void freeLinkedList(LinkedList* list);
void addItemToLinkedList(LinkedList* list, void* data);
void removeItemLinkedList(LinkedList* list, void* searchData);
void printList(LinkedList* list);
ALLEGRO_SAMPLE* loadAudioSample(CAEngine* engine, const char* path);
ALLEGRO_AUDIO_STREAM* loadAudioStream(CAEngine* engine, const char* path, int buffers, int samples);
ALLEGRO_SAMPLE_ID playAudioSample(ALLEGRO_SAMPLE* sample, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode);
void stopAudioSample(ALLEGRO_SAMPLE_ID* sampleId);
void configureAudioStream(ALLEGRO_AUDIO_STREAM* stream, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode);
void playAudioStream(ALLEGRO_AUDIO_STREAM* stream);
void pauseAudioStream(ALLEGRO_AUDIO_STREAM* stream);
void stopAudioStream(ALLEGRO_AUDIO_STREAM* stream);
ALLEGRO_MIXER* createAudioMixer(CAEngine* engine, unsigned int sampleRate);
void configureAudioMixer(ALLEGRO_MIXER* mixer, float gain);
void addAudioStreamToMixer(ALLEGRO_MIXER* mixer, ALLEGRO_AUDIO_STREAM* stream);
Scene* createScene(CAEngine* engine, void (*scriptFunction)(Scene*));
void freeScene(Scene* scene);
void setupSceneWorld(Scene* scene, ALLEGRO_BITMAP* tileSheet, int tileWidth, int tileHeight);
void addWorldTile(Scene* scene, int idX, int idY, int tileX, int tileY);
GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height, Scene* scene);
void setOnGameObjectCollisionFunction(GameObject* obj, void (*onCollision)(GameObject*, GameObject*));
void addGameObjectToScene(Scene* scene, GameObject* obj);
ALLEGRO_BITMAP* loadBitmap(CAEngine* engine, const char* pathToBitmap);
ALLEGRO_BITMAP* createSubBitmap(CAEngine* engine, ALLEGRO_BITMAP* bitmap, int sx, int sy, int sw, int sh);
void setGameObjectAnimation(GameObject* obj, ALLEGRO_BITMAP* bitmap, int frameWidth, int frameHeight, int totalFrames, float fps);
void setBitmapTransparentColor(ALLEGRO_BITMAP* bm, ALLEGRO_COLOR color);
float dist(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionInvertedCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
char checkCollisionInvertedRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void changeScene(CAEngine* engine, Scene* scene);
void setGameObjectBitmap(GameObject* obj, ALLEGRO_BITMAP* bitmap);
Font* loadTTF(CAEngine* engine, const char* path, int size);
Text* createText(const char* text, float x, float y, ALLEGRO_COLOR color, Font* font);
void addTextToScene(Scene* scene, Text* text);
Button* createButton(CAEngine* engine, float x, float y, int width, int height, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor, const char* text, const char* pathToFontFile, ALLEGRO_BITMAP* bitmap, void (*onClick)(Scene*));
void addButtonToScene(Scene* scene, Button* button);
ProgressBar* createProgressBar(float x, float y, int width, int height, float initValue, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor);
void addProgressBarToScene(Scene* scene, ProgressBar* bar);
void changeText(Text* text, const char* newText);
int randInt(int min, int max);
void createTempFile(const char* b64Content, char* resultPath);
void closeTempFile(const char* path);
void playSplashScreen(CAEngine* engine);
char* getCAESplash();

#endif
