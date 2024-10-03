#include "CAE.h"
#include <stdio.h>
#include <math.h>

// LLFF : (L)inked (L)ist (F)ree (F)unction

void LLFFFreeButtons(LinkedItem* item){
    Button* btn = item->data;
    free(btn->text);
    if (CAE_DEBUG)
        printf("\nFreed a Button!");
}

void LLFFFreeTexts(LinkedItem* item){
    Text* txt = item->data;
    free(txt->text);
    if (CAE_DEBUG)
        printf("\nFreed a Text!");
}

void LLFFDestroyBitmaps(LinkedItem* item){
    al_destroy_bitmap((ALLEGRO_BITMAP*)item->data);
    item->data=NULL;
    if (CAE_DEBUG)
        printf("\nFreed a Bitmap!");
}

void LLFFDestroyFonts(LinkedItem* item){
    Font* font = (Font*)item->data;
    al_destroy_font(font->font);
    if (CAE_DEBUG)
        printf("\nFreed a Font!");
}

void LLFFFreeScenes(LinkedItem* item){
    freeScene((Scene*)item->data);
    item->data=NULL;
    if (CAE_DEBUG)
        printf("\nFreed a Scene!");
}

void LLFFDestroyAudioSamples(LinkedItem* item){
    al_destroy_sample((ALLEGRO_SAMPLE*)item->data);
    item->data=NULL;
    if (CAE_DEBUG)
        printf("\nFreed an Audio Sample!");
}

void LLFFDestroyAudioStreams(LinkedItem* item){
    al_destroy_audio_stream((ALLEGRO_AUDIO_STREAM*)item->data);
    item->data=NULL;
    if (CAE_DEBUG)
        printf("\nFreed an Audio Stream!");
}

void LLFFDestroyAudioMixers(LinkedItem* item){
    al_destroy_mixer((ALLEGRO_MIXER*)item->data);
    item->data=NULL;
    if (CAE_DEBUG)
        printf("\nFreed an Audio Mixer!");
}

void assertInit(char result, const char* module){
    if (!result){
        printf("\n%s Module failed to initialize!", module);
        return;
    }
    printf("\n%s Module initialized successfully!", module);
}

CAEngine* initEngine(GameConfig config){
    assertInit(al_init(), "Allegro");
    assertInit(al_init_primitives_addon(), "Primitives");
    assertInit(al_init_image_addon(), "Image");
    assertInit(al_install_keyboard(), "Keyboard");
    assertInit(al_install_mouse(), "Mouse");
    assertInit(al_init_font_addon(), "Font");
    assertInit(al_init_ttf_addon(), "TTF");
    assertInit(al_install_audio(), "Audio");
    assertInit(al_init_acodec_addon(), "Audio Codecs");
    assertInit(al_reserve_samples(CAE_RESERVE_SAMPLES), "Audio Samples");
    assertInit(al_init_video_addon(), "Video");

    CAEngine* engine = (CAEngine*)malloc(sizeof(CAEngine));
    if (config.fullscreen){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        ALLEGRO_MONITOR_INFO info;
        al_get_monitor_info(0, &info);
        config.sizeX = info.x2 - info.x1;
        config.sizeY = info.y2 - info.y1;
    }

    engine->display = al_create_display(config.sizeX, config.sizeY);
    engine->displayWidth = config.sizeX;
    engine->displayHeight = config.sizeY;
    if (!config.fullscreen){
        al_set_window_position(engine->display, config.posX, config.posY);
    }
    al_set_window_title(engine->display, config.title);
    engine->ev_queue = al_create_event_queue();
    engine->timer = al_create_timer(1.0 / config.fps);
    engine->fps=config.fps;
    engine->isAlive = 1;
    engine->bitmaps = createLinkedList(LLFFDestroyBitmaps);
    engine->scenes = createLinkedList(LLFFFreeScenes);
    engine->fonts = createLinkedList(LLFFDestroyFonts);
    engine->audioSamples = createLinkedList(LLFFDestroyAudioSamples);
    engine->audioStreams = createLinkedList(LLFFDestroyAudioStreams);
    engine->audioMixers = createLinkedList(LLFFDestroyAudioMixers);

    engine->currentScene=NULL;

    al_register_event_source(engine->ev_queue, al_get_display_event_source(engine->display));
    al_register_event_source(engine->ev_queue, al_get_keyboard_event_source());
    al_register_event_source(engine->ev_queue, al_get_mouse_event_source()); 
    al_register_event_source(engine->ev_queue, al_get_timer_event_source(engine->timer));

    al_start_timer(engine->timer);

    engine->startTime=time(NULL);
    srand(engine->startTime-2);

    return engine;
}

