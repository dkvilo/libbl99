//
//  main.c
//  2D Lights - sandbox
//
//  Created by David Kviloria on 10/11/20.
//

#define LIBBL_STB_IMPLEMENTATION
#include "bl/libbl.h"
#include "ext/GLFW/glfw3.h"

#include <math.h> // cos, sin
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // time, time_t

void
key_callback(GLFWwindow* window,
             u32_t key,
             u32_t scancode,
             u32_t action,
             u32_t mods);
void

reload_program(shader_program_t* p,
               cc_t* vertex_shader_path,
               cc_t* fragment_shader_path,
               cc_t* vertex_shader_out,
               cc_t* fragment_shader_out);

static const bl_vertex_t global_vertices[4] = {
  // @Position             //@Color              //@UV
  { { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
  { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
  { { -0.5f, -0.5f, 0.0 }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
};

//@Indexies
static const bl_vertex_index_t global_indices[6] = {
  { 0, 1, 2 },
  { 0, 2, 3 },
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ARRAY_SIZE_2D(a) (sizeof(a) / sizeof(a[0][0]))

static const i32_t global_world_matrix[5][5] = { { 0, 0, 0, 0, 0 },
                                                 { 0, 1, 1, 1, 0 },
                                                 { 0, 1, 1, 1, 0 },
                                                 { 0, 1, 1, 1, 0 },
                                                 { 0, 0, 0, 0, 0 } };

int
main(int argc, const char* argv[])
{

  GLFWwindow* window;
  int mvp_location, uTime_location, uCircle_location;

  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window =
    glfwCreateWindow(1080, 720, "2D Lights Example with SD Shapes", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(-1);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  bl_ent_t player = { .name = "Player",
                      .index = 0,
                      .position = { 1.2f, 1.2f } };

  bl_register_entity(&player, global_manager);

  vertex_buffer_t vbo;
  gen_vertex_buffer(&vbo);
  bind_vertex_buffer(&vbo);
  vertex_buffer_data(sizeof(global_vertices), global_vertices);

  indice_buffer_t ebo;
  gen_indice_buffer(&ebo);
  bind_indice_buffer(&ebo);

  vertex_array_t vao;
  gen_and_bind_vertex_array(&vao);

  bind_vertex_buffer(&vbo);
  vertex_buffer_data(sizeof(global_vertices), global_vertices);

  bind_indice_buffer(&ebo);
  indice_buffer_data(sizeof(global_indices), global_indices);

  cc_t* vert_src = read_file_content("shaders/base_vertex_shader.vert");
  cc_t* frag_src = read_file_content("shaders/light_fragment_shader.frag");

  shader_program_t program;
  create_program(&program,
                 compile_shader(vert_src, GL_VERTEX_SHADER),
                 compile_shader(frag_src, GL_FRAGMENT_SHADER));

  free(vert_src);
  free(frag_src);

  u32_t texture_id;
  load_and_create_texture("./assets/sample.jpg", &texture_id, 0);

  u32_t texture_id2;
  load_and_create_texture("./assets/normal.jpg", &texture_id2, 1);

  mvp_location = glGetUniformLocation(program.m_program_id, "MVP");
  uTime_location = glGetUniformLocation(program.m_program_id, "uTime");
  uCircle_location = glGetUniformLocation(program.m_program_id, "uCircle");
  u32_t tex0Uni = glGetUniformLocation(program.m_program_id, "tex0");
  u32_t tex1Uni = glGetUniformLocation(program.m_program_id, "tex1");

  // position attribute
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(global_vertices [0]),
                        (void*)offsetof(bl_vertex_t, position));
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(bl_vertex_t),
                        (void*)offsetof(bl_vertex_t, color));
  glEnableVertexAttribArray(1);

  // texture coord attribute
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(bl_vertex_t),
                        (void*)offsetof(bl_vertex_t, uv));
  glEnableVertexAttribArray(2);

  f32_t ratio;
  u32_t width, height;

  mat4x4 model, projection, mvp;
  f64_t deltaTime, previousTime, elapsedTime;

  f32_t p_speed = 0.01f;
  bl_ent_t* ent_player = bl_get_entity("Player", global_manager);
  if (ent_player == NULL) {
    printf("Entity Player not found");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  double avgFps = 0;
  unsigned long long frame = 0;

  u16_t press_time = 0;

  while (!glfwWindowShouldClose(window)) {

    deltaTime = glfwGetTime();
    previousTime = deltaTime;
    elapsedTime = deltaTime - previousTime;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (f32_t)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    mat4x4_identity(model);
    mat4x4_ortho(projection, -ratio, ratio, 1.f, -1.f, 1.0f, -1000.f);

    for (int x = 0; x < ARRAY_SIZE(global_world_matrix); x++) {
      for (int y = 0; y < ARRAY_SIZE(global_world_matrix[x]); y++) {

        mat4x4_translate(model, 1.2f - x, 1.2f - y, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_H) != GLFW_PRESS) {
          mat4x4_scale_aniso(model, model, 0.98, 0.98, 0);
        }

        mat4x4_mul(mvp, projection, model);

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
        glUniform1f(uTime_location, deltaTime);

        if (global_world_matrix[x][y] == 1) {
          glUniform1f(uCircle_location, 1.0f);
        } else {
          glUniform1f(uCircle_location, 2.0f);
        }

        attach_program(&program);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
          if (++press_time == 1) {
            reload_program(&program,
                           "shaders/base_vertex_shader.vert",
                           "shaders/light_fragment_shader.frag",
                           vert_src,
                           frag_src);
          }
        } else {
          press_time = 0;
        }

        glUniform1i(tex0Uni, 0);
        glUniform1i(tex1Uni, 1);

        draw_triangles(sizeof(global_vertices));
        glDeleteTextures(1, &texture_id);
        detach_program(&program);
      }
    }

    mat4x4_translate(model,
                     ent_player->position[0],
                     ent_player->position[1],
                     ent_player->position[2]);

    mat4x4_scale_aniso(
      model, model, ent_player->position[2], ent_player->position[2], 0);
    mat4x4_rotate_Z(model, model, deltaTime);
    mat4x4_rotate_Y(model, model, deltaTime);

    mat4x4_mul(mvp, projection, model);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);

    glUniform1f(uCircle_location, 3.0f);
    attach_program(&program);
    draw_triangles(sizeof(global_vertices));
    detach_program(&program);

    // Controlls
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      ent_player->position[0] = ent_player->position[0] -= p_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      ent_player->position[0] = ent_player->position[0] += p_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      ent_player->position[1] = ent_player->position[1] += p_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      ent_player->position[1] = ent_player->position[1] -= p_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      ent_player->position[2] = ent_player->position[2] += p_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
      ent_player->position[2] = ent_player->position[2] -= p_speed;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return EXIT_SUCCESS;
}

void
key_callback(GLFWwindow* window,
             u32_t key,
             u32_t scancode,
             u32_t action,
             u32_t mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void
reload_program(shader_program_t* p,
               cc_t* vertex_shader_path,
               cc_t* fragment_shader_path,
               cc_t* vertex_shader_out,
               cc_t* fragment_shader_out)
{
  time_t begin = time(NULL);
  printf("[SHADER] Reloading Shader ... ");

  vertex_shader_out = read_file_content(vertex_shader_path);
  fragment_shader_out = read_file_content(fragment_shader_path);

  detach_program(&p);
  create_program(p,
                 compile_shader(vertex_shader_out, GL_VERTEX_SHADER),
                 compile_shader(fragment_shader_out, GL_FRAGMENT_SHADER));

  free(vertex_shader_out);
  free(fragment_shader_out);
  attach_program(p);
  time_t end = time(NULL);

  printf("%f ms Done.\n", difftime(end, begin) * 1000);
}
