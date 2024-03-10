/**
* Author: Grace Ableidinger
* Assignment: Lunar Lander
* Date due: 2024-03-09 11:59 pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
* 
* 
* 
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define ACC_OF_GRAVITY -0.0981f
#define NUM_OF_GROUND_OBJ 8
#define NUM_OF_OTHER_OBJ 4

#define STARTING_FUEL 500

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "Entity.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

//Window Settings
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//Background Color Settings
float BG_RED = 0.09f,
BG_BLUE = 0.50f,
BG_GREEN = 0.42f,
BG_OPACITY = 1.0f;

//Viewport Settings
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;


//Delta Time Transformation Variables
#define FIXED_TIMESTEP 0.0166666f
float g_time_accumulator = 0.0f,
g_previous_ticks = 0.0f,
delta_time = 1.0f,
g_prev_ticks = 0.0f;
int g_frame_counter = 0;

//Shader Paths
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",     // std::string
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//Declare Sprites 
int safe_landings[] = { 1,2,3,6 };
const char* SPRITES[NUM_OF_GROUND_OBJ+NUM_OF_OTHER_OBJ] = { "Sub.png", "Sand.png", "Kelp.png", "Plants.png", "Coral.png", "Oyster.png",  "Seaweed.png", "Tube.png", "Crab.png", "Failed.png", "Success.png", "No_Fuel.png"};
const glm::vec3 STARTING_POS[NUM_OF_GROUND_OBJ+1] = { glm::vec3(-4.0f, 3.0f, 0.0f), glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(-4.0f, -1.2f, 0.0f), glm::vec3(-1.2f, -1.5f, 0.0f) ,  glm::vec3(-2.5f, -2.4f, 0.0f), glm::vec3(4.5f, -1.5f, 0.0f), glm::vec3(2.7f, -1.2f, 0.0f), glm::vec3(1.0f, -2.0f, 0.0f), glm::vec3(0.5f, -2.4f, 0.0f) };
const float SCALES[NUM_OF_GROUND_OBJ+1] = {0.5, 5.0, 1.5, 1.5, 0.8, 0.7, 1.7, 1.0, 0.7};

//Texture Variables
const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

//Window and Shader Initialization?
SDL_Window* g_display_window;
ShaderProgram g_program;

//Matrix Declaration
glm::mat4 g_view_matrix, // pos of camera
g_projection_matrix; // camera characteristics

//Misc. Global Bools
bool g_game_is_running = true;

struct GameState
{
    Entity* player;
    Entity* ground[NUM_OF_GROUND_OBJ];
    Entity* UI[3];
    bool won = false, lost = false;
    float fuel = STARTING_FUEL;
}g_state;

GLuint load_texture(const char* filepath) {
    int width, height, num_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &num_of_components, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image." << std::endl;
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}



void initialize() {

    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);

    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    g_state.player = new Entity();
    for (int i = 0; i < NUM_OF_GROUND_OBJ; i++) {
        g_state.ground[i] = new Entity();
        g_state.ground[i]->set_texid(load_texture(SPRITES[i + 1]));
        g_state.ground[i]->set_model_matrix(glm::mat4(1.0f));
        g_state.ground[i]->set_position(STARTING_POS[i + 1]);
        g_state.ground[i]->set_model_matrix(glm::translate(g_state.ground[i]->get_model_matrix(), g_state.ground[i]->get_position()));
        for(int j = 0; j < (sizeof(safe_landings) / sizeof(safe_landings[0])); j++){
            if (safe_landings[j] == i+1) {
                g_state.ground[i]->set_safety(true);
            }
        }
        if (SPRITES[i + 1] == "Sand.png") {
            g_state.ground[i]->set_as_ground(true);
        }
    }
    g_state.ground[0]->set_as_ground(true);

    for (int i = 0; i < 3; i++) {
        g_state.UI[i] = new Entity();
        g_state.UI[i]->set_texid(load_texture(SPRITES[i +1+NUM_OF_GROUND_OBJ]));
        g_state.UI[i]->set_model_matrix(glm::mat4(1.0f));
    }
    g_state.UI[2]->set_position(glm::vec3(4.0, 3.0, 0.0));
    g_state.UI[2]->set_model_matrix(glm::translate(g_state.UI[2]->get_model_matrix(), g_state.UI[2]->get_position()));


    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    g_state.player->set_texid(load_texture(SPRITES[0]));

    g_view_matrix = glm::mat4(1.0f);
    g_state.player->set_model_matrix(glm::mat4(1.0f));
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_state.player->set_position(STARTING_POS[0]);
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f));

    g_program.SetViewMatrix(g_view_matrix);
    g_program.SetProjectionMatrix(g_projection_matrix);
    
    glUseProgram(g_program.GetProgramID());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    g_state.fuel = g_state.player->process_input(ACC_OF_GRAVITY, g_state.fuel);
}

void detect_collision() {
    float x_diff, y_diff, x_dist, y_dist;
    for (int i = 0; i < NUM_OF_GROUND_OBJ; i++) {
        if (!(g_state.ground[i]->is_safe_to_land()) && !g_state.ground[i]->is_ground()) {
            x_diff = fabs(g_state.player->get_position().x - g_state.ground[i]->get_position().x);
            y_diff = fabs(g_state.player->get_position().y - g_state.ground[i]->get_position().y);

            x_dist = x_diff - (g_state.player->get_width() + g_state.ground[i]->get_width()) ;
            y_dist = y_diff - (g_state.player->get_height() + g_state.ground[i]->get_height()) ;

            if (x_dist < 0 && y_dist < 0) {
                g_state.lost = true;
            }
        }
        else if (g_state.ground[i]->is_ground()) {
            if (g_state.player->get_position().y<-3) {
                g_state.won = true;
            }
        }
    }
}

void update(){
    if (!g_state.lost && !g_state.won) {
        // delta time
        float ticks = (float)SDL_GetTicks() / 1000;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;

        //fixed timestamp
        delta_time += g_time_accumulator;

        if (delta_time < FIXED_TIMESTEP)
        {
            g_time_accumulator = delta_time;
            return;
        }

        while (delta_time >= FIXED_TIMESTEP)
        {
            g_state.player->update(FIXED_TIMESTEP);
            delta_time -= FIXED_TIMESTEP;
        }

        g_time_accumulator = delta_time;
        detect_collision();
    }
}

void render() {

    glClear(GL_COLOR_BUFFER_BIT);

    if (!g_state.lost && !g_state.won) {
        if (g_state.fuel <= 0) {
            g_state.UI[2]->render(&g_program, 1.0);
        }
        for (int i = 0; i < NUM_OF_GROUND_OBJ; i++) {
            g_state.ground[i]->render(&g_program, SCALES[i+1]);
        }
        g_state.player->render(&g_program, SCALES[0]);
    }
    else if (g_state.lost) {
        g_state.UI[0]->render(&g_program, 3.0);
    }
    else if (g_state.won) {
        g_state.UI[1]->render(&g_program, 3.0);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialize();

    while (g_game_is_running)
    {
        process_input();

        update();

        render();
    }

    shutdown();
    return 0;
}