void freeEngine(CAEngine* engine){
    freeLinkedList(engine->scenes);
    freeLinkedList(engine->bitmaps);
    freeLinkedList(engine->fonts);
    freeLinkedList(engine->audioSamples);
    freeLinkedList(engine->audioStreams);
    freeLinkedList(engine->audioMixers);
    al_destroy_display(engine->display);
    al_destroy_event_queue(engine->ev_queue);
    al_destroy_timer(engine->timer);
    al_uninstall_keyboard();
    al_uninstall_mouse();
    free(engine);
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_shutdown_image_addon();
    al_shutdown_ttf_addon();
    al_uninstall_audio();
    al_shutdown_video_addon();
}

void addEventSource(CAEngine* engine, ALLEGRO_EVENT_SOURCE* ev_source){
    al_register_event_source(engine->ev_queue, ev_source);
}

void setEventFunction(CAEngine* engine, void (*f)(ALLEGRO_EVENT, Scene*, CAEngine*)){
    engine->eventFunction = f;
}

void renderButton(Button* button){
    if (button->visible){
        if (button->bitmap != NULL){
            al_draw_scaled_bitmap(button->bitmap, 0, 0, al_get_bitmap_width(button->bitmap), al_get_bitmap_height(button->bitmap), button->position.x, button->position.y, button->width, button->height, 0);
            return;
        }

        al_draw_filled_rounded_rectangle(button->position.x, button->position.y, button->position.x+button->width, button->position.y+button->height, 5, 5, button->backgroundColor);
        float x = button->position.x+button->width/2-al_get_text_width(button->font->font, button->text)/2;
        float y = button->position.y+button->height/2-button->font->size/2;
        al_draw_text(button->font->font, button->foregroundColor, x, y, 0, button->text);
    }
}

void renderText(Text* text){
    if (text->visible){
        al_draw_text(text->font->font, text->color, text->position.x, text->position.y, 0, text->text);
    }
}

void renderProgressBar(ProgressBar* bar){
    if (bar->visible){
        if (bar->value < 0.)
            bar->value = 0.;
        else if (bar->value > 100.)
            bar->value = 100.;
        
        if (bar->value != 100.)
            al_draw_filled_rounded_rectangle(bar->position.x, bar->position.y, bar->position.x+bar->width, bar->position.y+bar->height, 5, 5, bar->backgroundColor);
        if (bar->value != 0.)
            al_draw_filled_rounded_rectangle(bar->position.x, bar->position.y, bar->position.x+bar->width*(bar->value/100), bar->position.y+bar->height, 5, 5, bar->foregroundColor);
    }
}

// TRANSFORM GLOBAL POSITION TO CAMERA POSITION
void globalToLocal(Scene* scene, float* x, float* y){
    if (*x < 0 && scene->camera.offset.x < 0)
        *x=fabs(*x) - fabs(scene->camera.offset.x);
    else
        *x-=scene->camera.offset.x;
    if (*y < 0 && scene->camera.offset.y < 0)
        *y=fabs(*y) - fabs(scene->camera.offset.y);
    else
        *y-=scene->camera.offset.y;
    if (*x > scene->camera.offset.x)
        *x=fabs(*x);
    if (*y > scene->camera.offset.y)
        *y=fabs(*y);
}

