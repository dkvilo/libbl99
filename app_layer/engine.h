#ifndef _engine_h_
#define _engine_h_

#include "libbl.h"

#define VERTEX_SIZE 4
#define INDICES_SIZE 6
#define ENTITIES_SIZE 512

#define internal static
#define inlined __attribute__((always_inline)) inline
#define external extern

// @Entity
typedef bl_ent_t ent_t;

// @Mesh
typedef struct
{
  bl_vertex_t vertex[VERTEX_SIZE];
  bl_vertex_index_t indices[INDICES_SIZE];
} mesh_t;

void
make_mesh(mesh_t* mesh);

// @Game
typedef struct
{
  mesh_t mesh[ENTITIES_SIZE];
  ent_t entities[ENTITIES_SIZE];
} game_t;

// IMPLEMENTATION //

void
make_mesh(mesh_t* mesh)
{
  const bl_vertex_t _vertices[VERTEX_SIZE] = {
    // @Position             //@Color              //@UV
    { { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, 0.0 }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
  };

  //@Indexies
  const bl_vertex_index_t _indices[INDICES_SIZE] = {
    { 0, 1, 2 },
    { 0, 2, 3 },
  };

  memcpy(mesh->vertex, _vertices, sizeof(_vertices));
  memcpy(mesh->indices, _indices, sizeof(_indices));
}

#endif