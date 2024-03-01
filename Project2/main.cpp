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
float BG_RED = 0.0f,
BG_BLUE = 0.25f,
BG_GREEN = 0.50f,
BG_OPACITY = 1.0f;

//Viewport Settings
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;


//Delta Time Transformation Variables
float delta_time = 1.0f,
g_prev_ticks = 0.0f,
OG_SPEED = 0.002f;
float g_left_speed = OG_SPEED, 
g_right_speed = OG_SPEED;
int g_frame_counter = 0;


//Movement Vectors
glm::vec3 g_left_movement = glm::vec3(-3.5f, 0.0f, 0.0f),
g_right_movement = glm::vec3(3.5f, 0.0f, 0.0f);

//Shader Paths
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",     // std::string
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//Declare Sprites
const char LPADDLE_SPRITE[] = "BlueMan.png";
const char RPADDLE_SPRITE[] = "RedMan.png";
const char BGAMEOVER_SPRITE[] = "BlueWin.png";
const char RGAMEOVER_SPRITE[] = "RedWin.png";
const char BALL_SPRITE[] = "soccer.png";
const char LPOLE_SPRITE[] = "Lpole.png";
const char RPOLE_SPRITE[] = "Rpole.png";


//Texture Variables
GLuint g_lpaddle_texture_id, g_rpaddle_texture_id, g_blue_win_texture_id, g_red_win_texture_id, g_lpole_texture_id, g_rpole_texture_id;
const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

//Window and Shader Initialization?
SDL_Window* g_display_window;
ShaderProgram g_program;

//Matrix Declaration
glm::mat4 g_view_matrix, // pos of camera
g_left_paddle_model_matrix, // transformations
g_right_paddle_model_matrix,
g_lpole_model_matrix,
g_rpole_model_matrix,
g_b_win_model_matrix,
g_r_win_model_matrix,
g_projection_matrix; // camera characteristics

//Misc. Global Bools
bool g_game_is_running = true;
bool g_auto_move = false;
int g_winner = 0; //1 = blue, 2 = red
int g_ball_amount = 1;

struct ball_data{
    glm::vec3 g_ball_movement = glm::vec3(1.0f, 1.0f, 0.0f);
    float g_ball_x_speed = OG_SPEED,
        g_ball_y_speed = OG_SPEED;
    glm::mat4 g_ball_model_matrix;
    GLuint g_ball_texture_id;

}ball1, ball2, ball3;

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
    g_lpaddle_texture_id = load_texture(LPADDLE_SPRITE);
    g_rpaddle_texture_id = load_texture(RPADDLE_SPRITE);
    g_red_win_texture_id = load_texture(RGAMEOVER_SPRITE);
    g_blue_win_texture_id = load_texture(BGAMEOVER_SPRITE);
    ball1.g_ball_texture_id = load_texture(BALL_SPRITE);
    ball2.g_ball_texture_id = load_texture(BALL_SPRITE);
    ball3.g_ball_texture_id = load_texture(BALL_SPRITE);
    g_lpole_texture_id = load_texture(LPOLE_SPRITE);
    g_rpole_texture_id = load_texture(RPOLE_SPRITE);

    ball1.g_ball_movement = glm::vec3(0.0f, -1.0f, 0.0f);
    ball2.g_ball_movement = glm::vec3(0.0f, 1.5f, 0.0f);
    ball3.g_ball_movement = glm::vec3(0.0f, -1.5f, 0.0f);

    g_view_matrix = glm::mat4(1.0f);
    g_left_paddle_model_matrix = glm::mat4(1.0f);
    g_right_paddle_model_matrix = glm::mat4(1.0f);
    g_r_win_model_matrix = glm::mat4(1.0f);
    g_b_win_model_matrix = glm::mat4(1.0f);
    ball1.g_ball_model_matrix = glm::mat4(1.0f);
    ball2.g_ball_model_matrix = glm::mat4(1.0f);
    ball3.g_ball_model_matrix = glm::mat4(1.0f);
    g_lpole_model_matrix = glm::mat4(1.0f);
    g_rpole_model_matrix = glm::mat4(1.0f);
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
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                //if auto move button is pressed
            case SDLK_t:
                g_auto_move = !g_auto_move;
                g_left_speed = OG_SPEED;
            }
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    g_ball_amount = 1;
            }
            switch (event.key.keysym.sym) {
                case SDLK_2:
                    g_ball_amount = 2;
            }
            switch (event.key.keysym.sym) {
                case SDLK_3:
                    g_ball_amount = 3;
            }
        }
    }
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    //Player controlled movements
    if (!g_auto_move) {

        if (key_state[SDL_SCANCODE_W] && (g_left_movement.y <= 4)) {
            g_left_speed = OG_SPEED;
        }
        else if (key_state[SDL_SCANCODE_S] && (g_left_movement.y >= -4)) {
            g_left_speed = -OG_SPEED;
        }
        else {
            g_left_speed = 0;
        }
    }
    if (key_state[SDL_SCANCODE_UP] && (g_right_movement.y <= 4)) {
        g_right_speed = OG_SPEED;
    }
    else if (key_state[SDL_SCANCODE_DOWN] && (g_right_movement.y >= -4)) {
        g_right_speed = -OG_SPEED;
    }
    else {
        g_right_speed = 0;
    }
}

