/**
* Author: Grace Ableidinger
* Assignment: Platformer
* Date due: 2024-04-13 11:59 pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
* 
* 
* 
**/


/*-------TODO--------
V Map class
- Make map
- Scene Class
- Menu Screen
- Level 1
- Level 2
- Level 3
- Music
_ Sfx
- Lives

- character
- music
- sfx
- floor

- girl anim clothes
*/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define NUM_SCENES 4

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Scene.h"

#include "Start.h"
#include "Level1.h"

// ––––– SETTINGS ––––– //

// ___ Window Settings ___ //
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

// ___ Viewport Settings ___ //
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// ___ Font Settings ___ //
const int FONTBANK_SIZE = 16;

// ___ Texture Settings ___ //
const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

// ___ Game Settings ___ //
const int NUM_OF_LEVELS = 4;

// ___ Audio Settings ___ //
const int CD_QUAL_FREQ = 44100,  // compact disk (CD) quality frequency
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;
const char BGM_FILEPATH[] = "music.mp3";
const int    LOOP_FOREVER = -1; 
Mix_Music* g_music;
const int PLAY_ONCE = 0,
NEXT_CHNL = -1,  // next available channel
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;
Mix_Chunk* g_end_sfx;

// ––––– VARIABLES ––––– //

// ___ Window and Shaders ___ //
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

SDL_Window* g_display_window;
ShaderProgram g_program;


// ___ Delta Time ___ //
float g_time_accumulator = 0.0f,
g_previous_ticks = 0.0f;

// ___ Matrices ___ //
glm::mat4 g_view_matrix, // pos of camera
g_projection_matrix; // camera characteristics

// ––––– GAME VARIABLES ––––– //

// ___ Sprite Info ___ //

Scene* g_current_scene;
Start* g_start;
Level1* g_level1;
Level1* g_level2;
Level1* g_level3;
Scene* g_levels[NUM_OF_LEVELS];
bool g_game_is_running = true;
int g_lives = 3;


// ––––– HELPER FUNCTIONS ––––– //
void switch_scene(Scene* scene)
{
    if (scene != g_levels[0] && g_current_scene != g_levels[0]) {
        g_lives = g_current_scene->g_state.player->get_lives();
    }
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
    if (g_current_scene != g_levels[0]) {
        g_current_scene->g_state.player->set_lives(g_lives);
    }
}

// ––––– BASE FUNCTIONS ––––– //
void initialize() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    g_display_window = SDL_CreateWindow("Platformer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);

    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_program.SetViewMatrix(g_view_matrix);
    g_program.SetProjectionMatrix(g_projection_matrix);

    glUseProgram(g_program.GetProgramID());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    g_end_sfx = Mix_LoadWAV("end.wav");



    g_start = new Start();
    g_level1 = new Level1();
    g_level2 = new Level1();
    g_level3 = new Level1();
    
    g_level1->initialise();
    g_level2->initialise();
    g_level3->initialise();

    g_start->set_scene(0);
    g_level1->set_scene(1);
    g_level2->set_scene(2);
    g_level3->set_scene(3);


    g_level1->ENDING_POS = glm::vec3(13.0f, -0.8f, 0.0f);
    g_level2->ENDING_POS = glm::vec3(13.0f, -0.8f, 0.0f);
    g_level3->ENDING_POS = glm::vec3(13.0f, -0.8f, 0.0f);

    g_level2->set_sprites("Winter_Girl.png", "Winter_Tiles.png", "Spring_Flag.png");
    g_level3->set_sprites("Spring_Girl.png", "Spring_Tiles.png", "Summer_Flag.png");

    g_level1->set_enemy_attributes("Evil_Pumpkin.png", glm::vec3(4.5f, 0.0f, 0.0f), 0, Wait);
    g_level2->set_enemy_attributes("Snowflake.png", glm::vec3(3.0f, 2.0f, 0.0f), 0, Fly);
    g_level3->set_enemy_attributes("Butterfly.png", glm::vec3(5.0f, 0.0f, 0.0f), 0, Fly);

    g_levels[0] = g_start;
    g_levels[1] = g_level1;
    g_levels[2] = g_level2;
    g_levels[3] = g_level3;

    switch_scene(g_levels[0]);
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE :
                g_game_is_running = false;
                break;
        }
    }
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_RETURN]) {
        if (g_current_scene == g_start) {
            g_current_scene->g_state.next_scene_id = g_current_scene->g_state.scene_id+1;
        }
    }
    g_current_scene->process_input();
}

void update(){
    
    // --- DELTA TIME --- //
    float ticks = (float)SDL_GetTicks() / 1000;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    delta_time += g_time_accumulator;    

    // --- FIXED TIMESTAMP --- //
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }

    if (g_current_scene != g_start) {
        g_view_matrix = glm::mat4(1.0f);
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->g_state.player->get_position().x, 0.0f, 0.0f));
    }
    if (g_current_scene->g_state.win_state != In_Prog) {
        g_view_matrix = glm::mat4(1.0f);
    }
    g_time_accumulator = delta_time;
}

void render() {

    g_program.SetViewMatrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_program.GetProgramID());
    g_current_scene->render(&g_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    Mix_FreeMusic(g_music);
    SDL_Quit();
    delete g_level1;
    delete g_level2;
    delete g_level3;
}

int main(int argc, char* argv[])
{
    initialize();

    while (g_game_is_running)
    {
        process_input();

        update();

        if (g_current_scene->g_state.next_scene_id > 0) {
            Mix_PlayChannel(
                NEXT_CHNL,       // using the first channel that is not currently in use...
                g_end_sfx,  // ...play this chunk of audio...
                PLAY_ONCE        // ...once.
            );
            switch_scene(g_levels[g_current_scene->g_state.next_scene_id]);
        }
        else if (g_current_scene->g_state.next_scene_id == 0) {
            if (g_current_scene->g_state.win_state != Win) {
                Mix_PlayChannel(
                    NEXT_CHNL,       // using the first channel that is not currently in use...
                    g_end_sfx,  // ...play this chunk of audio...
                    PLAY_ONCE        // ...once.
                );
            }
            g_current_scene->g_state.win_state = Win;
        }
        render();
    }

    shutdown();
    return 0;
}