void render(CAEngine* engine){
    Scene* scene = engine->currentScene;
    char drawTime = 0;
    do {
        ALLEGRO_EVENT ev;
        al_wait_for_event(engine->ev_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            engine->isAlive=0;
            return;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (ev.timer.source == engine->timer)
                drawTime = 1;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && scene != NULL) {
            if (scene->ui.visible){
                ALLEGRO_MOUSE_STATE state;
                al_get_mouse_state(&state);
                if (al_mouse_button_down(&state, 1)){
                    LinkedItem* item = scene->ui.buttons->first;
                    while(item != NULL){
                        Button* btn = (Button*)item->data;
                        if (!btn->visible){
                            item = item->next;
                            continue;
                        }
                        if (ev.mouse.x >= btn->position.x && ev.mouse.x <= btn->position.x+btn->width && ev.mouse.y >= btn->position.y && ev.mouse.y <= btn->position.y+btn->height){
                            if (btn->onClick != NULL)
                                btn->onClick(scene);
                            break;
                        }
                        item = item->next;
                    }
                }
            }
        }

        if (scene != NULL && engine->eventFunction != NULL)
            engine->eventFunction(ev, scene, engine);

    } while(!al_is_event_queue_empty(engine->ev_queue));

    if (scene == NULL || !drawTime)
        return;

    // FADE EFFECT
    if (scene->fadeIn.value < 0){
        scene->fadeIn.value=0;
    } else{
        scene->fadeIn.value-=255/engine->fps*scene->fadeIn.speed;
    }

    al_clear_to_color(scene->backgroundColor);

    // RENDER WORLD TILES
    if (scene->world != NULL){
        LinkedItem* item = scene->world->tiles->first;
        while (item != NULL){
            Tile* tile = (Tile*)item->data;
            float x = tile->x*scene->world->tileWidth;
            float y = tile->y*scene->world->tileHeight;
            // NOT DRAW IF NOT VISIBLE IN CAMERA
            if ((x+scene->world->tileWidth > scene->camera.offset.x && x < scene->camera.offset.x+engine->displayWidth) && (y+scene->world->tileHeight > scene->camera.offset.y && y < scene->camera.offset.y+engine->displayHeight)){
                globalToLocal(scene, &x, &y);
                al_draw_scaled_bitmap(scene->world->tileSheet, tile->idX*scene->world->tileWidth, tile->idY*scene->world->tileHeight, scene->world->tileWidth, scene->world->tileHeight, x, y, scene->world->tileWidth, scene->world->tileHeight, 0);
            }
            item = item->next;
        }
    }

    // HERE: USER FUNCTION TO MANIPULATE THE SCENE (Objects positions for example)
    if (scene->scriptFunction != NULL)
        scene->scriptFunction(scene);

    // CAMERA FOLLOW ACTION
    if (scene->camera.followTarget != NULL){
        if (scene->camera.followTarget->visible){
            char dirX=0;
            char dirY=0;
            int viewX=scene->camera.offset.x+engine->displayWidth/2;
            int viewY=scene->camera.offset.y+engine->displayHeight/2;
            int targetX=scene->camera.followTarget->position.x+scene->camera.followTarget->width/2;
            int targetY=scene->camera.followTarget->position.y+scene->camera.followTarget->height/2;
            int difX=abs(targetX-viewX);
            int difY=abs(targetY-viewY);

            if (targetX > viewX && difX > 3){
                dirX=1;
            } else if(targetX < viewX && difX > 3){
                dirX=-1;
            } else{
                scene->camera.followSpeed.x=0;
            }
            if (targetY > viewY && difY > 3){
                dirY=1;
            } else if(targetY < viewY && difY > 3){
                dirY=-1;
            } else{
                scene->camera.followSpeed.y=0;
            }
            if (scene->camera.followSpeed.x < scene->camera.followMaxSpeed){
                scene->camera.followSpeed.x+=scene->camera.followAcc;
            } else{
                scene->camera.followSpeed.x=scene->camera.followMaxSpeed;
            } 
            if (scene->camera.followSpeed.y < scene->camera.followMaxSpeed){
                scene->camera.followSpeed.y+=scene->camera.followAcc;
            } else{
                scene->camera.followSpeed.y=scene->camera.followMaxSpeed;
            }

            scene->camera.offset.x+=scene->camera.followSpeed.x*dirX;
            scene->camera.offset.y+=scene->camera.followSpeed.y*dirY;
        }
    }

    // CAMERA OFFSET LIMIT
    if (scene->camera.minLimit.x != scene->camera.maxLimit.x && scene->camera.minLimit.y != scene->camera.maxLimit.y){
        if (scene->camera.offset.x < scene->camera.minLimit.x){
            scene->camera.offset.x=scene->camera.minLimit.x;
        } else if (scene->camera.offset.x+engine->displayWidth > scene->camera.maxLimit.x){
            scene->camera.offset.x=scene->camera.maxLimit.x-engine->displayWidth;
        } 
        if (scene->camera.offset.y < scene->camera.minLimit.y){
            scene->camera.offset.y=scene->camera.minLimit.y;
        } else if (scene->camera.offset.y+engine->displayHeight > scene->camera.maxLimit.y){
            scene->camera.offset.y=scene->camera.maxLimit.y-engine->displayHeight;
        }
    }

    // HERE: MY FUNCTION TO MAKE THE RENDER MAGIC, CAMERA ETC
    LinkedItem* item=scene->objects->first;
    while (item!=NULL){
        GameObject* obj=item->data;

        if (!obj->visible){
            item=item->next;
            continue;    
        }

        float x=obj->position.x;
        float y=obj->position.y;
        // PHYSICS PROCESS
        if (obj->physics.enabled){
            // SPEED BY ACCELERATION X
            //int xDir = obj->physics.speed.x/fabsf(obj->physics.speed.x)*-1;
            int xDir = 1;
            if (obj->physics.speed.x < 0)
                xDir=-1;
            obj->physics.speed.x += obj->physics.acc.x*obj->physics.directions.x-obj->physics.friction*xDir;
            if (obj->physics.acc.x == 0 && round(obj->physics.speed.x) == 0){
                obj->physics.speed.x=0;
                obj->physics.directions.x=0;
            }
            if (obj->physics.speed.x > obj->physics.maxSpeed || obj->physics.speed.x < -obj->physics.maxSpeed){
                if (obj->physics.speed.x < 0){
                    obj->physics.speed.x=-obj->physics.maxSpeed;
                } else{
                    obj->physics.speed.x=obj->physics.maxSpeed;
                }
            }

            // SPEED BY ACCELERATION Y
            int yDir = 1;
            if (obj->physics.speed.y < 0)
                yDir=-1;
            obj->physics.speed.y += obj->physics.acc.y*obj->physics.directions.y-obj->physics.friction*yDir;
            if (obj->physics.acc.y == 0 && round(obj->physics.speed.y) == 0){
                obj->physics.speed.y=0;
                obj->physics.directions.y=0;
            }
            if (obj->physics.speed.y > obj->physics.maxSpeed || obj->physics.speed.y < -obj->physics.maxSpeed){
                if (obj->physics.speed.y < 0){
                    obj->physics.speed.y=-obj->physics.maxSpeed;
                } else{
                    obj->physics.speed.y=obj->physics.maxSpeed;
                }
            }

            // GRAVITY
            if (obj->physics.gravity){
                obj->physics.gravitySpeed+=scene->gravityValue;
                y+=obj->physics.gravitySpeed;
            }

            // obj->position.x+=obj->physics.speed*obj->physics.directions.x;
            // obj->position.y+=obj->physics.speed*obj->physics.directions.y;
            x+=obj->physics.speed.x;
            y+=obj->physics.speed.y;
        }

        // COLLISIONS
        unsigned char hasCollision=0;
        if (obj->collisionEnabled){
            LinkedItem* item2 = scene->objects->first;
            while (item2!=NULL){
                GameObject* obj2 = item2->data;
                if (!obj2->visible){
                    item2=item2->next;
                    continue;
                }
                if (obj2->collisionEnabled && obj2!=obj){
                    switch(obj->collisionType){
                        case COLLISION_RECT:
                            if (!obj2->invertedCollision){
                                hasCollision=checkCollisionRect(
                                    x+obj->startCollisionOffset.x, 
                                    y+obj->startCollisionOffset.y, 
                                    obj->width+obj->endCollisionOffset.x-obj->startCollisionOffset.x, 
                                    obj->height+obj->endCollisionOffset.y-obj->startCollisionOffset.y, 
                                    obj2->position.x+obj2->startCollisionOffset.x, 
                                    obj2->position.y+obj2->startCollisionOffset.y, 
                                    obj2->width+obj2->endCollisionOffset.x-obj2->startCollisionOffset.x, 
                                    obj2->height+obj2->endCollisionOffset.y-obj2->startCollisionOffset.y
                                );
                            } else {
                                hasCollision=checkCollisionInvertedRect(
                                    x+obj->startCollisionOffset.x, 
                                    y+obj->startCollisionOffset.y, 
                                    obj->width+obj->endCollisionOffset.x-obj->startCollisionOffset.x, 
                                    obj->height+obj->endCollisionOffset.y-obj->startCollisionOffset.y, 
                                    obj2->position.x+obj2->startCollisionOffset.x, 
                                    obj2->position.y+obj2->startCollisionOffset.y, 
                                    obj2->width+obj2->endCollisionOffset.x-obj2->startCollisionOffset.x, 
                                    obj2->height+obj2->endCollisionOffset.y-obj2->startCollisionOffset.y
                                );
                            }
                            break;
                        case COLLISION_CIRCLE:
                            if (!obj2->invertedCollision){
                                hasCollision=checkCollisionCircle(
                                    x+obj->startCollisionOffset.x, 
                                    y+obj->startCollisionOffset.y, 
                                    obj->width+obj->endCollisionOffset.x-obj->startCollisionOffset.x, 
                                    obj->height+obj->endCollisionOffset.y-obj->startCollisionOffset.y, 
                                    obj2->position.x+obj2->startCollisionOffset.x, 
                                    obj2->position.y+obj2->startCollisionOffset.y, 
                                    obj2->width+obj2->endCollisionOffset.x-obj2->startCollisionOffset.x, 
                                    obj2->height+obj2->endCollisionOffset.y-obj2->startCollisionOffset.y
                                );
                            } else {
                                hasCollision=checkCollisionInvertedCircle(
                                    x+obj->startCollisionOffset.x, 
                                    y+obj->startCollisionOffset.y, 
                                    obj->width+obj->endCollisionOffset.x-obj->startCollisionOffset.x, 
                                    obj->height+obj->endCollisionOffset.y-obj->startCollisionOffset.y, 
                                    obj2->position.x+obj2->startCollisionOffset.x, 
                                    obj2->position.y+obj2->startCollisionOffset.y, 
                                    obj2->width+obj2->endCollisionOffset.x-obj2->startCollisionOffset.x, 
                                    obj2->height+obj2->endCollisionOffset.y-obj2->startCollisionOffset.y
                                );
                            }
                            break;
                        default:
                            break;
                    }
                    if (hasCollision){
                        if (obj->onCollision != NULL)
                            obj->onCollision(obj, obj2);
                        break;
                    }
                }
                item2=item2->next;
            }
        }

        if (!hasCollision){
            obj->position.x=x;
            obj->position.y=y;
        } else if (obj->physics.enabled && obj->physics.gravity){
            y-=obj->physics.gravitySpeed+scene->gravityValue;
            obj->physics.gravitySpeed=0;
            obj->position.y=y;
        }

        // IS OBJECT NOT VISIBLE IN CAMERA
        if (!((obj->position.x+obj->width > scene->camera.offset.x && obj->position.x < scene->camera.offset.x+engine->displayWidth) && (obj->position.y+obj->height > scene->camera.offset.y && obj->position.y < scene->camera.offset.y+engine->displayHeight))){
            item=item->next;
            continue;
        }

        // TRANSFORM THE GLOBAL POSITION OF EVERYTHING IN LOCAL POSITION
        // if (x < 0 && scene->camera.offset.x < 0)
        //     x=fabs(x) - fabs(scene->camera.offset.x);
        // else
        //     x-=scene->camera.offset.x;
        // if (y < 0 && scene->camera.offset.y < 0)
        //     y=fabs(y) - fabs(scene->camera.offset.y);
        // else
        //     y-=scene->camera.offset.y;

        // if (x > scene->camera.offset.x)
        //     x=fabs(x);
        // if (y > scene->camera.offset.y)
        //     y=fabs(y);

        globalToLocal(scene, &x, &y);
        
        // DRAW EACH TYPE OF GAME OBJECT
        switch (obj->type){
            case SOLID:
                al_draw_filled_rectangle(x, y, x+obj->width, y+obj->height, obj->color);
                break;
            case SPRITE:
                al_draw_scaled_bitmap(obj->bitmap, 0, 0, al_get_bitmap_width(obj->bitmap), al_get_bitmap_height(obj->bitmap), x, y, obj->width, obj->height, 0);
                break;
            case ANIMATED_SPRITE:
                if ((int)obj->animation.index.x > obj->animation.totalFrames-1)
                    obj->animation.index.x=0;
                //printf("Index: %d\n", obj->animation.width);
                if (obj->animation.direction.x<0)
                    x+=fabs(obj->width*obj->animation.direction.x);
                if (obj->animation.direction.y<0)
                    y+=fabs(obj->height*obj->animation.direction.y);
                al_draw_scaled_bitmap(obj->bitmap, ((int)obj->animation.index.x)*obj->animation.width, ((int)obj->animation.index.y)*obj->animation.height, obj->animation.width, obj->animation.height, x, y, obj->width*obj->animation.direction.x, obj->height*obj->animation.direction.y, 0);
                obj->animation.index.x+=obj->animation.fps;
                break;
            default:
                break;
        }
        item=item->next;
    }

    // RENDER UI LAYER
    if (scene->ui.visible){
        // BUTTONS
        LinkedItem* item=scene->ui.buttons->first;
        while (item!=NULL){
            renderButton((Button*)item->data);
            item=item->next;
        }
        // TEXTS
        item=scene->ui.texts->first;
        while (item!=NULL){
            renderText((Text*)item->data);
            item=item->next;
        }
        // PROGRESS BARS
        item=scene->ui.progressBars->first;
        while (item!=NULL){
            renderProgressBar((ProgressBar*)item->data);
            item=item->next;
        }
    }
    
    // FADE EFFECT
    if (scene->fadeIn.speed > 0 && (int)scene->fadeIn.value > 0)
        al_draw_filled_rectangle(0, 0, engine->displayWidth, engine->displayHeight, al_map_rgba(0, 0, 0, (int)scene->fadeIn.value));

    al_flip_display();
}

