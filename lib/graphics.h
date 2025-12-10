#pragma once

#include <arena.h>
#include <bp.h>
#include <buff.h>
#include <logger.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#ifdef PRINT_AVAILABLE_LAYERS
#include <stdio.h>

void print_available_layers() {
    uint32_t layerCount;
    // First call to get the count
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties pLayers[layerCount];
    // Second call to get the actual layer data
    vkEnumerateInstanceLayerProperties(&layerCount, pLayers);

    printf("Available Vulkan Layers: \n");
    for (uint32_t i = 0; i < layerCount; ++i) {
        printf("    - %s (Version: %i)\n", pLayers[i].layerName, pLayers[i].specVersion);
    }
}
#endif

#ifdef PRINT_AVAILABLE_EXTENSIONS
#include <stdio.h>

void print_available_extensions() {
    u32 extension_count;
    // First call to get the count
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkExtensionProperties pExtensions[extension_count];
    // Second call to get the actual layer data
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, pExtensions);

    printf("Available Vulkan Extensions: \n");
    for (u32 i = 0; i < extension_count; ++i) {
        printf("    - %s (Version: %i)\n", pExtensions[i].extensionName, pExtensions[i].specVersion);
    }
}
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback( //
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* cb_data,
    void*                                       user_data) {

    char* message_type_tag = NULL;

    // TODO do something with:
    // cb_data->pObjects & objectCount
    // cb_data->pCmdBufLabels & count
    // cb_data->pQueueLabels & count
    // Track the objects in the debugger?

    switch (message_type) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
            message_type_tag = A_START A_BOLD A_BG(0, 64, 0) A_END " GENERAL " A_RESET " | ";
        } break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
            message_type_tag = A_START A_BOLD A_BG(0, 64, 0) A_END " VALIDATION " A_RESET " | ";
        } break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
            message_type_tag = A_START A_BOLD A_BG(0, 64, 0) A_END " PERFORMANCE " A_RESET " | ";
        } break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: {
            message_type_tag = A_START A_BOLD A_BG(0, 64, 0) A_END " DEVICE_ADDRESS_BINDING " A_RESET " | ";
        } break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT: {
            message_type_tag = A_START A_BOLD A_BG(0, 64, 0) A_END " MAX_BITS " A_RESET " | ";
        } break;
        default: {
            message_type_tag = "UNKNOWN TYPE";
        } break;
    }

    switch (message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            VERBOSE_LOG("%s %i - %s: %s",
                        message_type_tag,
                        cb_data->messageIdNumber,
                        cb_data->pMessageIdName,
                        cb_data->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            INFO_LOG("%s %i - %s: %s",
                     message_type_tag,
                     cb_data->messageIdNumber,
                     cb_data->pMessageIdName,
                     cb_data->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            WARNING_LOG("%s %i - %s: %s",
                        message_type_tag,
                        cb_data->messageIdNumber,
                        cb_data->pMessageIdName,
                        cb_data->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            ERROR_LOG("%s %i - %s: %s",
                      message_type_tag,
                      cb_data->messageIdNumber,
                      cb_data->pMessageIdName,
                      cb_data->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: {
            CRITICAL_LOG("%s %i - %s: %s",
                         message_type_tag,
                         cb_data->messageIdNumber,
                         cb_data->pMessageIdName,
                         cb_data->pMessage);
        } break;
        default: {
            CRITICAL_LOG("UKNOWN LOG LEVEL, MAYBE A COMBO?");
            DEBUG_LOG("%s %i - %s: %s",
                      message_type_tag,
                      cb_data->messageIdNumber,
                      cb_data->pMessageIdName,
                      cb_data->pMessage);
        } break;
    }

    return VK_FALSE;
}

// struct Instance;

// TODO move this to a debug file
// Should these be loaded in a separate header? vk_ext_debug.h
VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks*              pAllocator,
                                      VkDebugUtilsMessengerEXT*                 pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// TODO move this to a debug file
VkDebugUtilsMessengerEXT setup_debug_messenger(VkInstance instance) {
    VkDebugUtilsMessengerCreateInfoEXT create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = NULL;

    VkDebugUtilsMessengerEXT debug_messenger;
    VkResult                 result = CreateDebugUtilsMessengerEXT(instance, &create_info, NULL, &debug_messenger);
    if (result != VK_SUCCESS) {
        CRITICAL_LOG("Failed to setup debug messenger!");
        exit(1);
    }

    return debug_messenger;
}

// TODO move this to a debug file
// Should these be loaded in a separate header?
void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                   VkDebugUtilsMessengerEXT     debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkInstance create_instance(Arena* arena) {

#ifdef PRINT_AVAILABLE_LAYERS
    print_available_layers();
#endif

#ifdef PRINT_AVAILABLE_EXTENSIONS
    print_available_extensions();
#endif

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Ritual Test App";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Ritual";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    AllocDynList ext_names = alloc_list_create_with_cap(arena, sizeof(char*), 8);

#ifdef __linux__
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_surface"));
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_wayland_surface"));
#elif defined(x11)
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_surface"));
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_xlib_surface"));
    // OR
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_xcb_surface"));
#elif defined(__APPLE__)
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_surface"));
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_MVK_macos_surface"));
    // OR
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_EXT_metal_surface"));

    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME));
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#elif defined(_WIN32)
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_surface"));
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, "VK_KHR_win32_surface"));
#endif

    AllocDynList layer_names = alloc_list_create_with_cap(arena, sizeof(char*), 8);
#ifdef __DEBUG
    alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena, VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

    alloc_list_push_ptr(arena, &layer_names, arena_copy_str(arena, "VK_LAYER_KHRONOS_validation"));
    // VK_LAYER_RENDERDOC_Capture
    // VK_LAYER_LUNARG_api_dump
    // VK_LAYER_LUNARG_monitor
    // VK_LAYER_LUNARG_screenshot
    // VK_LAYER_KHRONOS_validation
#endif
    // alloc_list_push_ptr(arena, &ext_names, arena_copy_str(arena,
    // VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME));

    create_info.enabledExtensionCount = ext_names.len;
    create_info.ppEnabledExtensionNames = ext_names.data;

    create_info.enabledLayerCount = layer_names.len;
    create_info.ppEnabledLayerNames = layer_names.data;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_callback;
    debug_create_info.pUserData = NULL;

    create_info.pNext = &debug_create_info;

    VkInstance instance = {0};
    // Second param here is allocator, provide arena impl?
    // It SHOULD NOT matter:
    // https://docs.vulkan.org/spec/latest/chapters/memory.html#memory-allocation
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        CRITICAL_LOG("Failed to create vk instance. Shutting down.");
        exit(1);
    }

    arena_rollback(arena);

    return instance;
}

void cleanup_instance(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger) {
    DestroyDebugUtilsMessengerEXT(instance, debug_messenger, NULL);
    vkDestroyInstance(instance, NULL); //
}
