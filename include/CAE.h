#ifndef CAE_ENGINE_INIT
#define CAE_ENGINE_INIT
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define CAE_DEBUG 1
#define CAE_SPLASH_SCREEN 1
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

typedef struct GameCamera GameCamera;
struct GameCamera{
    Vector2 offset;
    GameObject* followTarget;
    Vector2 followSpeed;
    float followMaxSpeed;
    float followAcc;
    Vector2 minLimit;
    Vector2 maxLimit;
    float zoom;
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

typedef struct TileGenerationSettings TileGenerationSettings;
struct TileGenerationSettings{
    unsigned char enabled;
    int idMinX;
    int idMaxX;
    int idMinY;
    int idMaxY;
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
    TileGenerationSettings tileGenerationSettings;
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
    int width;
    ALLEGRO_COLOR backgroundColor;
    ALLEGRO_BITMAP* backgroundBitmap;
    unsigned char visible;
    Vector2 padding;
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

// LLFF FUNCTIONS ARE USED TO CUSTOMIZE THE FREE LINKED LIST FUNCTION
void LLFFFreeFreedStuff(LinkedItem* item);
void LLFFFreeButtons(LinkedItem* item);
void LLFFFreeTexts(LinkedItem* item);
void LLFFDestroyBitmaps(LinkedItem* item);
void LLFFDestroyFonts(LinkedItem* item);
void LLFFFreeScenes(LinkedItem* item);
void LLFFDestroyAudioSamples(LinkedItem* item);
void LLFFDestroyAudioStreams(LinkedItem* item);
void LLFFDestroyAudioMixers(LinkedItem* item);
// ASSERT IF THE MODULE OF ALLEGRO IS OK
void assertInit(char result, const char* module);
// INITIALIZE THE ENGINE AND RETURNS THE `CAEngine*`
CAEngine* initEngine(GameConfig config);
// FREE THE `CAEngine*` AND ITS RESOURCES
void freeEngine(CAEngine* engine);
// ADD ALLEGRO EVENT SOURCE TO THE MAIN EVENT QUEUE OF THE ENGINE
void addEventSource(CAEngine* engine, ALLEGRO_EVENT_SOURCE* ev_source);
// SET THE EVENT FUNCTION TO THE ENGINE, THIS WILL RUN EVERY RENDER CALL, LIKE A CALLBACK FOR EVERY EVENT
void setEventFunction(CAEngine* engine, void (*f)(ALLEGRO_EVENT, Scene*, CAEngine*));
// FUNCTION THAT DRAWS A BUTTON
void renderButton(Button* button);
// FUNCTION THAT DRAWS A TEXT
void renderText(Text* text);
// FUNCTION THAT DRAWS A PROGRESS BAR
void renderProgressBar(ProgressBar* bar);
// THIS FUNCTION MAKES THE MAGIC OF TRANSFORMING THE X AND Y OF AN GAME OBJECT INTO A SCREEN X AND Y, MAINLY USED BY THE CAMERA FOR TAKING THE GIANT NUMBERS POSITIONS AND TURNING INTO SCREEN POSITIONS RELATIVE TO THE CAMERA OFFSET
void globalToLocal(Scene* scene, float* x, float* y);
// FUNCTION THAT MAKES MAGIC, PROCESSING ALL THE PHYSICS AND OTHER COOL STUFF AND FINALLY DRAWING ALL THAT IN THE DISPLAY, IMAGINE BEEING CALLED TO DRAW EVERY FRAME
void render(CAEngine* engine);
// CREATE A LINKED LIST STRUCTURE AND RETURNS THE `LinkedList*`
LinkedList* createLinkedList(void (*onDestroy)(LinkedItem* item));
// FREE A UNIQUE LINKED LIST ITEM
void freeLinkedItem(LinkedItem* item);
// DEPRECATED, NOT USED ANYMORE, STACK OVERFLOW!: A RECURIVE WAY TO FREE A LINKED LIST THAT IS CALLED BY THE FUNCTION `freeLinkedList(LinkedList* list)`, THIS FUNCTION ALSO CALLS THE LLFF type of function
void freeLinkedListItems(LinkedItem* item, LinkedList* list);
// THE FINAL FUNCTION TO FREE A LINKED LIST, THIS FUNCTION IS THE FUNCTION YOU NEED TO CALL TO FREE A LINKED LIST
void freeLinkedList(LinkedList* list);
// ADD AN ITEM TO A LINKED LIST
void addItemToLinkedList(LinkedList* list, void* data);
// REMOVE AN ITEM FROM A LINKED LIST
void removeItemLinkedList(LinkedList* list, void* searchData);
// SEARCH A DATA IN A LINKED LIST
LinkedItem* searchDataInLinkedList(LinkedList* list, void* data);
// MORE A DEBUG THING, BUT IF YOU WANT TO PRINT LIKE HOW IS THE ACTUAL STRUCT OF A LINKED LIST, CALL THIS
void printList(LinkedList* list);
// LOADS AN AUDIO SAMPLE AND RETURNS THE `ALLEGRO_SAMPLE*`, ALSO PUTS IN FINAL FREES LIST OF THE ENGINE
ALLEGRO_SAMPLE* loadAudioSample(CAEngine* engine, const char* path);
// LOADS AN AUDIO STREAM AND RETURNS THE `ALLEGRO_AUDIO_STREAM*`, ALSO PUTS IN FINAL FREES LIST OF THE ENGINE
ALLEGRO_AUDIO_STREAM* loadAudioStream(CAEngine* engine, const char* path, int buffers, int samples);
// PLAYS THE AUDIO SAMPLE AND RETURN ITS ID
ALLEGRO_SAMPLE_ID playAudioSample(ALLEGRO_SAMPLE* sample, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode);
// STOPS THE AUDIO SAMPLE BY THE ID
void stopAudioSample(ALLEGRO_SAMPLE_ID* sampleId);
// CONFIG OF AN AUDIO STREAM
void configureAudioStream(ALLEGRO_AUDIO_STREAM* stream, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode);
// PLAYS AND AUDIO STREAM
void playAudioStream(ALLEGRO_AUDIO_STREAM* stream);
// PAUSES AN AUDIO STREAM
void pauseAudioStream(ALLEGRO_AUDIO_STREAM* stream);
// STOPS AN AUDIO STREAM
void stopAudioStream(ALLEGRO_AUDIO_STREAM* stream);
// CREATES AN ALLEGRO AUDIO MIXER AND RETURNS THE `ALLEGRO_MIXER*`, ALSO PUTS IN FINAL FREES LIST OF THE ENGINE
ALLEGRO_MIXER* createAudioMixer(CAEngine* engine, unsigned int sampleRate);
// CONFIG OF AN AUDIO MIXER
void configureAudioMixer(ALLEGRO_MIXER* mixer, float gain);
// ADD AN AUDIO STREAM TO AN AUDIO MIXER
void addAudioStreamToMixer(ALLEGRO_MIXER* mixer, ALLEGRO_AUDIO_STREAM* stream);
// CREATE A SCENE, THINK A SCENE LIKE A LEVEL OR PART OF YOU GAME, RETURNS A `Scene*`, ALSO ADDS THIS TO THE FREES LIST OF THE ENGINE
Scene* createScene(CAEngine* engine, void (*scriptFunction)(Scene*));
// FREE A SCENE AND ITS RESOURCES, DON'T NEED TO CALL THIS FUNCTION BECAUSE THE ENGINE DOES IT WHEN FREE ENGINE
void freeScene(Scene* scene);
// SETUP THE WORLD OF THE SCENE, THINK LIKE A BACKGROUND IMAGE AND/OR THE GAME MAP
void setupSceneWorld(Scene* scene, ALLEGRO_BITMAP* tileSheet, int tileWidth, int tileHeight);
// ADD A TILE TO THE WORLD OF THE SCENE
void addWorldTile(Scene* scene, int idX, int idY, int tileX, int tileY);
// CREATES A GAME OBJECT AND RETURNS THE `GameObject*`, ALSO ADDS THIS TO THE FREES LIST OF THE ENGINE
GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height, Scene* scene);
// ADDS A CALLBACK FUNCTION TO A GAME OBJECT, WHEN THIS OBJECT COLLIDE WITH ANOTHER OBJECT, THE FUNCTION WILL BE CALLED
void setOnGameObjectCollisionFunction(GameObject* obj, void (*onCollision)(GameObject*, GameObject*));
// ADD A GAME OBJECT TO A SCENE
void addGameObjectToScene(Scene* scene, GameObject* obj);
// LOADS A BITMAP AND RETURNS THE `ALLEGRO_BITMAP*`, ALSO PUTS IN FINAL FREES LIST OF THE ENGINE
ALLEGRO_BITMAP* loadBitmap(CAEngine* engine, const char* pathToBitmap);
// CREATES A SUB BITMAP FROM A BITMAP, THIS IS USED TO VARIANTS OF THE ORIGINAL BITMAP, LIKE A SPRITE INSIDE A SPRITESHEET, ALSO PUTS IN FREES LIST OF THE ENGINE
ALLEGRO_BITMAP* createSubBitmap(CAEngine* engine, ALLEGRO_BITMAP* bitmap, int sx, int sy, int sw, int sh);
// CONFIG THE ANIMATION OF THE GAME OBJECT
void setGameObjectAnimation(GameObject* obj, ALLEGRO_BITMAP* bitmap, int frameWidth, int frameHeight, int totalFrames, float fps);
// IF YOU WANT TO INTERPRET A COLOR INSIDE THIS BITMAP AS TRANSPARENT, YOU CAN USE THIS FUNCTION
void setBitmapTransparentColor(ALLEGRO_BITMAP* bm, ALLEGRO_COLOR color);
// CALCULATES THE DISTANCE BETWEEN TWO GAME OBJECTS USING HYPOT
float dist(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
// CHECKS THE COLLISION OF TWO OBJECTS USING THE `dist` FUNCTION
char checkCollisionCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
// CHECKS THE COLLISION OF TWO OBJECTS USING THE `dist` FUNCTION, BUT INVERTED LIKE !RESPONSE
char checkCollisionInvertedCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
// CHECKS THE COLLISION OF TWO OBJECTS USING COMPARISON OPERATORS
char checkCollisionRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
// CHECKS THE COLLISION OF TWO OBJECTS USING COMPARISON OPERATORS, BUT INVERTED LIKE !RESPONSE
char checkCollisionInvertedRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
// CHANGE THE CURRENT RENDERING SCENE TO THE TARGET ONE
void changeScene(CAEngine* engine, Scene* scene);
// SETS THE GAME OBJECTS BITMAP
void setGameObjectBitmap(GameObject* obj, ALLEGRO_BITMAP* bitmap);
// LOADS A TTF FONT AND RETURNS THE `Font*`, ALSO PUTS IN FINAL FREES LIST OF THE ENGINE
Font* loadTTF(CAEngine* engine, const char* path, int size);
// CREATES A TEXT AND RETURNS THE `Text*`
Text* createText(const char* text, float x, float y, int width, ALLEGRO_COLOR color, ALLEGRO_COLOR backColor, ALLEGRO_BITMAP* backBitmap, Font* font, float paddingX, float paddingY, Scene* scene);
// ADD A Text* TO A SCENE IN UI
void addTextToScene(Scene* scene, Text* text);
// CREATES A BUTTON AND RETURNS THE `Button*`, ALSO ADDS THIS TO THE FREES LIST OF THE ENGINE
Button* createButton(CAEngine* engine, float x, float y, int width, int height, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor, const char* text, const char* pathToFontFile, ALLEGRO_BITMAP* bitmap, void (*onClick)(Scene*));
// ADDS A BUTTON TO A SCENE IN UI
void addButtonToScene(Scene* scene, Button* button);
// CREATES A PROGRESS BAR AND RETURNS THE `ProgressBar*`, ALSO ADDS THIS TO THE FREES LIST OF THE ENGINE
ProgressBar* createProgressBar(float x, float y, int width, int height, float initValue, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor);
// ADDS A PROGRESS BAR TO A SCENE IN UI
void addProgressBarToScene(Scene* scene, ProgressBar* bar);
// CHANGES THE TEXT OF A TEXT ELEMENT IN A SCENE
void changeText(Text* text, const char* newText);
// RANDOM INTEGER BETWEEN MIN AND MAX, INCLUDING BOTH
int randInt(int min, int max);
// RANDOM FLOAT BETWEEN MIN AND MAX, INCLUDING BOTH
float randFloat(int min, int max);
// CREATE A TEMP FILE AND RETURNS THE PATH OF THE FILE
void createTempFile(const char* b64Content, char* resultPath);
// CLOSE/DELETE THE TEMP FILE
void closeTempFile(const char* path);
// PLAY THE SPLASH SCREEN OF THE ENGINE
void playSplashScreen(CAEngine* engine);
// GET THE SPLASH SCREEN BASE64 CONTENT
char* getCAESplash();
// MULTIPLY THE VECTOR2 BY A CONSTANT
void multVector2(Vector2* vector2, float k);
// RETURNS A MOVEMENT VECTOR2 BASED ON THE INPUT KEYS
Vector2 getMovementVector2(ALLEGRO_KEYBOARD_STATE* kState, int keyLeft, int keyRight, int keyUp, int keyDown);
// SET THE AUTO TILE GENERATION FOR THE WORLD OF THE SCENE
void setSceneAutoTileGeneration(Scene* scene, int minIdX, int maxIdX, int minIdY, int maxIdY);

#endif