LinkedList* createLinkedList(void (*onDestroy)(LinkedItem* item)){
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->length=0;
    list->first=NULL;
    list->last=NULL;
    list->onDestroy=onDestroy;
    return list;
}

void freeLinkedItem(LinkedItem* item){
    free(item->data);
    free(item);
}

void freeLinkedListItems(LinkedItem* item, LinkedList* list){
    if (item == NULL){
        return;
    }
    freeLinkedListItems(item->next, list);
    if (list->onDestroy != NULL)
        list->onDestroy(item);
    freeLinkedItem(item);
    if (CAE_DEBUG)
        printf("\nFreed item!");
}

void freeLinkedList(LinkedList* list){
    freeLinkedListItems(list->first, list);
    free(list);
    if (CAE_DEBUG)
        printf("\n");
}

void addItemToLinkedList(LinkedList* list, void* data){
    list->length++;
    LinkedItem* newItem = (LinkedItem*)malloc(sizeof(LinkedItem));
    newItem->next=NULL;
    newItem->data=data;
    // IF FIRST OBJECT IN LIST
    if (list->first == NULL && list->last == NULL){
        list->first=newItem;
        list->last=newItem;
        return;
    }
    // IF IS NOT THE FIRST OBJECT
    list->last->next=newItem;
    list->last=newItem;
}

