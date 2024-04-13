#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

enum WinState { Win, Loss, In_Prog };


struct GameState
{
    Entity* player;
    Entity* enemies;
    Entity* flag;

    WinState win_state = In_Prog;
    Map* map;
    Utility* util;

    int scene_id;
    int next_scene_id;
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //
    int g_number_of_enemies = 1;

    // ___ Background Color Settings ___ //
    float BG_RED = 0.18f,
        BG_BLUE = 0.005f,
        BG_GREEN = 0.005f,
        BG_OPACITY = 1.0f;
    const char* FONT = { "Font.png" };

    GameState g_state;

    // ————— METHODS ————— //
    void update_win_state();
    void process_input();
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    // ————— GETTERS ————— //
    GameState const get_state()             const { return g_state; }
    int       const get_number_of_enemies() const { return g_number_of_enemies; }

    void set_scene(int scene) { g_state.scene_id = scene; }
};
