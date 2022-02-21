//  libbl.h
//  (heders)
//
//  Created by David Kviloria on 10/14/20.
//

// License: Public domain 

// This is free and unencumbered software released into the public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#ifndef _lib_bl_h_
#define _lib_bl_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#ifdef LIBBL_USE_APPLE_OPENGL && __APPLE__
#include <OpenGL/gl.h>
#else
#include "../ext/glad/glad.h"
#endif

#if __APPLE__
#include <strings.h>
#else
#include <string.h>
#endif

#ifdef LIBBL_STB_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb/stb_image.h"
#endif

#include "../ext/math/linmath.h"

#define PI 3.14159265
#define DEG PI / 180.0f

typedef unsigned char         u8_t;
typedef unsigned short        u16_t;
typedef unsigned int          u32_t;
typedef unsigned long long    u64_t;
typedef short                 i16_t;
typedef int                   i32_t;
typedef long long             i64_t;
typedef signed char           s8_t;
typedef signed short          s16_t;
typedef signed int            s32_t;
typedef signed long long      s64_t;
typedef float                 f32_t;
typedef double                f64_t;
typedef char                  c_t;
typedef char                  b_t;
typedef const char            cc_t;
typedef size_t                sz_t;
typedef intptr_t              iptr_t;
typedef uintptr_t             uptr_t;

typedef struct {
vec3                          position;
vec3                          color;
vec2                          uv;
} bl_vertex_t;

typedef struct {
u32_t                         vertices[3];
} bl_vertex_index_t;

typedef struct
{
u32_t                         draw_call_count;
u32_t                         vertex_count;
u32_t                         index_count;
u32_t                         texture_count;
u32_t                         shader_count;
u32_t                         entity_count;
} bl_stats_t;

static bl_stats_t             bl_global_stats = {0};

// @transform
typedef struct
{
  vec2                        position;
  vec2                        rotation;
  vec2                        scale; 
} bl_transfor_t;

// @entity
typedef struct
{
  cc_t                       *name;
  u16_t                       index;
  vec3                        position;
  vec3                        scale;
  f32_t                       rotation; 
} bl_ent_t;

#define GLOBAL_M_SIZE         512
bl_ent_t* global_manager      [GLOBAL_M_SIZE];

bl_ent_t* bl_get_entity(cc_t* name, bl_ent_t* _manager[])
{
  for (u32_t i = 0; i <= GLOBAL_M_SIZE; i++) {
    if (_manager[i] == NULL)  { continue; }
    if (strcmp(name, _manager[i]->name) == 0) {
      return                  _manager[i];
    }
  }
  return                      NULL;
}

void bl_register_entity(bl_ent_t* ent, bl_ent_t* _manager[GLOBAL_M_SIZE])
{
  _manager[*&ent->index] =    ent;
}

//
// @Utility functions
//
cc_t* read_file_content(cc_t* path)
{
  FILE                       *infile;
  c_t                        *buffer;
  s64_t                       numbytes;

  infile = fopen              (path, "r");
  if (infile == NULL) {
    perror                    ("fopen\t");
    exit                      (EXIT_FAILURE);
  }

  fseek                       (infile, 0L, SEEK_END);
  numbytes = ftell            (infile);
  fseek                       (infile, 0L, SEEK_SET);
  buffer =                    (b_t*) calloc(numbytes, sizeof(b_t));
  if (buffer == NULL)         { exit  (EXIT_FAILURE); }

  fread                       (buffer, sizeof(b_t), numbytes, infile);
  fclose                      (infile);
  return                      buffer;
} 

/**
 * @GL_TEXTURE
 * 
 * @param {cc_t*} path
 * @param {u32_t*} texture_id
 * 
 * @brief Load file from disk, Store in the memory and generate GL_TEXTURE buffer
 */