void removeItemLinkedList(LinkedList* list, void* searchData){
    LinkedItem* item = list->first;
    // EMPTY LIST
    if (list->first == NULL && list->last == NULL)
        return;
    // FIRST ITEM
    else if (item->data == searchData){
        list->first=item->next;
        if (list->last == item)
            list->last=NULL;
        freeLinkedItem(item);
        list->length--;
        return;
    }
    while (item != NULL){
        if (item->next->data == searchData){
            LinkedItem* itemToExclude = item->next;
            item->next=itemToExclude->next;
            freeLinkedItem(itemToExclude);
            if (item->next == NULL){
                list->last=item;
            }
            list->length--;
            return;
        }
        item=item->next;
    }
}

void printList(LinkedList* list){
    printf("\n_LENGTH: %d_", list->length);
    LinkedItem* i = list->first;
    while (i != NULL){
        printf("\n(%p) -> %p", i, i->next);
        i=i->next;
    }
    printf("\n");
}

ALLEGRO_SAMPLE* loadAudioSample(CAEngine* engine, const char* path){
    ALLEGRO_SAMPLE* sample = al_load_sample(path);
    addItemToLinkedList(engine->audioSamples, sample);
    return sample;
}

ALLEGRO_AUDIO_STREAM* loadAudioStream(CAEngine* engine, const char* path, int buffers, int samples){
    ALLEGRO_AUDIO_STREAM* stream = al_load_audio_stream(path, buffers, samples);
    al_attach_audio_stream_to_mixer(stream, al_get_default_mixer());
    addItemToLinkedList(engine->audioStreams, stream);
    return stream;
}

