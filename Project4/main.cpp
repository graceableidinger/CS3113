/**
* Author: Grace Ableidinger
* Assignment: Rise of the AI
* Date due: 2024-03-30 11:59 pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
* 
* 
* 
**/


/*-------TODO--------
V Enemy 1 - walking side to side
V Enemy 2 - flying up and down
V Enemy 3 - lying in wait or flying in a circle
V Player Kill
V Player Movement
V Player Jump
- Assets
- Animations - walking, idle, killing, enemy move 
V Floor
V Collisions with floor
V Animation Code
*/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define FIXED_TIMESTEP 0.0166666f



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
#include "map.h"

// ––––– SETTINGS ––––– //

// ___ Window Settings ___ //
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

// ___ Background Color Settings ___ //
float BG_RED = 0.18f,
BG_BLUE = 0.005f,
BG_GREEN = 0.005f,
BG_OPACITY = 1.0f;

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
const char* SPRITES[NUM_OF_ENEMIES + 1] = { "Girl.png", "Zombie.png", "Zombie.png", "Ghost.png"};
const char* MAP = { "Floor.png" };
const char* FONT = { "Font.png" };
const glm::vec3 STARTING_POS[NUM_OF_ENEMIES + 1] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.5f, 0.0f, 0.0f) , glm::vec3(10.0, 2.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f)};
const float SCALES[NUM_OF_ENEMIES + 1] = { 0.5f, 0.5f, 0.5f, 0.5f};

// ___ Platform Design ___ //
unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


// ––––– USER DEFINED ––––– //
// 
// ___ Enums ___ //
enum WinState { Win, Loss, In_Prog };

// ___ Structs ___ //
struct GameState
{
    Entity* player;
    Map* map;
    Entity* enemies;

    WinState win_state = In_Prog;
    bool game_is_running = true;
}g_state;



// ––––– HELPER FUNCTIONS ––––– //
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->SetModelMatrix(model_matrix);
    glUseProgram(program->GetProgramID());

    glVertexAttribPointer(program->GetPositionAttribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(g_program.GetPositionAttribute());
    glVertexAttribPointer(g_program.GetTexCoordAttribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(g_program.GetTexCoordAttribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(g_program.GetPositionAttribute());
    glDisableVertexAttribArray(g_program.GetTexCoordAttribute());
}

void update_win_state() {
    if (g_state.player->get_defeated() || g_state.player->get_position().y < -3.5) {
        g_state.win_state = Loss;
    }
    else {
        g_state.win_state = Win;
        for (int i = 0; i < NUM_OF_ENEMIES; i++) {
            if (!g_state.enemies[i].get_defeated()) {
                g_state.win_state = In_Prog;
            }
        }
    }
}

// ––––– BASE FUNCTIONS ––––– //
void initialize() {

    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Rise of the AI",
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

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);


    // --- PLAYER --- //
    g_state.player = new Entity();

    g_state.player->set_texid(load_texture(SPRITES[0]));
    g_state.player->set_width(SCALES[0]);
    g_state.player->set_height(SCALES[0]);
    g_state.player->set_model_matrix(glm::mat4(1.0f));
    g_state.player->set_type(Player);
    g_state.player->set_position(STARTING_POS[0]);
    g_state.player->set_has_anim(true);


    // --- ENEMIES --- //
    g_state.enemies = new Entity[NUM_OF_ENEMIES];

    for (int i = 0; i < NUM_OF_ENEMIES; i++) {
        g_state.enemies[i].set_texid(load_texture(SPRITES[1 + i]));

        g_state.enemies[i].set_width(SCALES[i + 1]);
        g_state.enemies[i].set_height(SCALES[i + 1]);

        g_state.enemies[i].set_model_matrix(glm::mat4(1.0f));

        g_state.enemies[i].set_position(STARTING_POS[i + 1]);

        g_state.enemies[i].set_type(Enemy);
    }
    // -- INDIVIDUAL ENEMY ATTRIBUTES -- //
    g_state.enemies[0].set_ai_type(Walk);
    g_state.enemies[0].set_has_anim(true);
    g_state.enemies[0].set_anim_dir(Left);
    g_state.enemies[1].set_ai_type(Wait);
    g_state.enemies[1].set_has_anim(true);
    g_state.enemies[2].set_ai_type(Fly);
    g_state.enemies[2].set_has_anim(false);


    // --- MAP --- //
    GLuint map_texture_id = load_texture(MAP);
    g_state.map = new Map(14.0f, 5.0f, LEVEL_1_DATA, map_texture_id, 1.0f, 2, 1);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE :
                g_state.game_is_running = false;
                break;
        }
    }
    g_state.player->process_input();
}

void update(){
    // --- DELTA TIME --- //
    float ticks = (float)SDL_GetTicks() / 1000;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    delta_time += g_time_accumulator;    

    // --- PLATFORM --- //
   // g_state.map.update(delta_time, g_state.player, g_state.enemies, g_state.map);


    // --- FIXED TIMESTAMP --- //
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        if (g_state.win_state == In_Prog) {
            g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map);
            for (int i = 0; i < NUM_OF_ENEMIES; i++) {
                if (!g_state.enemies[i].get_defeated()) {
                    g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map);
                }
            }
        }
        delta_time -= FIXED_TIMESTEP;
    }
    g_time_accumulator = delta_time;
    g_view_matrix = glm::mat4(1.0f);
    if (g_state.win_state == In_Prog) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_state.player->get_position().x, 0.0f, 0.0f));
    }
}

void render() {

    g_program.SetViewMatrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    update_win_state();

    if (g_state.win_state == Loss) {
        draw_text(&g_program, load_texture(FONT), "You Lose", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else if (g_state.win_state == Win) {
        draw_text(&g_program, load_texture(FONT), "You Win", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else {
        g_state.map->render(&g_program);
        g_state.player->render(&g_program, SCALES[0]);
        for (int i = 0; i < NUM_OF_ENEMIES; i++) {
            if (!g_state.enemies[i].get_defeated()) {
                g_state.enemies[i].render(&g_program, SCALES[i + 1]);
            }
        }
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    delete g_state.map;
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialize();

    while (g_state.game_is_running)
    {
        process_input();

        update();

        render();
    }

    shutdown();
    return 0;
}