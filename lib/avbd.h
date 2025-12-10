#pragma once

#include <arena.h>
#include <bp.h>
#include <buff.h>

// 12 bytes
typedef struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

// 16 bytes
typedef struct Vec4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vec4;

// 40 bytes
typedef struct PhysicsEntity {
    Vec3  position;
    Vec3  velocity;
    Vec3  angular_velocity; // normal of rotation, magnitude is speed
    float mass;
} PhysicsEntity;

typedef struct Constraint {
    //
} Constraint;

typedef struct Collision {
    Vec3 position;
    Vec3 direction;
    uidx entities[2];
} Collision;

typedef struct Collider {

} Collider;

HBuff* detect_collisions(Arena* physics_arena, const Collider* colliders, uidx colliders_len) {
    HBuff* collisions = arena_pre_alloc(physics_arena);

    // TODO parallelize this with an atomic length that gets incremented?

    arena_post_alloc(physics_arena, collisions->len);
    return collisions;
}

// This is here in the steps in case something applies forces to all contacts
// magnets?
// Are these just constraints?
void apply_forces(Arena* physics_arena, HBuff* collisions) {
    HBuff* forces = arena_pre_alloc(physics_arena);

    arena_post_alloc(physics_arena, forces->len);
}

// Can we avoid this entirely by double buffering the collisions?
// No we cannot. We need to write to both collided obj at once.
void graph_color(const HBuff* collisions) {
    //
}

typedef struct Color {
    HBuff* vertices;
} Color;

void avbd_iteration(HBuff* colors) {
    // for each color
    for (uidx c_idx = 0; c_idx < colors->len; c_idx++) {
        Color color = *(Color*)(colors->data + c_idx);
        // for each vertex in color (these are run on different threads)
        for (uidx v_idx = 0; v_idx < color.vertices->len; v_idx++) {
            //
        }

        // Sync threads to ensure all are done with their slice of this color before moving on
        // sync_color();
    }
}