ALLEGRO_SAMPLE_ID playAudioSample(ALLEGRO_SAMPLE* sample, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode){
    ALLEGRO_SAMPLE_ID id;
    al_play_sample(sample, gain, pan, speed, playMode, &id);
    return id;
}

void stopAudioSample(ALLEGRO_SAMPLE_ID* sampleId){
    al_stop_sample(sampleId);
}

void configureAudioStream(ALLEGRO_AUDIO_STREAM* stream, float gain, float pan, float speed, ALLEGRO_PLAYMODE playMode){
    al_set_audio_stream_gain(stream, gain);
    al_set_audio_stream_pan(stream, pan);
    al_set_audio_stream_speed(stream, speed);
    al_set_audio_stream_playmode(stream, playMode);
}

void playAudioStream(ALLEGRO_AUDIO_STREAM* stream){
    al_set_audio_stream_playing(stream, 1);
}

void pauseAudioStream(ALLEGRO_AUDIO_STREAM* stream){
    al_set_audio_stream_playing(stream, 0);
}

void stopAudioStream(ALLEGRO_AUDIO_STREAM* stream){
    al_set_audio_stream_playing(stream, 0);
    al_rewind_audio_stream(stream);
}

ALLEGRO_MIXER* createAudioMixer(CAEngine* engine, unsigned int sampleRate){
    ALLEGRO_MIXER* mixer = al_create_mixer(sampleRate, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    al_attach_mixer_to_voice(mixer, al_get_default_voice());
    addItemToLinkedList(engine->audioMixers, mixer);
    return mixer;
}

void configureAudioMixer(ALLEGRO_MIXER* mixer, float gain){
    al_set_mixer_gain(mixer, gain);
}

void addAudioStreamToMixer(ALLEGRO_MIXER* mixer, ALLEGRO_AUDIO_STREAM* stream){
    al_attach_audio_stream_to_mixer(stream, mixer);
}

Scene* createScene(CAEngine* engine, void (*scriptFunction)(Scene*)){
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    scene->objects = createLinkedList(NULL);
    scene->camera.offset.x=0.;
    scene->camera.offset.y=0.;
    scene->camera.followTarget=NULL;
    scene->camera.followAcc=0.1;
    scene->camera.followSpeed.x=scene->camera.followSpeed.y=0;
    scene->camera.followMaxSpeed=4;
    scene->camera.maxLimit.x=scene->camera.maxLimit.y=0;
    scene->camera.minLimit.x=scene->camera.minLimit.y=0;
    scene->scriptFunction=scriptFunction;
    scene->gravityValue=0.1;
    scene->backgroundColor=al_map_rgb(30, 30, 30);
    scene->ui.buttons=createLinkedList(LLFFFreeButtons);
    scene->ui.texts=createLinkedList(LLFFFreeTexts);
    scene->ui.progressBars=createLinkedList(NULL);
    scene->ui.visible=1;
    scene->world=NULL;
    scene->fadeIn.speed=1;
    scene->fadeIn.value=0;
    
    addItemToLinkedList(engine->scenes, scene);
    return scene;
}

void freeScene(Scene* scene){
    freeLinkedList(scene->objects);
    freeLinkedList(scene->ui.buttons);
    freeLinkedList(scene->ui.texts);
    freeLinkedList(scene->ui.progressBars);
    if (scene->world != NULL){
        freeLinkedList(scene->world->tiles);
        free(scene->world);
    }
    free(scene);
}

void setupSceneWorld(Scene* scene, ALLEGRO_BITMAP* tileSheet, int tileWidth, int tileHeight){
    scene->world = (World*)malloc(sizeof(World));
    scene->world->tileSheet=tileSheet;
    scene->world->tileWidth=tileWidth;
    scene->world->tileHeight=tileHeight;
    scene->world->tiles=createLinkedList(NULL);
}

void addWorldTile(Scene* scene, int idX, int idY, int tileX, int tileY){
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->idX=idX;
    tile->idY=idY;
    tile->x=tileX;
    tile->y=tileY;
    addItemToLinkedList(scene->world->tiles, tile);
}

GameObject* createGameObject(enum OBJECT_TYPE type, float x, float y, int width, int height, Scene* scene){
    GameObject* newObj = (GameObject*)malloc(sizeof(GameObject));
    newObj->type=type;
    newObj->position.x=x;
    newObj->position.y=y;
    newObj->width=width;
    newObj->height=height;
    newObj->color=al_map_rgb(0, 0, 0);
    newObj->physics.acc.x=0;
    newObj->physics.acc.y=0;
    newObj->physics.enabled=0;
    newObj->physics.gravity=0;
    newObj->physics.directions.x=0;
    newObj->physics.directions.y=0;
    newObj->physics.friction=0;
    newObj->physics.maxSpeed=0;
    newObj->physics.speed.x=0;
    newObj->physics.speed.y=0;
    newObj->physics.gravitySpeed=0;
    newObj->bitmap=NULL;
    newObj->animation.direction.x=1;
    newObj->animation.direction.y=1;
    newObj->collisionEnabled=0;
    newObj->collisionType=COLLISION_RECT;
    newObj->onCollision=NULL;
    newObj->startCollisionOffset.x=newObj->startCollisionOffset.y=0;
    newObj->endCollisionOffset.x=newObj->endCollisionOffset.y=0;
    newObj->invertedCollision=0;
    newObj->visible=1;

    addGameObjectToScene(scene, newObj);

    return newObj;
}

void setOnGameObjectCollisionFunction(GameObject* obj, void (*onCollision)(GameObject*, GameObject*)){
    obj->onCollision=onCollision;
}

void addGameObjectToScene(Scene* scene, GameObject* obj){
    // VERIFY IF IS ALREADY IN THE SCENE
    LinkedItem* item = scene->objects->first;
    while (item != NULL){
        if (item->data == obj){
            return;
        }
        item=item->next;
    }
    addItemToLinkedList(scene->objects, obj);
}

ALLEGRO_BITMAP* loadBitmap(CAEngine* engine, const char* pathToBitmap){
    ALLEGRO_BITMAP* bm = al_load_bitmap(pathToBitmap);
    addItemToLinkedList(engine->bitmaps, bm);
    return bm;
}

ALLEGRO_BITMAP* createSubBitmap(CAEngine* engine, ALLEGRO_BITMAP* bitmap, int sx, int sy, int sw, int sh){
    ALLEGRO_BITMAP* subBitmap = al_create_sub_bitmap(bitmap, sx, sy, sw, sh);
    addItemToLinkedList(engine->bitmaps, subBitmap);
    return subBitmap;
}

void setGameObjectAnimation(GameObject* obj, ALLEGRO_BITMAP* bitmap, int frameWidth, int frameHeight, int totalFrames, float fps){
    if (obj->type != ANIMATED_SPRITE)
       return;
    obj->bitmap = bitmap;
    obj->animation.index.x=0;
    obj->animation.index.y=0;
    obj->animation.width=frameWidth;
    obj->animation.height=frameHeight;
    obj->animation.totalFrames=totalFrames;
    obj->animation.fps=fps;
    if (fps > 0)
        obj->animation.fps/=100;
}

void setBitmapTransparentColor(ALLEGRO_BITMAP* bm, ALLEGRO_COLOR color){
    al_convert_mask_to_alpha(bm, color);
}

// float dist(GameObject* a, GameObject* b){
//     return sqrt(pow((a->x+a->width/2.) - (b->x+b->width/2.), 2) + pow((a->y+a->height/2.) - (b->y+b->height/2.), 2));
// }

float dist(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    return sqrt(pow((x1+w1/2.) - (x2+w2/2.), 2) + pow((y1+h1/2.) - (y2+h2/2.), 2));
}

char checkCollisionCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    int d = (int)dist(x1, y1, w1, h1, x2, y2, w2, h2);
    if((d<=(w1/2+w2/2))||(d<=(h1/2+h2/2)))
        return 1;
    return 0;
}

