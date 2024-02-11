/**
* Author: Grace Ableidinger
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

//Window Settings
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//Background Color Settings
float BG_RED = 0.00f,
BG_BLUE = 0.25f,
BG_GREEN = 0.00f,
BG_OPACITY = 1.0f;

//Viewport Settings
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//Non Delta Time Transformation Variables
const float FACTOR = 0.01f;
const int MAX_FRAME = 200;


//Delta Time Transformation Variables
const float DEGREES_PER_SEC = 0.1f;
float delta_time = 1.0f,
g_prev_ticks = 0.0f,
g_x_planet, g_y_planet, g_x_moons, g_y_moons, g_x_sun, g_y_sun, g_x_comet, g_y_comet = 0.0f,
g_circle_speed = 0.0005f,
g_rotation_planet, g_rotation_comet = 0.0f,
g_max_ticks = 10000.0f,
g_bg_amount = 0.00002f;
float g_factor = FACTOR;

int g_frame_counter = 0;


//Shader Paths
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",     // std::string
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//Declare Sprites
const char PLANET_SPRITE[] = "Planet.png",
SUN_SPRITE[] = "Sun.png",
MOONS_SPRITE[] = "Moons.png",
COMET_SPRITE[] = "Comet.png";

//Texture Variables
GLuint g_planet_texture_id,
g_sun_texture_id,
g_moons_texture_id,
g_comet_texture_id;
const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

//Window and Shader Initialization?
SDL_Window* g_display_window;
ShaderProgram g_program;

//Matrix Declaration
glm::mat4 g_view_matrix, // pos of camera
g_planet_model_matrix, // transformations
g_sun_model_matrix,
g_moons_model_matrix,
g_comet_model_matrix,
g_projection_matrix; // camera characteristics

//Misc. Global Bools
bool g_is_growing = true;
bool g_sun_up = true;
bool g_game_is_running = true;

//Function to control color change of BG - Non Delta Time
void bg_color_change() {
    if (g_sun_up) {
        if (BG_RED < 0.75f) {
            BG_RED += g_bg_amount;
            BG_BLUE += g_bg_amount;
        }
        if (BG_GREEN < 1.0f) {
            BG_GREEN += g_bg_amount;
        }
        else {
            g_sun_up = false;
        }
    }
    else {
        if (BG_RED > 0.0f) {
            BG_RED -= g_bg_amount;
            BG_BLUE -= g_bg_amount;
        }
        if (BG_GREEN > 0.0f) {
            BG_GREEN -= g_bg_amount;
        }
        else {
            g_sun_up = true;
        }
    }
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

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

void draw_object(glm::mat4& model_matrix, GLuint& tex_id)
{
    g_program.SetModelMatrix(model_matrix);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void initialize() {

    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Hello, World!",
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
    g_planet_texture_id = load_texture(PLANET_SPRITE);
    g_sun_texture_id = load_texture(SUN_SPRITE);
    g_moons_texture_id = load_texture(MOONS_SPRITE);
    g_comet_texture_id = load_texture(COMET_SPRITE);

    g_view_matrix = glm::mat4(1.0f);
    g_planet_model_matrix = glm::mat4(1.0f);
    g_sun_model_matrix = glm::mat4(1.0f);
    g_moons_model_matrix = glm::mat4(1.0f);
    g_comet_model_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_program.SetViewMatrix(g_view_matrix);
    g_program.SetProjectionMatrix(g_projection_matrix);
    
    glUseProgram(g_program.GetProgramID());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_game_is_running = false;
        }
    }
}

void update() {
    
    //General rotation variables
    float ticks = (float)SDL_GetTicks();
    delta_time = ticks - g_prev_ticks;
    g_prev_ticks = ticks;

    //float angle = g_circle_speed * ticks * delta_time;
    float angle = g_circle_speed * ticks;

    //Planet movement calculations
    float planet_orbit_radius = 3.0f;
    g_x_planet = planet_orbit_radius *cos(angle);
    g_y_planet = planet_orbit_radius *sin(angle);
    g_rotation_planet += (DEGREES_PER_SEC * delta_time);

    //Comet movement calculations
    float comet_offset = 4.0f;
    float comet_orbit_radius = 6.0f;
    g_x_comet = comet_orbit_radius * cos(angle);
    g_y_comet = comet_orbit_radius * sin(angle);
    g_rotation_comet = angle/28;

    //Planet movement
    g_planet_model_matrix = glm::mat4(1.0f);
    g_planet_model_matrix = glm::translate(g_planet_model_matrix, glm::vec3(g_x_planet, g_y_planet, 0.0f));
    g_planet_model_matrix = glm::rotate(g_planet_model_matrix, glm::radians(g_rotation_planet), glm::vec3(0.0f, 0.0f, 1.0f));

    //Comet movement
    g_comet_model_matrix = glm::mat4(1.0f);
    g_comet_model_matrix = glm::translate(g_comet_model_matrix, glm::vec3(g_x_comet+comet_offset, g_y_comet-comet_offset, 0.0f));
    g_comet_model_matrix = glm::rotate(g_comet_model_matrix, glm::degrees(g_rotation_comet-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    //Moon movement
    g_moons_model_matrix = glm::mat4(1.0f);
    g_moons_model_matrix = glm::translate(g_planet_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    g_moons_model_matrix = glm::rotate(g_moons_model_matrix, glm::radians(g_rotation_planet), glm::vec3(0.0f, 0.0f, 1.0f));


    if (g_x_sun >= 2.5f)
    {
        g_factor = -FACTOR;
    }
    else if (g_x_sun < 1.0f) {
        g_factor = FACTOR;
    }

    g_x_sun += g_factor/10 * delta_time;
    g_y_sun += g_factor/10 * delta_time;

    g_sun_model_matrix = glm::mat4(1.0f);
    g_sun_model_matrix = glm::scale(g_sun_model_matrix, glm::vec3(g_x_sun, g_y_sun, 1.0f));
    
    
    //BG Color Change
    bg_color_change();

}

void render() {

    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
    };

    glVertexAttribPointer(g_program.GetPositionAttribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.GetPositionAttribute());
    
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };
    
    glVertexAttribPointer(g_program.GetTexCoordAttribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_program.GetTexCoordAttribute());

    draw_object(g_planet_model_matrix, g_planet_texture_id);
    draw_object(g_sun_model_matrix, g_sun_texture_id);
    draw_object(g_moons_model_matrix, g_moons_texture_id);
    draw_object(g_comet_model_matrix, g_comet_texture_id);

    glDisableVertexAttribArray(g_program.GetPositionAttribute());
    glDisableVertexAttribArray(g_program.GetTexCoordAttribute());


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