void load_and_create_texture(cc_t* path, u32_t* texture_id, u32_t __SLOT)
{

  #if                         defined(STB_IMAGE_IMPLEMENTATION)
  
  if (__SLOT == NULL)         { __SLOT = 0; }

  stbi_set_flip_vertically_on_load(1);

  s32_t                        width;
  s32_t                        height;
  s32_t                        channels;
  u8_t                         *buffer;

  buffer = stbi_load          (path, &width, &height, &channels, 0);

  glGenTextures               (1, texture_id);
  glActiveTexture             (GL_TEXTURE0 + __SLOT);
  glBindTexture               (GL_TEXTURE_2D, texture_id);

  u32_t                       texDataFormat;
  if (channels == 4)          { texDataFormat = GL_RGBA; }
  if (channels == 3)          { texDataFormat = GL_RGB; }

  glTexParameteri             (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri             (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri             (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri             (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D                (GL_TEXTURE_2D, 0, texDataFormat, width, height, 0, texDataFormat, GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap            (GL_TEXTURE_2D);
  stbi_image_free             (buffer);
  
  #else
  #error                      "STB_IMAGE_IMPLEMENTATION not defined"
  #endif  
}

//
// @VertexArray
//
typedef struct
{
u32_t                         m_renderer_id;
} vertex_array_t;

void gen_and_bind_vertex_array(vertex_array_t* vao)
{
#if defined(LIBBL_USE_APPLE_OPENGL) && defined(__APPLE__)
  glGenVertexArraysAPPLE      (1, &vao->m_renderer_id);
  glBindVertexArrayAPPLE      (vao->m_renderer_id);
#else
  glGenVertexArrays           (1, &vao->m_renderer_id);
  glBindVertexArray           (vao->m_renderer_id);
#endif
}

void unbind_vertex_array(vertex_array_t* vao) 
{
#if defined(LIBBL_USE_APPLE_OPENGL) && defined(__APPLE__)
  glBindVertexArrayAPPLE      (0);
#else
  glBindVertexArray           (0);
#endif
}

void free_vertex_array(vertex_array_t* vao)
{
#if defined(LIBBL_USE_APPLE_OPENGL) && defined(__APPLE__)
  glDeleteVertexArraysAPPLE   (1, &vao->m_renderer_id);
#else
  glDeleteVertexArrays        (1, &vao->m_renderer_id);
#endif
}

//
// @VertexBuffer
//
typedef struct
{
u32_t                         m_renderer_id;
} vertex_buffer_t;

void gen_vertex_buffer(vertex_buffer_t* vbo)
{
  glGenBuffers                (1, &vbo->m_renderer_id);
}

void bind_vertex_buffer(vertex_buffer_t* vbo)
{
  glBindBuffer                (GL_ARRAY_BUFFER, vbo->m_renderer_id);
}

void vertex_buffer_data(iptr_t size, f32_t* data)
{
  glBufferData                (GL_ARRAY_BUFFER, size, *&data, GL_STATIC_DRAW);
}

void unbind_vertex_buffer()
{
  glBindBuffer                (GL_ARRAY_BUFFER, 0);
}

void free_vertex_buffer(vertex_buffer_t* vbo)
{
  glDeleteBuffers             (1, &vbo->m_renderer_id);
}

//
// @IndiceBuffer
//
typedef struct
{
u32_t                         m_renderer_id;
} indice_buffer_t;

void gen_indice_buffer(indice_buffer_t* ebo)
{
  glGenBuffers                (1, &ebo->m_renderer_id);
}

void bind_indice_buffer(indice_buffer_t* ebo)
{
  glBindBuffer                (GL_ELEMENT_ARRAY_BUFFER, ebo->m_renderer_id);
}

void indice_buffer_data(iptr_t size, u32_t* data)
{
  glBufferData                (GL_ELEMENT_ARRAY_BUFFER, size, *&data, GL_STATIC_DRAW);
}

void free_indice_buffer(vertex_buffer_t* ebo)
{
  glDeleteBuffers             (1, &ebo->m_renderer_id);
}

//
// @ShaderProgram
//

typedef struct
{
i32_t                  m_program_id;
} shader_program_t;

u32_t compile_shader(cc_t* content, u32_t type)
{
  u32_t shader =              glCreateShader(type);
  if( 0 == shader )          
  {
    fprintf                   (stderr, "Error creating vertex shader.\n");
    exit                      (EXIT_FAILURE);
  }
  glShaderSource              (shader, 1, &content, NULL);
  glCompileShader             (shader);
  return                      shader;
}

void create_program(shader_program_t* p, u32_t fsh, u32_t vsh)
{
  p->m_program_id =           glCreateProgram();
  glAttachShader              (p->m_program_id, fsh);
  glAttachShader              (p->m_program_id, vsh);
  glLinkProgram               (p->m_program_id);
}

void attach_program(shader_program_t* p)
{
  glUseProgram                (p->m_program_id);
}

void detach_program(shader_program_t* p)
{
  glDeleteProgram             (p->m_program_id);
}

void draw_triangles(s32_t count)
{
  glDrawElements              (GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
  bl_global_stats.draw_call_count++;
}

#ifdef                        __cplusplus
};
#endif

#endif                        /* _lib_bl_h_ */