char checkCollisionInvertedCircle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    int d = (int)dist(x1, y1, w1, h1, x2, y2, w2, h2);
    if((d<=(w1/2+w2/2))||(d<=(h1/2+h2/2)))
        return 0;
    return 1;
}

char checkCollisionRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    return x1+w1 > x2 && x1 < x2+w2 && y1+h1 > y2 && y1 < y2+h2;
}

char checkCollisionInvertedRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    return x1 <= x2 || x1+w1 >= x2 + w2 || y1 <= y2 || y1+h1 >= y2 + h2;
}

void changeScene(CAEngine* engine, Scene* scene){
    scene->fadeIn.value=255;
    engine->currentScene=scene;
}

void setGameObjectBitmap(GameObject* obj, ALLEGRO_BITMAP* bitmap){
    obj->bitmap=bitmap;
}

Font* loadTTF(CAEngine* engine, const char* path, int size){
    Font* font = (Font*)malloc(sizeof(Font));
    ALLEGRO_FONT* ttf = al_load_ttf_font(path, size, 0);
    font->font=ttf;
    font->size=size;
    addItemToLinkedList(engine->fonts, font);
    return font;
}

Text* createText(const char* text, float x, float y, ALLEGRO_COLOR color, Font* font){
    Text* textObj = (Text*)malloc(sizeof(Text));
    char* t = (char*)malloc(sizeof(char)*strlen(text)+1);
    strcpy(t, text);
    textObj->text=t;
    textObj->position.x=x;
    textObj->position.y=y;
    textObj->color=color;
    textObj->font=font;
    textObj->visible=1;
    return textObj;
}

