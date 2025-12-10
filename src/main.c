#include <arena.h>
#include <bp.h>
#include <graphics.h>
#include <logger.h>
#include <window.h>

// SLEEP
#include <unistd.h>

int main() {
    Arena general_arena = arena_create();
    // Arena      scratch_arena = arena_create();
    Window                   window = open_window();
    VkInstance               instance = create_instance(&general_arena);
    VkDebugUtilsMessengerEXT debug_messenger = setup_debug_messenger(instance);

    // /sys/devices/system/cpu/cpu31/cache/index3/size
    // P & E cores
    // /sys/devices/cpu_core/cpus: Lists all P-cores.
    // /sys/devices/cpu_atom/cpus: Lists all E-cores.

    // VERBOSE_LOG("This is a VERBOSE log test %i %c %s", 1, '2', "3");
    // INFO_LOG("This is a INFO log test %i %c %s", 1, '2', "3");
    // DEBUG_LOG("This is a DEBUG log test %i %c %s", 1, '2', "3");
    // WARNING_LOG("This is a WARNING log test %i %c %s", 1, '2', "3");
    // ERROR_LOG("This is a ERROR log test %i %c %s", 1, '2', "3");
    // CRITICAL_LOG("This is a CRITICAL log test %i %c %s", 1, '2', "3");
    // sleep(5);

    cleanup_instance(instance, debug_messenger);
    close_window(&window);
    return 0;
}

// Task List
//   -[] draw something to the screen
//   -[] logging/tracing (socket? file? shared_mem?)
//   -[] physics
