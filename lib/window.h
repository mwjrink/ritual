#pragma once

#include <arena.h>
#include <bp.h>

// #ifdef WAYLAND // GLFW literally checks the env for runtime shit
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

typedef struct Window {
    // struct wl_display* display;
    struct wl_surface* surface;
} Window;

struct wl_compositor* compositor;
struct wl_display*    display;
struct wl_pointer*    pointer;
struct wl_seat*       seat;
struct wl_shell*      shell;
struct wl_shm*        shm;

static const struct wl_registry_listener registry_listener;
static const struct wl_pointer_listener  pointer_listener;

static void registry_global(void* data, struct wl_registry* registry, u32 name, const char* interface, u32 version) {

    if (strcmp(interface, wl_compositor_interface.name) == 0)
        compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(version, 4));
    else if (strcmp(interface, wl_shm_interface.name) == 0)
        shm = wl_registry_bind(registry, name, &wl_shm_interface, min(version, 1));
    else if (strcmp(interface, wl_shell_interface.name) == 0)
        shell = wl_registry_bind(registry, name, &wl_shell_interface, min(version, 1));
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat = wl_registry_bind(registry, name, &wl_seat_interface, min(version, 2));
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, NULL);
    }
}

static void registry_global_remove(void* a, struct wl_registry* b, u32 c) {}

#include <sys/stat.h>
typedef u32 pixel;
struct pool_data {
    int      fd;
    pixel*   memory;
    unsigned capacity;
    unsigned size;
};

// Window open_window(const Arena* arena) {
Window open_window() {
    // struct wl_display* display = wl_display_connect(NULL);
    display = wl_display_connect(NULL);
    if (display == NULL) {
        // eprintf("WAYLAND: Error connecting to the display.");
        // return NULL;
    }

    struct wl_registry*         registry = wl_display_get_registry(display);
    struct wl_registry_listener listener = {
        .global = registry_global,
        .global_remove = registry_global_remove,
    };
    wl_registry_add_listener(registry, &listener, NULL);
    wl_display_roundtrip(display); // get all the stuff through the callback
    wl_registry_destroy(registry);

    // struct wl_shm_pool* pool;
    // // create mem pool
    // {
    //     struct pool_data*   data;
    //     struct wl_shm_pool* pool;
    //     struct stat         stat;
    //
    //     if (fstat(file, &stat) != 0) {
    //         pool = NULL;
    //         goto pool_create_end;
    //     }
    //
    //     data = malloc(sizeof(struct pool_data));
    //
    //     if (data == NULL) {
    //         pool = NULL;
    //         goto pool_create_end;
    //     }
    //
    //     data->capacity = stat.st_size;
    //     data->size = 0;
    //     data->fd = file;
    //
    //     data->memory = mmap(0, data->capacity, PROT_READ, MAP_SHARED, data->fd, 0);
    //
    //     if (data->memory == MAP_FAILED)
    //         goto cleanup_alloc;
    //
    //     pool = wl_shm_create_pool(shm, data->fd, data->capacity);
    //
    //     if (pool == NULL)
    //         goto cleanup_mmap;
    //
    //     wl_shm_pool_set_user_data(pool, data);
    //
    //     goto pool_create_end;
    //
    // cleanup_mmap:
    //     munmap(data->memory, data->capacity);
    // cleanup_alloc:
    //     free(data);
    //     goto pool_create_end;
    // pool_create_end: {}
    // }

    struct wl_surface* surface = wl_compositor_create_surface(compositor);

    // wl_surface_attach(surface, buffer, x, y);

    // wl_display_prepare_read(display);

    wl_display_dispatch(display);
    wl_display_flush(display);

    return (Window){
        .surface = surface,
    };
}

struct pointer_data {
    struct wl_surface* surface;
    struct wl_buffer*  buffer;
    int32_t            hot_spot_x;
    int32_t            hot_spot_y;
    struct wl_surface* target_surface;
};

void close_window(Window* window) {
    wl_surface_destroy(window->surface);

    // if we have multiple windows, don't
    // destroy everything until all windows are closed.
    wl_display_disconnect(display);
    display = NULL;
}

static void pointer_enter(void*              data,
                          struct wl_pointer* wl_pointer,
                          uint32_t           serial,
                          struct wl_surface* surface,
                          wl_fixed_t         surface_x,
                          wl_fixed_t         surface_y) {
    struct pointer_data* pointer_data;

    pointer_data = wl_pointer_get_user_data(wl_pointer);
    pointer_data->target_surface = surface;
    wl_surface_attach(pointer_data->surface, pointer_data->buffer, 0, 0);
    wl_surface_commit(pointer_data->surface);
    wl_pointer_set_cursor(
        wl_pointer, serial, pointer_data->surface, pointer_data->hot_spot_x, pointer_data->hot_spot_y);
}

static void pointer_leave(void* data, struct wl_pointer* wl_pointer, uint32_t serial, struct wl_surface* wl_surface) {}

static void
pointer_motion(void* data, struct wl_pointer* wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {}

static void pointer_button(
    void* data, struct wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    struct pointer_data* pointer_data;
    void (*callback)(uint32_t);

    pointer_data = wl_pointer_get_user_data(wl_pointer);
    callback = wl_surface_get_user_data(pointer_data->target_surface);
    if (callback != NULL)
        callback(button);
}

static void pointer_axis(void* data, struct wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {}

static const struct wl_pointer_listener pointer_listener = {.enter = pointer_enter,
                                                            .leave = pointer_leave,
                                                            .motion = pointer_motion,
                                                            .button = pointer_button,
                                                            .axis = pointer_axis};

// #endif