void addTextToScene(Scene* scene, Text* text){
    addItemToLinkedList(scene->ui.texts, text);
}

Button* createButton(CAEngine* engine, float x, float y, int width, int height, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor, const char* text, const char* pathToFontFile, ALLEGRO_BITMAP* bitmap, void (*onClick)(Scene*)){
    Button* button = (Button*)malloc(sizeof(Button));
    button->position.x=x;
    button->position.y=y;
    button->width=width;
    button->height=height;
    button->onClick=onClick;
    button->visible=1;
    button->bitmap=bitmap;
    char* btnTxt = (char*)malloc(sizeof(char) * strlen(text) + 1);
    strcpy(btnTxt, text);
    button->text=btnTxt;
    button->backgroundColor=backgroundColor;
    button->foregroundColor=foregroundColor;
    button->font=loadTTF(engine, pathToFontFile, round(height/2));
    int textWidth=al_get_text_width(button->font->font, btnTxt);
    if (textWidth > width-20)
        button->width=textWidth+20;
    return button;
}

void addButtonToScene(Scene* scene, Button* button){
    addItemToLinkedList(scene->ui.buttons, button);
}

ProgressBar* createProgressBar(float x, float y, int width, int height, float initValue, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR foregroundColor){
    ProgressBar* bar = (ProgressBar*)malloc(sizeof(ProgressBar));
    bar->position.x=x;
    bar->position.y=y;
    bar->width=width;
    bar->height=height;
    bar->backgroundColor=backgroundColor;
    bar->foregroundColor=foregroundColor;
    bar->visible=1;
    if (initValue < 0)
        initValue=0.;
    else if (initValue > 100)
        initValue=100.;
    bar->value=initValue;
    return bar;
}

void addProgressBarToScene(Scene* scene, ProgressBar* bar){
    addItemToLinkedList(scene->ui.progressBars, bar);
}

void changeText(Text* text, const char* newText){
    char* t = (char*)malloc(sizeof(char)*strlen(newText)+1);
    strcpy(t, newText);
    free(text->text);
    text->text=t;
}

int randInt(int min, int max){
    return rand() % (max-min+1) + min;
}

// void playSplashScreen(CAEngine* engine){
//     ALLEGRO_VIDEO* splash = al_open_video("./splash.mp4");
//     if (!splash){
//         printf("\nFailed to load splash video");
//         return;
//     }
//     al_start_video(splash, al_get_default_mixer());
//     while(al_is_video_playing(splash)){
//         ALLEGRO_BITMAP* frame = al_get_video_frame(splash);
//         al_draw_scaled_bitmap(frame, 0, 0, al_get_bitmap_width(frame), al_get_bitmap_height(frame), 0, 0, engine->displayWidth, engine->displayHeight, 0);
//         al_flip_display();
//     }
//     al_close_video(splash);
// }