void ball_move(ball_data& ball) {
    float left_x_distance, left_y_distance, right_x_distance, right_y_distance;
    ball.g_ball_movement.x += ball.g_ball_x_speed / 2.0f * delta_time;
    ball.g_ball_movement.y += ball.g_ball_y_speed / 2.0f * delta_time;
    if (ball.g_ball_movement.y >= 4) {
        ball.g_ball_y_speed = -OG_SPEED;
    }
    else if (ball.g_ball_movement.y <= -4) {
        ball.g_ball_y_speed = OG_SPEED;
    }

    if (ball.g_ball_movement.x >= 5) {
        g_winner = 1;
    }
    else if(ball.g_ball_movement.x <= -5) {
        g_winner = 2;
    }

    left_x_distance = fabs(ball.g_ball_movement.x - g_left_movement.x) - ((1.0f + 1.0f) / 2.0f);
    left_y_distance = fabs(ball.g_ball_movement.y - g_left_movement.y) - ((1.0f + 0.5f) / 2.0f);
    right_x_distance = fabs(ball.g_ball_movement.x - g_right_movement.x) - ((1.0f + 1.0f) / 2.0f);
    right_y_distance = fabs(ball.g_ball_movement.y - g_right_movement.y) - ((1.0f + 0.5f) / 2.0f);


    if (left_x_distance < 0.0f && left_y_distance < 0.0f)
    {
        ball.g_ball_x_speed = OG_SPEED;
    }
    if (right_x_distance < 0.0f && right_y_distance < 0.0f)
    {
        ball.g_ball_x_speed = -OG_SPEED;

    }
}

void ball_translate(ball_data& ball) {
    ball.g_ball_model_matrix = glm::mat4(1.0f);
    ball.g_ball_model_matrix = glm::translate(ball.g_ball_model_matrix, ball.g_ball_movement);
}

void update() {
   
    //General rotation variables
    float ticks = (float)SDL_GetTicks();
    delta_time = ticks - g_prev_ticks;
    g_prev_ticks = ticks;

    
    ball_move(ball1);
    if (g_ball_amount >1) {
        ball_move(ball2);
    }
    if (g_ball_amount > 2) {
        ball_move(ball3);
    }
    

    //Paddle movement calculations
    if (g_auto_move) {
        //If auto movement is on
        if (g_left_movement.y >= 4 || g_left_movement.y <= -4) {
            g_left_speed *= -1;
        }
    }
    g_left_movement.y += g_left_speed * delta_time;
    g_right_movement.y += g_right_speed * delta_time;

    if (!g_winner) {
        //Ball Paddle Movement
        ball_translate(ball1);
        if (g_ball_amount > 1) {
            ball_translate(ball2);
        }
        if (g_ball_amount > 2) {
            ball_translate(ball3);
        }

        //Left Paddle Movement
        g_left_paddle_model_matrix = glm::mat4(1.0f);
        g_left_paddle_model_matrix = glm::translate(g_left_paddle_model_matrix, g_left_movement);

        //Right Paddle movement
        g_right_paddle_model_matrix = glm::mat4(1.0f);
        g_right_paddle_model_matrix = glm::translate(g_right_paddle_model_matrix, g_right_movement);
    }
    g_lpole_model_matrix = glm::mat4(1.0f);
    g_rpole_model_matrix = glm::mat4(1.0f);
    g_lpole_model_matrix = glm::translate(g_lpole_model_matrix, glm::vec3(-3.5f, 0.0f, 0.0f));
    g_lpole_model_matrix = glm::scale(g_lpole_model_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    g_rpole_model_matrix = glm::scale(g_rpole_model_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    g_rpole_model_matrix = glm::translate(g_rpole_model_matrix, glm::vec3(3.5f, 0.0f, 0.0f));
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

    draw_object(g_lpole_model_matrix, g_lpole_texture_id);
    draw_object(g_rpole_model_matrix, g_rpole_texture_id);
    draw_object(g_left_paddle_model_matrix, g_lpaddle_texture_id);
    draw_object(g_right_paddle_model_matrix, g_rpaddle_texture_id);
    draw_object(ball1.g_ball_model_matrix, ball1.g_ball_texture_id);
    if (g_ball_amount > 1) {
        draw_object(ball2.g_ball_model_matrix, ball2.g_ball_texture_id);
    }
    if (g_ball_amount > 2) {
        draw_object(ball3.g_ball_model_matrix, ball3.g_ball_texture_id);
    }
    


    if (g_winner == 1) {
        draw_object(g_b_win_model_matrix, g_blue_win_texture_id);
    }
    else if (g_winner == 2) {
        draw_object(g_r_win_model_matrix, g_red_win_texture_id);
    }

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