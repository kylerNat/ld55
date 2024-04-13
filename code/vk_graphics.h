#ifndef GRAPHICS
#define GRAPHICS

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <utils/logging.h>
#include <utils/misc.h>
#define MSF_GIF_IMPL
#include <msf_gif.h>

#include "rectangle_packer.h"

const char* vk_error_string(int error)
{
    switch(error) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
            // Provided by VK_VERSION_1_1
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
            // Provided by VK_VERSION_1_1
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            // Provided by VK_VERSION_1_2
        case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
            // Provided by VK_VERSION_1_2
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            // Provided by VK_VERSION_1_3
        case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
            // Provided by VK_KHR_surface
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
            // Provided by VK_KHR_surface
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            // Provided by VK_KHR_swapchain
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
            // Provided by VK_KHR_swapchain
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
            // Provided by VK_KHR_display_swapchain
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            // Provided by VK_EXT_debug_report
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
            // Provided by VK_NV_glsl_shader
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
            // Provided by VK_KHR_video_queue
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
            // Provided by VK_EXT_image_drm_format_modifier
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
            // Provided by VK_KHR_global_priority
        case VK_ERROR_NOT_PERMITTED_KHR: return "VK_ERROR_NOT_PERMITTED_KHR";
            // Provided by VK_EXT_full_screen_exclusive
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
            // Provided by VK_KHR_deferred_host_operations
        case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
            // Provided by VK_KHR_deferred_host_operations
        case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
            // Provided by VK_KHR_deferred_host_operations
        case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
            // Provided by VK_KHR_deferred_host_operations
        case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
            // Provided by VK_KHR_video_encode_queue
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
            // Provided by VK_EXT_image_compression_control
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
            // Provided by VK_EXT_shader_object
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
            // Provided by VK_KHR_maintenance1
        // case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
            // Provided by VK_KHR_external_memory
        // case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
            // Provided by VK_EXT_descriptor_indexing
        // case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
            // Provided by VK_EXT_global_priority
        // case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
            // Provided by VK_EXT_buffer_device_address
        // case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
            // Provided by VK_KHR_buffer_device_address
        // case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR";
            // Provided by VK_EXT_pipeline_creation_cache_control
        // case VK_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
            // Provided by VK_EXT_pipeline_creation_cache_control
        // case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
            // Provided by VK_EXT_shader_object
        // case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
        default: return "Unknown Vulkan Error";
    }
}

#define vk_assert(...) {if(!(error == VK_SUCCESS)) {log_error("", vk_error_string(error), " in ", __FILE__, " (", __LINE__, "): ", ##__VA_ARGS__);}}

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void*                                       user_data) {

    if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        log_error("validation layer: ", callback_data->pMessage, "\n");
    } else if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log_warning("validation layer: ", callback_data->pMessage, "\n");
    }

    return VK_FALSE;
}

struct bitmap_t {
    uint8_4* data;
    int_2 size;

};

bitmap_t create_bitmap(int_2 size)
{
    bitmap_t bitmap = {
        .data = (uint8_4*) dynamic_alloc(size.x*size.y*sizeof(bitmap.data[0])),
        .size = size,
    };
    //TODO
    return bitmap;
};

///////////////////////////////

#define COLOR_FORMAT VK_FORMAT_R8G8B8A8_SRGB
#define DEPTH_FORMAT VK_FORMAT_D32_SFLOAT

int draw_width = 1280;
int draw_height = 720;

struct replay_buffer {
    VkImage* images;
    VkImageView* image_views;

    uint8_4* data;
    int_2 resolution;
    int buffer_length;
    int current_frame;
    int n_frames;

    real centiseconds;
};

void init_replay_buffer(replay_buffer* rb, int_2 resolution, int frames)
{
    //vk stuff



    //replay data
    rb->data = (uint8_4*) dynamic_alloc(resolution.x*resolution.y*frames*sizeof(rb->data[0]));
    rb->resolution = resolution;
    rb->buffer_length = frames;
    rb->current_frame = 0;
    rb->n_frames = 0;

    rb->centiseconds = 3;
};

struct texture_t {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView image_view;
};

#define font_resolution 1024

struct font_info {
    texture_t texture;
    stbtt_fontinfo info;
    stbtt_packedchar* char_data;
    real size;
};

struct render_context {
    real fov;
    real_3 camera_pos;
    real_3x3 camera_axes;
    real_4x4 camera; //full matrix with perspective and projection

    real_4 background_color;
    real_4 foreground_color;
    real_4 highlight_color;

    int_2 resolution;
};

const int MAX_QUEUED_FRAMES = 2;

struct vulkan_context {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    uint graphics_family_index;
    VkQueue graphics_queue;
    uint present_family_index;
    VkQueue present_queue;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;

    VkSwapchainKHR swapchain;
    VkImage* swapchain_images;
    VkImageView* swapchain_image_views;
    uint32 n_swapchain_images;
    VkFormat swapchain_format;
    VkExtent2D swapchain_extent;
    VkFramebuffer* swapchain_framebuffers;
    VkRenderPass swapchain_render_pass;

    VkImage* replay_images;
    VkImageView* replay_image_views;
    VkDeviceMemory replay_image_memory;
    VkFramebuffer* replay_framebuffers;
    VkRenderPass replay_render_pass;
    VkDescriptorPool replay_descriptor_pool;
    uint32 max_replay_frames;
    uint32 n_replay_frames;
    uint32 replay_index;
    int_2 replay_resolution;
    real replay_centiseconds;
    int replay_mode;
    int old_replay_mode;
    bool do_gif_frame;

    VkImage color_image;
    VkImageView color_image_view;
    VkDeviceMemory color_image_memory;
    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;
    VkFramebuffer render_framebuffer;

    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffers[MAX_QUEUED_FRAMES];
    VkCommandPool single_time_command_pool;
    VkCommandBuffer single_time_command_buffer;

    VkSemaphore image_available_semaphores[MAX_QUEUED_FRAMES];
    VkSemaphore render_finished_semaphores[MAX_QUEUED_FRAMES];
    VkFence in_flight_fences[MAX_QUEUED_FRAMES];

    bool framebuffer_resized;
    bool swapchain_valid;

    uint32 current_frame;

    //
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer dynamic_vertex_buffers[MAX_QUEUED_FRAMES];
    VkDeviceMemory dynamic_vertex_buffers_memory[MAX_QUEUED_FRAMES];
    void* dynamic_vertex_buffers_mapped[MAX_QUEUED_FRAMES];
    uint32 dynamic_vertex_buffers_used[MAX_QUEUED_FRAMES];

    VkBuffer uniform_buffers[MAX_QUEUED_FRAMES];
    VkDeviceMemory uniform_buffers_memory[MAX_QUEUED_FRAMES];
    void* uniform_buffers_mapped[MAX_QUEUED_FRAMES];

    VkBuffer shader_storage_buffers[MAX_QUEUED_FRAMES];
    VkDeviceMemory shader_storage_buffers_memory[MAX_QUEUED_FRAMES];
    void* shader_storage_buffers_mapped[MAX_QUEUED_FRAMES];

    texture_t test_texture;
    VkSampler texture_sampler;

    VkDescriptorSetLayout global_descriptor_set_layout;
    VkDescriptorSet global_descriptor_sets[MAX_QUEUED_FRAMES];

    VkDescriptorSetLayout postprocess_descriptor_set_layout;
    VkDescriptorSet postprocess_descriptor_set;

    VkDescriptorSetLayout replay_descriptor_set_layout;
    VkDescriptorSet replay_descriptor_set;

    VkDescriptorPool descriptor_pool;

    int_2 render_resolution;

    real aspect_ratio;
    real fov;
    real_3 camera_pos;
    real_3x3 camera_axes;
    real_4x4 camera; //full matrix with perspective and projection
    real time;

    font_info default_font;

    real frame_time;
};

vulkan_context vkon;

#define vk_load_operation(fn) PFN_##fn _##fn = 0;
#include "vk_functions_list.h"

void load_vk_functions()
{
    HMODULE vk_module_handle = GetModuleHandle("vulkan-1.dll");

#define vk_load_operation(fn) _##fn = (PFN_##fn) vkGetInstanceProcAddr(vkon.instance, #fn); assert(_##fn, #fn " could not be loaded");
#include "vk_functions_list.h"
}

VkCommandBuffer begin_single_time_command_buffer()
{
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBeginCommandBuffer(vkon.single_time_command_buffer, &begin_info);

    return vkon.single_time_command_buffer;
}

void end_single_time_command_buffer()
{
    vkEndCommandBuffer(vkon.single_time_command_buffer);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkon.single_time_command_buffer,
    };

    vkQueueSubmit(vkon.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkon.graphics_queue);

    vkResetCommandPool(vkon.device, vkon.single_time_command_pool, 0);
}

uint32 find_memory_type(uint32 type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(vkon.physical_device, &memory_properties);
    for(uint32 i = 0;; i++) {
        assert(i < memory_properties.memoryTypeCount, "could not find suitable memory type for vertex buffer");
        if((type_filter & (1<<i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

void create_vk_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory)
{
    int error = 0;
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    error = vkCreateBuffer(vkon.device, &buffer_info, 0, buffer);
    vk_assert("could not create vertex buffer");

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(vkon.device, *buffer, &memory_requirements);

    VkMemoryAllocateInfo mem_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, properties),
    };

    error = vkAllocateMemory(vkon.device, &mem_alloc_info, 0, buffer_memory); //BIG TODO: real memory system

    vk_assert("could not allocate vertex buffer memory");

    vkBindBufferMemory(vkon.device, *buffer, *buffer_memory, 0);
}

void create_image(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory)
{
    int error = 0;

    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    error = vkCreateImage(vkon.device, &image_info, 0, image);
    vk_assert("could not create image");

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(vkon.device, *image, &memory_requirements);
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };

    error = vkAllocateMemory(vkon.device, &alloc_info, 0, image_memory); //TODO: memory allocation
    vk_assert("could not allocate image memory");

    vkBindImageMemory(vkon.device, *image, *image_memory, 0);
}

void transition_image_layout(VkCommandBuffer command_buffer, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
    };

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    if(old_layout==VK_IMAGE_LAYOUT_UNDEFINED && new_layout==VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if(old_layout==VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout==VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        assert(0, "unsupported image layout transition");
    }

    vkCmdPipelineBarrier(command_buffer,
                         src_stage, dst_stage,
                         0,
                         0, 0,
                         0, 0,
                         1, &barrier);
}

VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
{
    int error = 0;

    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = aspect_flags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkImageView image_view;
    error = vkCreateImageView(vkon.device, &view_info, 0, &image_view);
    vk_assert("could not create image view");

    return image_view;
}

VkImageView create_image_view(VkImage image, VkFormat format)
{
    return create_image_view(image, format, VK_IMAGE_ASPECT_COLOR_BIT);
}

texture_t create_texture(int_2 size, byte* image_data, VkFormat format)
{
    int error = 0;

    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView image_view;

    VkDeviceSize image_size = size.x*size.y*4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_vk_buffer(image_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &staging_buffer,
                     &staging_buffer_memory);
    void* data;
    vkMapMemory(vkon.device, staging_buffer_memory, 0, image_size, 0, &data);
    memcpy(data, image_data, image_size);
    vkUnmapMemory(vkon.device, staging_buffer_memory);

    create_image(size.x, size.y,
                 format,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &image,
                 &image_memory);

    VkCommandBuffer command_buffer = begin_single_time_command_buffer();

    transition_image_layout(command_buffer, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0,0,0},
        .imageExtent = {size.x, size.y, 1},
    };

    vkCmdCopyBufferToImage(command_buffer, staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    transition_image_layout(command_buffer, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    end_single_time_command_buffer();

    vkDestroyBuffer(vkon.device, staging_buffer, 0);
    vkFreeMemory(vkon.device, staging_buffer_memory, 0);

    image_view = create_image_view(image, format);

    return {image, image_memory, image_view};
}

texture_t create_texture(int_2 size, byte* image_data)
{
    return create_texture(size, image_data, VK_FORMAT_R8G8B8A8_SRGB);
}

void cleanup_texture(texture_t texture)
{
    vkDestroyImageView(vkon.device, texture.image_view, 0);
    vkDestroyImage(vkon.device, texture.image, 0);
    vkFreeMemory(vkon.device, texture.memory, 0);
}

texture_t load_image_to_texture(char* filename)
{
    int_2 size = {};
    byte* image_data = stbi_load(filename, &size.x, &size.y, 0, 4);
    assert(image_data, " could not load ", filename, "\n");
    texture_t texture = create_texture(size, image_data);
    stbi_image_free(image_data);

    return texture;
}

font_info load_font(char* font_filename, real font_size)
{
    uint8* font_data = (uint8*) load_file_0_terminated(font_filename);
    int error = 0;

    stbtt_pack_context spc = {};

    int num_chars = 1024;
    stbtt_pack_range ranges[] = {
        {
            .font_size = font_size,
            .first_unicode_codepoint_in_range = 0,
            .num_chars = num_chars,
            .chardata_for_range = (stbtt_packedchar*) stalloc_clear(sizeof(stbtt_packedchar)*num_chars),
        },
    };

    uint8* pixels = (uint8*) stalloc(4*sq(font_resolution));

    error = stbtt_PackBegin(&spc, pixels, font_resolution, font_resolution, 0, 1, 0);
    assert(error);

    stbtt_PackSetOversampling(&spc, 2, 2);
    // error = stbtt_PackFontRanges(&spc, font_data, 0, ranges, len(ranges));
    //expanding out stbtt_PackFontRanges so we can get out the stbtt_fontinfo
    stbtt_fontinfo info;
    int font_index = 0;
    {
        int i,j,n, return_value = 1;
        stbrp_rect    *rects;

        n = 0;
        for (i=0; i < len(ranges); ++i)
            n += ranges[i].num_chars;

        rects = (stbrp_rect *) STBTT_malloc(sizeof(*rects) * n, spc.user_allocator_context);
        assert(rects, "could not allocate font rects");

        info.userdata = spc.user_allocator_context;
        stbtt_InitFont(&info, font_data, stbtt_GetFontOffsetForIndex(font_data,font_index));

        n = stbtt_PackFontRangesGatherRects(&spc, &info, ranges, len(ranges), rects);

        stbtt_PackFontRangesPackRects(&spc, rects, n);

        return_value = stbtt_PackFontRangesRenderIntoRects(&spc, &info, ranges, len(ranges), rects);

        STBTT_free(rects, spc.user_allocator_context);
        error = return_value;
    }
    assert(error);

    stbtt_PackEnd(&spc);

    texture_t font_texture = create_texture({font_resolution, font_resolution}, pixels, VK_FORMAT_R8_SRGB);

    stunalloc(pixels);

    return {font_texture, info, ranges[0].chardata_for_range, font_size};
}

VkShaderModule create_shader_module(const uint32_t* code, size_t code_size)
{
    int error;
    VkShaderModuleCreateInfo shader_create = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code_size,
        .pCode = code,
    };

    VkShaderModule module;
    error = vkCreateShaderModule(vkon.device, &shader_create, 0, &module);
    vk_assert("could not create shader module");
    return module;
}

VkShaderModule create_shader_module_from_file(char* filename)
{
    file_t file = open_file(filename, OPEN_EXISTING);
    size_t code_size = sizeof_file(file);
    byte* code = stalloc(code_size);
    read_from_disk(code, file, 0, code_size);
    close_file(file);
    VkShaderModule module = create_shader_module((uint32*) code, code_size);
    stunalloc(code);
    return module;
}

#include "aabb_tree.h"

#pragma pack(push, 1)
struct test_vertex {
    real_3 p;
    real_3 color;
    alignas(16) real_2 uv;
};

struct global_uniform_buffer {
    real_4x4 t;
    real_4 camera_axes[3];
    real_3 camera_pos;
    real fov;
    real aspect_ratio;
    real time;
};

struct global_ssbo {
    int n_primitives;
    alignas(16) primitive_t primitives[MAX_AABB_PRIMITIVES];

#ifdef DEBUG
    int n_nodes;
    alignas(16) aabb_t nodes[MAX_AABB_NODES];
#endif
};
#pragma pack(pop)

struct pipeline_info;
typedef void (*pipeline_create_function)(pipeline_info* pipeline);

struct pipeline_info
{
    pipeline_create_function create_function;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
};

pipeline_info pipeline_list[256] = {};
int n_pipeline_list = 0;

pipeline_info* add_pipeline(pipeline_create_function create_function)
{
    pipeline_info* pipeline = pipeline_list+(n_pipeline_list++);
    pipeline->create_function = create_function;
    return pipeline;
}

void cleanup_pipeline(pipeline_info pipeline)
{
    vkDestroyPipeline(vkon.device, pipeline.pipeline, 0);
    vkDestroyPipelineLayout(vkon.device, pipeline.pipeline_layout, 0);
};

#include "circle_pipeline.h"
#include "rectangle_pipeline.h"
#include "sphere_pipeline.h"
#include "world_pipeline.h"
#include "lightmap_pipeline.h"
#include "lightprobe_visualization_pipeline.h"
#include "aabb_visualization_pipeline.h"
#include "text_pipeline.h"
#include "fullscreen_image_pipeline.h"
#include "replay_pipeline.h"

void cleanup_pipelines()
{
    for(int i = 0; i < n_pipeline_list; i++) {
        cleanup_pipeline(pipeline_list[i]);
    }

    vkDestroyPipeline(vkon.device, vkon.graphics_pipeline, 0);
    vkDestroyPipelineLayout(vkon.device, vkon.pipeline_layout, 0);
}

void create_pipelines()
{
    int error = 0;

    for(int i = 0; i < n_pipeline_list; i++) {
        pipeline_list[i].create_function(pipeline_list+i);
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vkon.global_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &vkon.pipeline_layout);
    vk_assert("could not create pipeline layout");
}

void recreate_pipelines()
{
    int error = 0;
    vkDeviceWaitIdle(vkon.device);

    cleanup_pipelines();
    create_pipelines();
}

struct swapchain_support_details
{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR present_mode;
    int format_score;
    int present_score;
    VkExtent2D extent;
};

swapchain_support_details query_swapchain_support(VkPhysicalDevice physical_device)
{
    swapchain_support_details swapchain_support = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, vkon.surface, &swapchain_support.capabilities);

    uint32 n_formats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vkon.surface, &n_formats, 0);
    VkSurfaceFormatKHR* formats = stalloc_typed(n_formats, VkSurfaceFormatKHR);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vkon.surface, &n_formats, formats);

    uint32 n_present_modes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vkon.surface, &n_present_modes, 0);
    VkPresentModeKHR* present_modes = stalloc_typed(n_present_modes, VkPresentModeKHR);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vkon.surface, &n_present_modes, present_modes);

    swapchain_support.format_score = -1;
    for(int j = 0; j < n_formats; j++) {
        int f_score = 0;
        f_score += 20*(formats[j].format == VK_FORMAT_B8G8R8A8_SRGB);
        f_score += 20*(formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
        if(f_score > swapchain_support.format_score) {
            swapchain_support.format_score = f_score;
            swapchain_support.format = formats[j];
        }
    }

    swapchain_support.present_score = -1;
    for(int j = 0; j < n_present_modes; j++) {
        int p_score = 0;
        p_score += 10*(present_modes[j] == VK_PRESENT_MODE_IMMEDIATE_KHR);
        if(p_score > swapchain_support.present_score) {
            swapchain_support.present_score = p_score;
            swapchain_support.present_mode = present_modes[j];
        }
    }

    swapchain_support.extent = swapchain_support.capabilities.currentExtent;
    if(swapchain_support.extent.width == INT_MAX) {
        swapchain_support.extent.width = swapchain_support.capabilities.maxImageExtent.width;
        swapchain_support.extent.height = swapchain_support.capabilities.maxImageExtent.height;
    }

    stunalloc(present_modes);
    stunalloc(formats);

    return swapchain_support;
}

void create_render_framebuffer()
{
    int error = 0;

    //color buffer
    create_image(vkon.render_resolution.x, vkon.render_resolution.y,
                 COLOR_FORMAT,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &vkon.color_image,
                 &vkon.color_image_memory);
    vkon.color_image_view = create_image_view(vkon.color_image, COLOR_FORMAT, VK_IMAGE_ASPECT_COLOR_BIT);

    //depth buffer
    create_image(vkon.render_resolution.x, vkon.render_resolution.y,
                 DEPTH_FORMAT,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &vkon.depth_image,
                 &vkon.depth_image_memory);
    vkon.depth_image_view = create_image_view(vkon.depth_image, DEPTH_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT);

    //framebuffer
    VkImageView attachments[] = {
        vkon.color_image_view,
        vkon.depth_image_view,
    };

    VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vkon.render_pass,
        .attachmentCount = len(attachments),
        .pAttachments = attachments,
        .width = vkon.render_resolution.x,
        .height = vkon.render_resolution.y,
        .layers = 1,
    };

    error = vkCreateFramebuffer(vkon.device, &framebuffer_info, 0, &vkon.render_framebuffer);
    vk_assert("could not create render framebuffer");
}

void cleanup_render_framebuffer()
{
    vkDestroyFramebuffer(vkon.device, vkon.render_framebuffer, 0);

    vkDestroyImageView(vkon.device, vkon.depth_image_view, 0);
    vkDestroyImage(vkon.device, vkon.depth_image, 0);
    vkFreeMemory(vkon.device, vkon.depth_image_memory, 0);

    vkDestroyImageView(vkon.device, vkon.color_image_view, 0);
    vkDestroyImage(vkon.device, vkon.color_image, 0);
    vkFreeMemory(vkon.device, vkon.color_image_memory, 0);
}

void create_replay_framebuffers()
{
    int error = 0;

    if(vkon.max_replay_frames == 0) return;

    vkon.replay_images       = dynamic_alloc_typed(vkon.max_replay_frames, VkImage);
    vkon.replay_image_views  = dynamic_alloc_typed(vkon.max_replay_frames, VkImageView);
    vkon.replay_framebuffers = dynamic_alloc_typed(vkon.max_replay_frames, VkFramebuffer);

    for(int i = 0; i < vkon.max_replay_frames; i++) {
        VkImageCreateInfo image_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = COLOR_FORMAT,
            .extent = {vkon.replay_resolution.x, vkon.replay_resolution.y, 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        error = vkCreateImage(vkon.device, &image_info, 0, vkon.replay_images+i);
        vk_assert("could not create image");
    }

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(vkon.device, vkon.replay_images[0], &memory_requirements);
    uint32 frame_size = memory_requirements.size;

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vkon.max_replay_frames*frame_size,
        .memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };
    error = vkAllocateMemory(vkon.device, &alloc_info, 0, &vkon.replay_image_memory); //TODO: memory allocation
    vk_assert("could not allocate replay images memory");

    for(int i = 0; i < vkon.max_replay_frames; i++) {

        vkBindImageMemory(vkon.device, vkon.replay_images[i], vkon.replay_image_memory, frame_size*i);

        vkon.replay_image_views[i] = create_image_view(vkon.replay_images[i], COLOR_FORMAT);

        //framebuffer
        VkImageView attachments[] = {
            vkon.replay_image_views[i],
        };

        VkFramebufferCreateInfo framebuffer_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vkon.replay_render_pass,
            .attachmentCount = len(attachments),
            .pAttachments = attachments,
            .width = vkon.replay_resolution.x,
            .height = vkon.replay_resolution.y,
            .layers = 1,
        };

        error = vkCreateFramebuffer(vkon.device, &framebuffer_info, 0, &vkon.replay_framebuffers[i]);
        vk_assert("could not create render framebuffer");
    }

    { //replay descriptor set
        VkDescriptorPoolSize pool_sizes[] = {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = vkon.max_replay_frames,
            }
        };

        VkDescriptorPoolCreateInfo desc_pool_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = len(pool_sizes),
            .pPoolSizes = pool_sizes,
        };

        error = vkCreateDescriptorPool(vkon.device, &desc_pool_info, 0, &vkon.replay_descriptor_pool);
        vk_assert("could not create replay descriptor pool");

        //descriptor set layout
        VkDescriptorSetLayoutBinding layout_bindings[] = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = vkon.max_replay_frames,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }
        };

        VkDescriptorSetLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = len(layout_bindings),
            .pBindings = layout_bindings,
        };

        error = vkCreateDescriptorSetLayout(vkon.device, &layout_info, 0, &vkon.replay_descriptor_set_layout);
        vk_assert("could not create replay descriptor set layout");

        VkDescriptorSetAllocateInfo desc_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkon.replay_descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &vkon.replay_descriptor_set_layout,
        };

        error = vkAllocateDescriptorSets(vkon.device, &desc_alloc_info, &vkon.replay_descriptor_set);
        vk_assert("could not allocate replay descriptor sets");

        VkDescriptorImageInfo* image_infos = stalloc_typed(vkon.max_replay_frames, VkDescriptorImageInfo);
        VkWriteDescriptorSet* descriptor_writes = stalloc_typed(vkon.max_replay_frames, VkWriteDescriptorSet);
        for(int i = 0; i < vkon.max_replay_frames; i++) {
            image_infos[i] = (VkDescriptorImageInfo) {
                .sampler = vkon.texture_sampler,
                .imageView = vkon.replay_image_views[i],
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            descriptor_writes[i] = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.replay_descriptor_set,
                    .dstBinding = 0,
                    .dstArrayElement = i,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = image_infos+i,
            };
        }

        vkUpdateDescriptorSets(vkon.device, vkon.max_replay_frames, descriptor_writes, 0, 0);
        stunalloc(descriptor_writes);
        stunalloc(image_infos);

        VkAccessFlags src_access_mask = 0;
        VkAccessFlags dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        VkImageLayout old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkPipelineStageFlags src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkCommandBuffer command_buffer = begin_single_time_command_buffer();
        for(int i = 0; i < vkon.max_replay_frames; i++)
        { //transition image layout
            VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = src_access_mask,
                .dstAccessMask = dst_access_mask,
                .oldLayout = old_layout,
                .newLayout = new_layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vkon.replay_images[i],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };
            vkCmdPipelineBarrier(command_buffer,
                                 src_stage,
                                 dst_stage,
                                 0,
                                 0, 0,
                                 0, 0,
                                 1, &barrier);
        }
        end_single_time_command_buffer();
    }

    create_draw_replay_pipeline(draw_replay_pipeline);
}

void cleanup_replay_framebuffers()
{
    if(vkon.max_replay_frames == 0) return;

    cleanup_pipeline(*draw_replay_pipeline);

    for(int i = 0; i < vkon.max_replay_frames; i++) {
        vkDestroyImageView(vkon.device, vkon.replay_image_views[i], 0);
        vkDestroyImage(vkon.device, vkon.replay_images[i], 0);
        vkDestroyFramebuffer(vkon.device, vkon.replay_framebuffers[i], 0);
    }
    vkFreeMemory(vkon.device, vkon.replay_image_memory, 0);

    dynamic_unalloc(vkon.replay_image_views);
    dynamic_unalloc(vkon.replay_images);
    dynamic_unalloc(vkon.replay_framebuffers);

    vkDestroyDescriptorPool(vkon.device, vkon.replay_descriptor_pool, 0);
}

bool create_swapchain()
{
    int error = 0;

    swapchain_support_details swapchain_support = query_swapchain_support(vkon.physical_device);
    if(swapchain_support.extent.width == 0 || swapchain_support.extent.height == 0) return false;
    vkon.swapchain_extent = swapchain_support.extent;
    vkon.aspect_ratio = ((real) vkon.swapchain_extent.width)/vkon.swapchain_extent.height;

    uint32 n_swap_images = 0;
    n_swap_images = swapchain_support.capabilities.minImageCount+1;
    if(swapchain_support.capabilities.maxImageCount > 0 && n_swap_images > swapchain_support.capabilities.maxImageCount)
        n_swap_images = swapchain_support.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_create = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vkon.surface,
        .minImageCount = n_swap_images,
        .imageFormat = swapchain_support.format.format,
        .imageColorSpace = swapchain_support.format.colorSpace,
        .imageExtent = vkon.swapchain_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = swapchain_support.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = swapchain_support.present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    uint32 queue_family_indices[] = {vkon.graphics_family_index, vkon.present_family_index};
    if(queue_family_indices[1] != queue_family_indices[0]) {
        swapchain_create.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create.queueFamilyIndexCount = 2;
        swapchain_create.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    vkon.swapchain_format = swapchain_support.format.format;

    error = vkCreateSwapchainKHR(vkon.device, &swapchain_create, 0, &vkon.swapchain);
    vk_assert("could not create swap chain");

    //swapchain images
    vkGetSwapchainImagesKHR(vkon.device, vkon.swapchain, &vkon.n_swapchain_images, 0);
    vkon.swapchain_images = (VkImage*) dynamic_alloc(vkon.n_swapchain_images*sizeof(VkImage));
    vkGetSwapchainImagesKHR(vkon.device, vkon.swapchain, &vkon.n_swapchain_images, vkon.swapchain_images);

    vkon.swapchain_image_views = (VkImageView*) dynamic_alloc(vkon.n_swapchain_images*sizeof(VkImageView));
    for(int i = 0; i < vkon.n_swapchain_images; i++) {
        vkon.swapchain_image_views[i] = create_image_view(vkon.swapchain_images[i], vkon.swapchain_format);
    }
    return true;
}

bool has_stencil_component(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void create_swapchain_framebuffers()
{
    int error = 0;

    vkon.swapchain_framebuffers = dynamic_alloc_typed(vkon.n_swapchain_images, VkFramebuffer);
    for(int i = 0; i < vkon.n_swapchain_images; i++) {
        VkImageView attachments[] = {
            vkon.swapchain_image_views[i],
        };

        VkFramebufferCreateInfo framebuffer_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vkon.swapchain_render_pass,
            .attachmentCount = len(attachments),
            .pAttachments = attachments,
            .width = vkon.swapchain_extent.width,
            .height = vkon.swapchain_extent.height,
            .layers = 1,
        };

        error = vkCreateFramebuffer(vkon.device, &framebuffer_info, 0, &vkon.swapchain_framebuffers[i]);
        vk_assert("could not create framebuffer ", i, " for swap chain");
    }

    vkon.framebuffer_resized = false;
    vkon.swapchain_valid = true;
}

void cleanup_swapchain()
{
    vkon.swapchain_valid = false;
    for(int i = 0; i < vkon.n_swapchain_images; i++) {
        vkDestroyFramebuffer(vkon.device, vkon.swapchain_framebuffers[i], 0);
    }

    for(int i = 0; i < vkon.n_swapchain_images; i++) {
        vkDestroyImageView(vkon.device, vkon.swapchain_image_views[i], 0);
    }

    vkDestroySwapchainKHR(vkon.device, vkon.swapchain, 0);

    dynamic_unalloc(vkon.swapchain_framebuffers);
    dynamic_unalloc(vkon.swapchain_images);
    dynamic_unalloc(vkon.swapchain_image_views);
}

void recreate_swapchain()
{
    int error = 0;
    error = vkDeviceWaitIdle(vkon.device);
    vk_assert("could not wait for device to be idle");

    if(vkon.swapchain_valid) cleanup_swapchain();
    error = create_swapchain();
    if(error) create_swapchain_framebuffers();
}

void init_vulkan(HWND hwnd, HINSTANCE hinstance)
{
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Game",
        .applicationVersion = 0,
        .pEngineName = "No Engine",
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_1,
    };

    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
#ifdef DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };

    VkInstanceCreateInfo instance_create = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .enabledExtensionCount = len(extensions),
        .ppEnabledExtensionNames = extensions,
    };

#ifdef DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_create = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
        .messageType = (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
        .pfnUserCallback = vk_debug_callback,
    };

    const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
    { //check support for validation layers
        uint32 n_layers;
        vkEnumerateInstanceLayerProperties(&n_layers, 0);
        VkLayerProperties* available_layers = stalloc_typed(n_layers, VkLayerProperties);
        vkEnumerateInstanceLayerProperties(&n_layers, available_layers);

        for(int i = 0; i < len(validation_layers); i++) {
            for(int j = 0;; j++) {
                if(j >= n_layers) {
                    log_error("validation layer not available: ", validation_layers[i], "\n");
                    break;
                }
                if(strcmp(validation_layers[i], available_layers[j].layerName) == 0) {
                    break;
                }
            }
        }
        stunalloc(available_layers);

        instance_create.enabledLayerCount = len(validation_layers);
        instance_create.ppEnabledLayerNames = validation_layers;
        instance_create.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create;
    }
#endif

    uint32 n_extensions = 0;
    vkEnumerateInstanceExtensionProperties(0, &n_extensions, 0);
    VkExtensionProperties* available_extensions = stalloc_typed(n_extensions, VkExtensionProperties);
    vkEnumerateInstanceExtensionProperties(0, &n_extensions, available_extensions);
    log_output("available vulkan extensions:\n");
    for(int i = 0; i < n_extensions; i++) {
        log_output("\t", available_extensions[i].extensionName, "\n");
    }
    stunalloc(available_extensions);

    log_output("\n");

    int error;
    error = vkCreateInstance(&instance_create, 0, &vkon.instance);
    vk_assert(error);

    load_vk_functions();

#ifdef DEBUG
    error = _vkCreateDebugUtilsMessengerEXT(vkon.instance, &debug_create, 0, &vkon.debug_messenger);
    vk_assert("could not set up debug messenger");
#endif

    VkWin32SurfaceCreateInfoKHR surface_create = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = hinstance,
        .hwnd = hwnd,
    };

    error = vkCreateWin32SurfaceKHR(vkon.instance, &surface_create, 0, &vkon.surface);
    vk_assert("could not create window surface");

    uint32 n_devices = 0;
    vkEnumeratePhysicalDevices(vkon.instance, &n_devices, 0);
    assert(n_devices > 0, "could not find any GPU with vulkan support");

    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkPhysicalDevice* devices = stalloc_typed(n_devices, VkPhysicalDevice);
    vkEnumeratePhysicalDevices(vkon.instance, &n_devices, devices);
    int best_score = -1;
    int best_index = -1;
    vkon.graphics_family_index = 0;
    vkon.present_family_index = 0;
    VkQueueFamilyProperties graphics_family;
    log_output("available devices:\n");
    for(int i = 0; i < n_devices; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(devices[i], &features);
        int score = 100*(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        score += 10*features.samplerAnisotropy;
        log_output("\t", i, ": ", properties.deviceName, "\n");

        uint32 graphics_family_indices;
        bool has_graphics_family = false;
        uint32 present_family_indices;
        bool has_present_family = false;

        uint32 n_queue_families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &n_queue_families, 0);
        VkQueueFamilyProperties* queue_families = stalloc_typed(n_queue_families, VkQueueFamilyProperties);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &n_queue_families, queue_families);
        for(int j = 0; j < n_queue_families; j++) {
            if(queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                graphics_family_indices = j;
                has_graphics_family = true;
            }

            VkBool32 present_supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, vkon.surface, &present_supported);
            if(present_supported) {
                present_family_indices = j;
                has_present_family = true;
            }
        }
        stunalloc(queue_families);

        uint32 n_extensions;
        vkEnumerateDeviceExtensionProperties(devices[i], 0, &n_extensions, 0);
        VkExtensionProperties* available_extensions = stalloc_typed(n_extensions, VkExtensionProperties);
        vkEnumerateDeviceExtensionProperties(devices[i], 0, &n_extensions, available_extensions);
        bool extensions_found = true;

        for(int j = 0; j < len(device_extensions) && extensions_found; j++) {
            for(int k = 0;; k++) {
                if(k >= n_extensions) {
                    extensions_found = false;
                    break;
                }
                if(strcmp(device_extensions[j], available_extensions[k].extensionName) == 0) break;
            }
        }
        stunalloc(available_extensions);

        if(extensions_found)
        {
            swapchain_support_details swapchain_support = query_swapchain_support(devices[i]);

            score += swapchain_support.format_score+swapchain_support.present_score;
            if(score > best_score
               && has_graphics_family
               && has_present_family
               && swapchain_support.format_score >= 0
               && swapchain_support.present_score >= 0)
            {
                vkon.physical_device = devices[i];
                best_score = score;
                best_index = i;
                vkon.graphics_family_index = graphics_family_indices;
                vkon.present_family_index = present_family_indices;
            }
        }
    }
    stunalloc(devices);
    assert(vkon.physical_device, "could not find suitable GPU");
    log_output("using device ", best_index, "\n");

    float queue_priority = 1.0f;

    int n_queue_create_infos = 0;
    VkDeviceQueueCreateInfo queue_create_infos[2] = {};
    queue_create_infos[n_queue_create_infos].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[n_queue_create_infos].queueFamilyIndex = vkon.graphics_family_index;
    queue_create_infos[n_queue_create_infos].queueCount = 1;
    queue_create_infos[n_queue_create_infos].pQueuePriorities = &queue_priority;
    n_queue_create_infos++;

    if(vkon.graphics_family_index != vkon.present_family_index) {
        queue_create_infos[n_queue_create_infos].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[n_queue_create_infos].queueFamilyIndex = vkon.present_family_index;
        queue_create_infos[n_queue_create_infos].queueCount = 1;
        queue_create_infos[n_queue_create_infos].pQueuePriorities = &queue_priority;
        n_queue_create_infos++;
    }

    VkPhysicalDeviceFeatures device_features = {
        .depthClamp = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };

    VkDeviceCreateInfo device_create = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = n_queue_create_infos,
        .pQueueCreateInfos = queue_create_infos,
#ifdef DEBUG
        .enabledLayerCount = len(validation_layers),
        .ppEnabledLayerNames = validation_layers,
#else
        .enabledLayerCount = 0,
#endif
        .enabledExtensionCount = len(device_extensions),
        .ppEnabledExtensionNames = device_extensions,
        .pEnabledFeatures = &device_features,
    };

    error = vkCreateDevice(vkon.physical_device, &device_create, 0, &vkon.device);
    vk_assert("could not create logical device");

    vkGetDeviceQueue(vkon.device, vkon.graphics_family_index, 0, &vkon.graphics_queue);
    vkGetDeviceQueue(vkon.device, vkon.present_family_index, 0, &vkon.present_queue);

    { //create sampler
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(vkon.physical_device, &properties);

        VkSamplerCreateInfo sampler_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 1.0f,//min(properties.limits.maxSamplerAnisotropy, 16.0f),
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .unnormalizedCoordinates = VK_FALSE,
        };

        error = vkCreateSampler(vkon.device, &sampler_info, 0, &vkon.texture_sampler);
        vk_assert("could not create sampler");
    }

    create_swapchain();

    { //create render pass
        VkAttachmentDescription color_attachment = {
            .format = COLOR_FORMAT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentReference color_attachment_ref = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentDescription depth_attachment = {
            .format = DEPTH_FORMAT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentReference depth_attachment_ref = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_ref,
            .pDepthStencilAttachment = &depth_attachment_ref,
        };

        VkSubpassDependency dependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };

        VkAttachmentDescription attachments[] = {color_attachment, depth_attachment};
        VkRenderPassCreateInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = len(attachments),
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };

        vkCreateRenderPass(vkon.device, &render_pass_info, 0, &vkon.render_pass);
    }

    { //create swapchain render pass
        VkAttachmentDescription color_attachment = {
            .format = vkon.swapchain_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference color_attachment_ref = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_ref,
        };

        VkSubpassDependency dependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };

        VkAttachmentDescription attachments[] = {color_attachment};
        VkRenderPassCreateInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = len(attachments),
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };

        vkCreateRenderPass(vkon.device, &render_pass_info, 0, &vkon.swapchain_render_pass);
    }

    { //create replay render pass
        VkAttachmentDescription color_attachment = {
            .format = COLOR_FORMAT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference color_attachment_ref = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_ref,
        };

        VkSubpassDependency dependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };

        VkAttachmentDescription attachments[] = {color_attachment};
        VkRenderPassCreateInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = len(attachments),
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };

        vkCreateRenderPass(vkon.device, &render_pass_info, 0, &vkon.replay_render_pass);
    }

    create_render_framebuffer();
    create_swapchain_framebuffers();

    //command buffer
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vkon.graphics_family_index,
    };
    error = vkCreateCommandPool(vkon.device, &pool_info, 0, &vkon.command_pool);
    vk_assert("could not create command pool");

    VkCommandPoolCreateInfo single_use_pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = vkon.graphics_family_index,
    };
    error = vkCreateCommandPool(vkon.device, &single_use_pool_info, 0, &vkon.single_time_command_pool);
    vk_assert("could not create single_use command pool");

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vkon.command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = len(vkon.command_buffers),
    };
    error = vkAllocateCommandBuffers(vkon.device, &alloc_info, vkon.command_buffers);
    vk_assert("could not allocate command buffer");

    VkCommandBufferAllocateInfo single_time_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vkon.single_time_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    error = vkAllocateCommandBuffers(vkon.device, &alloc_info, &vkon.single_time_command_buffer);
    vk_assert("could not allocate command buffer");

    //descriptor pool
    VkDescriptorPoolSize pool_sizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 128,
        }, {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 128,
        }, {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 128,
        }
    };

    VkDescriptorPoolCreateInfo desc_pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 128,
        .poolSizeCount = len(pool_sizes),
        .pPoolSizes = pool_sizes,
    };

    error = vkCreateDescriptorPool(vkon.device, &desc_pool_info, 0, &vkon.descriptor_pool);
    vk_assert("could not create descriptor pool");

    //global ssbo
    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        VkDeviceSize buffer_size = sizeof(global_ssbo);
        create_vk_buffer(buffer_size,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &vkon.shader_storage_buffers[i],
                         &vkon.shader_storage_buffers_memory[i]);

        vkMapMemory(vkon.device, vkon.shader_storage_buffers_memory[i], 0, buffer_size, 0, &vkon.shader_storage_buffers_mapped[i]);

        // VkBufferMemoryBarrier barrier = {
        //     .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        //     .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
        //     .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
        //     .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        //     .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        //     .buffer = vkon.shader_storage_buffers[i],
        //     .offset = 0,
        //     .size = VK_WHOLE_SIZE,
        // };
        // VkCommandBuffer command_buffer = begin_single_time_command_buffer();
        // vkCmdPipelineBarrier(command_buffer,
        //                      VK_PIPELINE_STAGE_2_HOST_BIT,
        //                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        //                      0,
        //                      0, 0,
        //                      1, &barrier,
        //                      0, 0);
        // end_single_time_command_buffer();
    }

    { //global descriptor sets
        //descriptor set layout
        VkDescriptorSetLayoutBinding layout_bindings[] = {
            {    //uniform buffer
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            }, { //font atlas
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }, { //primitive list
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags =  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
            }, { //lightmap color
                .binding = 3,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }, { //lightmap depth
                .binding = 4,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }
        };

        VkDescriptorSetLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = len(layout_bindings),
            .pBindings = layout_bindings,
        };

        error = vkCreateDescriptorSetLayout(vkon.device, &layout_info, 0, &vkon.global_descriptor_set_layout);
        vk_assert("could not create descriptor set layout");

        VkDescriptorSetLayout desc_layouts[MAX_QUEUED_FRAMES];
        for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
            desc_layouts[i] = vkon.global_descriptor_set_layout;
        }

        VkDescriptorSetAllocateInfo desc_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkon.descriptor_pool,
            .descriptorSetCount = len(desc_layouts),
            .pSetLayouts = desc_layouts,
        };

        error = vkAllocateDescriptorSets(vkon.device, &desc_alloc_info, vkon.global_descriptor_sets);
        vk_assert("could not allocate descriptor sets");

        vkon.default_font = load_font("data/arial.ttf", 24);

        create_lightmap_descriptor_sets();

        for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
            VkDeviceSize buffer_size = sizeof(global_uniform_buffer);
            create_vk_buffer(buffer_size,
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &vkon.uniform_buffers[i],
                             &vkon.uniform_buffers_memory[i]);

            vkMapMemory(vkon.device, vkon.uniform_buffers_memory[i], 0, buffer_size, 0, &vkon.uniform_buffers_mapped[i]);

            VkDescriptorBufferInfo buffer_info = {
                .buffer = vkon.uniform_buffers[i],
                .offset = 0,
                .range = sizeof(global_uniform_buffer),
            };

            VkDescriptorImageInfo image_info = {
                .sampler = vkon.texture_sampler,
                .imageView = vkon.default_font.texture.image_view,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            VkDescriptorBufferInfo ssbo_info = {
                .buffer = vkon.shader_storage_buffers[i],
                .offset = 0,
                .range = sizeof(global_ssbo),
            };

            VkDescriptorImageInfo lightmap_color_image_info = {
                .sampler = vkon.texture_sampler,
                .imageView = lightmap_color_image_views[i],
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            VkDescriptorImageInfo lightmap_depth_image_info = {
                .sampler = vkon.texture_sampler,
                .imageView = lightmap_depth_image_views[i],
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            VkWriteDescriptorSet descriptor_writes[] = {
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.global_descriptor_sets[i],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pBufferInfo = &buffer_info,
                }, {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.global_descriptor_sets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &image_info,
                }, {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.global_descriptor_sets[i],
                    .dstBinding = 2,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .pBufferInfo = &ssbo_info,
                }, {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.global_descriptor_sets[i],
                    .dstBinding = 3,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &lightmap_color_image_info,
                }, {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkon.global_descriptor_sets[i],
                    .dstBinding = 4,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &lightmap_depth_image_info,
                }
            };

            vkUpdateDescriptorSets(vkon.device, len(descriptor_writes), descriptor_writes, 0, 0);
        }
    }

    { //postprocess descriptor sets
        //descriptor set layout
        VkDescriptorSetLayoutBinding layout_bindings[] = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }
        };

        VkDescriptorSetLayoutCreateInfo layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = len(layout_bindings),
            .pBindings = layout_bindings,
        };

        error = vkCreateDescriptorSetLayout(vkon.device, &layout_info, 0, &vkon.postprocess_descriptor_set_layout);
        vk_assert("could not create postprocess descriptor set layout");

        VkDescriptorSetAllocateInfo desc_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkon.descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &vkon.postprocess_descriptor_set_layout,
        };

        error = vkAllocateDescriptorSets(vkon.device, &desc_alloc_info, &vkon.postprocess_descriptor_set);
        vk_assert("could not allocate postprocess descriptor sets");

        VkDescriptorImageInfo image_info = {
            .sampler = vkon.texture_sampler,
            .imageView = vkon.color_image_view,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        VkWriteDescriptorSet descriptor_writes[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = vkon.postprocess_descriptor_set,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &image_info,
            },
        };

        vkUpdateDescriptorSets(vkon.device, len(descriptor_writes), descriptor_writes, 0, 0);
    }

    create_replay_framebuffers();

    //vertex buffer
    create_vk_buffer(8*sizeof(test_vertex)+12*sizeof(uint16),
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &vkon.staging_buffer,
                     &vkon.staging_buffer_memory);

    create_vk_buffer(8*sizeof(test_vertex),
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     &vkon.vertex_buffer,
                     &vkon.vertex_buffer_memory);

    create_vk_buffer(12*sizeof(uint16),
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     &vkon.index_buffer,
                     &vkon.index_buffer_memory);

    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        create_vk_buffer(1024*1024,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &vkon.dynamic_vertex_buffers[i],
                         &vkon.dynamic_vertex_buffers_memory[i]);
        vkMapMemory(vkon.device, vkon.dynamic_vertex_buffers_memory[i], 0, 1024*1024, 0, &vkon.dynamic_vertex_buffers_mapped[i]);
    }

    //signaling variables
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        error = vkCreateSemaphore(vkon.device, &semaphore_info, 0, &vkon.image_available_semaphores[i]);
        vk_assert("could not create semaphore");
        error = vkCreateSemaphore(vkon.device, &semaphore_info, 0, &vkon.render_finished_semaphores[i]);
        vk_assert("could not create semaphore");

        error = vkCreateFence(vkon.device, &fence_info, 0, &vkon.in_flight_fences[i]);
        vk_assert("could not create fence");
    }

    //texture
    vkon.test_texture = load_image_to_texture("data/altar_foreground.png");

    create_pipelines();
}

void cleanup_vulkan()
{
    vkDeviceWaitIdle(vkon.device);

    vkDestroyDescriptorPool(vkon.device, vkon.descriptor_pool, 0);
    vkDestroyDescriptorSetLayout(vkon.device, vkon.global_descriptor_set_layout, 0);
    vkDestroyDescriptorSetLayout(vkon.device, vkon.postprocess_descriptor_set_layout, 0);
    vkDestroyDescriptorSetLayout(vkon.device, vkon.replay_descriptor_set_layout, 0);

    vkDestroyBuffer(vkon.device, vkon.index_buffer, 0);
    vkFreeMemory(vkon.device, vkon.index_buffer_memory, 0);
    vkDestroyBuffer(vkon.device, vkon.vertex_buffer, 0);
    vkFreeMemory(vkon.device, vkon.vertex_buffer_memory, 0);

    vkDestroyBuffer(vkon.device, vkon.staging_buffer, 0);
    vkFreeMemory(vkon.device, vkon.staging_buffer_memory, 0);

    vkDestroySampler(vkon.device, vkon.texture_sampler, 0);
    cleanup_texture(vkon.test_texture);
    cleanup_texture(vkon.default_font.texture);

    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        vkDestroySemaphore(vkon.device, vkon.image_available_semaphores[i], 0);
        vkDestroySemaphore(vkon.device, vkon.render_finished_semaphores[i], 0);
        vkDestroyFence(vkon.device, vkon.in_flight_fences[i], 0);

        vkDestroyBuffer(vkon.device, vkon.uniform_buffers[i], 0);
        vkFreeMemory(vkon.device, vkon.uniform_buffers_memory[i], 0);

        vkDestroyBuffer(vkon.device, vkon.dynamic_vertex_buffers[i], 0);
        vkFreeMemory(vkon.device, vkon.dynamic_vertex_buffers_memory[i], 0);

        vkDestroyBuffer(vkon.device, vkon.shader_storage_buffers[i], 0);
        vkFreeMemory(vkon.device, vkon.shader_storage_buffers_memory[i], 0);
    }

    vkDestroyCommandPool(vkon.device, vkon.command_pool, 0);
    vkDestroyCommandPool(vkon.device, vkon.single_time_command_pool, 0);

    cleanup_replay_framebuffers();
    cleanup_swapchain();
    cleanup_render_framebuffer();

    cleanup_lightmap_data();

    cleanup_pipelines();

    vkDestroyRenderPass(vkon.device, vkon.replay_render_pass, 0);
    vkDestroyRenderPass(vkon.device, vkon.swapchain_render_pass, 0);
    vkDestroyRenderPass(vkon.device, vkon.render_pass, 0);

    vkDestroyDevice(vkon.device, 0);
    vkDestroySurfaceKHR(vkon.instance, vkon.surface, 0);
#ifdef DEBUG
    _vkDestroyDebugUtilsMessengerEXT(vkon.instance, vkon.debug_messenger, 0);
#endif
    vkDestroyInstance(vkon.instance, 0);
}

struct user_input;
void render_game(user_input* input);

void draw_frame(user_input* input)
{
    int error = 0;

    uint32 f = vkon.current_frame;
    vkWaitForFences(vkon.device, 1, &vkon.in_flight_fences[f], VK_TRUE, UINT64_MAX);

    vkon.dynamic_vertex_buffers_used[f] = 0;

    if(!vkon.swapchain_valid) {
        recreate_swapchain();
        return;
    }

    uint32 image_index;
    error = vkAcquireNextImageKHR(vkon.device, vkon.swapchain, UINT64_MAX, vkon.image_available_semaphores[f], VK_NULL_HANDLE, &image_index);
    if(error == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return;
    } else vk_assert("could not acquire swap chain image");

    vkResetFences(vkon.device, 1, &vkon.in_flight_fences[f]);

    vkResetCommandBuffer(vkon.command_buffers[f], 0);

    { //record command buffer
        VkCommandBufferBeginInfo begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = 0,
        };

        error = vkBeginCommandBuffer(vkon.command_buffers[f], &begin_info);
        vk_assert("could not begin recording command buffer");

        if(vkon.replay_mode != vkon.old_replay_mode)
        {
            VkAccessFlags src_access_mask = 0;
            VkAccessFlags dst_access_mask = 0;
            VkImageLayout old_layout = {};
            VkImageLayout new_layout = {};
            VkPipelineStageFlags src_stage = 0;
            VkPipelineStageFlags dst_stage = 0;
            switch(vkon.old_replay_mode) {
                case 0: {
                    src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
                    old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } break;
                case 1: {
                    src_access_mask = VK_ACCESS_SHADER_READ_BIT;
                    dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    old_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } break;
            }

            for(int i = 0; i < vkon.max_replay_frames; i++)
            { //transition image layout
                VkCommandBuffer command_buffer = vkon.command_buffers[f];
                VkImageMemoryBarrier barrier = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .srcAccessMask = src_access_mask,
                    .dstAccessMask = dst_access_mask,
                    .oldLayout = old_layout,
                    .newLayout = new_layout,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = vkon.replay_images[i],
                    .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                    },
                };

                vkCmdPipelineBarrier(command_buffer,
                                     src_stage,
                                     dst_stage,
                                     0,
                                     0, 0,
                                     0, 0,
                                     1, &barrier);
            }
            vkon.old_replay_mode = vkon.replay_mode;
        }

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkon.command_buffers[f], 0, 1, &vkon.dynamic_vertex_buffers[f], offsets);

        {
            global_uniform_buffer uniforms = {
                .t = vkon.camera,
                .camera_axes = {pad_4(vkon.camera_axes[0]), pad_4(vkon.camera_axes[1]), pad_4(vkon.camera_axes[2])},
                .camera_pos = vkon.camera_pos,
                .fov = vkon.fov,
                .aspect_ratio = vkon.aspect_ratio,
                .time = vkon.time,
            };
            memcpy(vkon.uniform_buffers_mapped[f], &uniforms, sizeof(uniforms));
        }

        update_lightmap();

        VkClearValue clear_values[] = {
            {.color = {0.025f, 0.0f, 0.035f, 1.0f}},
            {.depthStencil = {1.0f, 0}},
        };
        VkRenderPassBeginInfo render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = vkon.render_pass,
            .framebuffer = vkon.render_framebuffer,
            .renderArea = {
                .offset = {0, 0},
                .extent = {vkon.render_resolution.x, vkon.render_resolution.y},
            },
            .clearValueCount = len(clear_values),
            .pClearValues = clear_values,
        };
        vkCmdBeginRenderPass(vkon.command_buffers[f], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float) vkon.render_resolution.x,
            .height = (float) vkon.render_resolution.y,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport(vkon.command_buffers[f], 0, 1, &viewport);

        VkRect2D scissor = {
            .offset = {0, 0},
            .extent = {vkon.render_resolution.x, vkon.render_resolution.y},
        };
        vkCmdSetScissor(vkon.command_buffers[f], 0, 1, &scissor);

        vkCmdBindDescriptorSets(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, vkon.pipeline_layout, 0, 1, &vkon.global_descriptor_sets[f], 0, 0);

        render_game(input);

        vkCmdEndRenderPass(vkon.command_buffers[f]);

        { //transition image layout for postprocess
            VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vkon.color_image,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };

            vkCmdPipelineBarrier(vkon.command_buffers[f],
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0, 0,
                                 0, 0,
                                 1, &barrier);
        }

        //swapchain renderpass
        VkRenderPassBeginInfo swapchain_render_pass_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = vkon.swapchain_render_pass,
            .framebuffer = vkon.swapchain_framebuffers[image_index],
            .renderArea = {
                .offset = {0, 0},
                .extent = vkon.swapchain_extent,
            },
            .clearValueCount = len(clear_values),
            .pClearValues = clear_values,
        };

        vkCmdBeginRenderPass(vkon.command_buffers[f], &swapchain_render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        {
            VkViewport viewport = {
                .x = 0.0f,
                .y = 0.0f,
                .width = (float) vkon.swapchain_extent.width,
                .height = (float) vkon.swapchain_extent.height,
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            };
            vkCmdSetViewport(vkon.command_buffers[f], 0, 1, &viewport);

            VkRect2D scissor = {
                .offset = {0, 0},
                .extent = vkon.swapchain_extent,
            };
            vkCmdSetScissor(vkon.command_buffers[f], 0, 1, &scissor);
        }

        vkCmdBindDescriptorSets(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, fullscreen_image_pipeline->pipeline_layout, 0, 1, &vkon.postprocess_descriptor_set, 0, 0);

        vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, fullscreen_image_pipeline->pipeline);
        vkCmdDraw(vkon.command_buffers[f], 4, 1, 0, 0);
        vkCmdEndRenderPass(vkon.command_buffers[f]);

        if(vkon.replay_mode==0 && vkon.max_replay_frames > 0 && vkon.do_gif_frame) { //replay renderpass
            vkon.do_gif_frame = 0;
            VkRenderPassBeginInfo replay_render_pass_info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = vkon.replay_render_pass,
                .framebuffer = vkon.replay_framebuffers[vkon.replay_index],
                .renderArea = {
                    .offset = {0, 0},
                    .extent = {vkon.replay_resolution.x, vkon.replay_resolution.y},
                },
                .clearValueCount = len(clear_values),
                .pClearValues = clear_values,
            };

            vkon.n_replay_frames = min(vkon.n_replay_frames+1, vkon.max_replay_frames);
            vkon.replay_index = (vkon.replay_index+1)%vkon.max_replay_frames;

            vkCmdBeginRenderPass(vkon.command_buffers[f], &replay_render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

            {
                VkViewport viewport = {
                    .x = 0.0f,
                    .y = 0.0f,
                    .width  = (float) vkon.replay_resolution.x,
                    .height = (float) vkon.replay_resolution.y,
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
                };
                vkCmdSetViewport(vkon.command_buffers[f], 0, 1, &viewport);

                VkRect2D scissor = {
                    .offset = {0, 0},
                    .extent = {vkon.replay_resolution.x, vkon.replay_resolution.y},
                };
                vkCmdSetScissor(vkon.command_buffers[f], 0, 1, &scissor);
            }

            vkCmdBindDescriptorSets(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, replay_pipeline->pipeline_layout, 0, 1, &vkon.postprocess_descriptor_set, 0, 0);

            vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_GRAPHICS, replay_pipeline->pipeline);
            vkCmdDraw(vkon.command_buffers[f], 4, 1, 0, 0);
            vkCmdEndRenderPass(vkon.command_buffers[f]);
        }

        { //transition image layout
            VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vkon.color_image,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };

            vkCmdPipelineBarrier(vkon.command_buffers[f],
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 0,
                                 0, 0,
                                 0, 0,
                                 1, &barrier);
        }

        error = vkEndCommandBuffer(vkon.command_buffers[f]);
        vk_assert("could not record command buffer");
    }

    VkSemaphore wait_semaphores[] = {vkon.image_available_semaphores[f]};
    VkPipelineStageFlags wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signal_semaphores[] = {vkon.render_finished_semaphores[f]};

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = len(wait_semaphores),
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkon.command_buffers[f],
        .signalSemaphoreCount = len(signal_semaphores),
        .pSignalSemaphores = signal_semaphores,
    };

    error = vkQueueSubmit(vkon.graphics_queue, 1, &submit_info, vkon.in_flight_fences[f]);
    vk_assert("could not submit draw command buffer");

    VkSwapchainKHR swapchains[] = {vkon.swapchain};

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = len(signal_semaphores),
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = len(swapchains),
        .pSwapchains = swapchains,
        .pImageIndices = &image_index,
        .pResults = 0,
    };

    error = vkQueuePresentKHR(vkon.present_queue, &present_info);
    if(error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR || vkon.framebuffer_resized) {
        recreate_swapchain();
        return;
    } else vk_assert("could not acquire swap chain image");

    vkon.current_frame = (vkon.current_frame+1) % MAX_QUEUED_FRAMES;
}

void init_render_context(render_context* rc, int_2 resolution)
{
    rc->resolution = resolution;

}

void use_render_context(render_context* rc)
{

}

void update_camera_matrix()
{
    real screen_dist = 1.0/tan(0.5*vkon.fov);

    real_4x4 translate = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -vkon.camera_pos.x, -vkon.camera_pos.y, -vkon.camera_pos.z, 1,
    };

    real_4x4 rotate = {
        vkon.camera_axes[0][0], vkon.camera_axes[1][0], vkon.camera_axes[2][0], 0,
        vkon.camera_axes[0][1], vkon.camera_axes[1][1], vkon.camera_axes[2][1], 0,
        vkon.camera_axes[0][2], vkon.camera_axes[1][2], vkon.camera_axes[2][2], 0,
        0, 0, 0, 1,
    };

    real n = 0.1;
    real f = 1000.0;

    real_4x4 perspective = {
        (screen_dist/vkon.aspect_ratio), 0, 0, 0,
        0, screen_dist,  0, 0,
        0, 0,      f/(f-n), 1,
        0, 0, -(f*n)/(f-n), 0,
    };

    vkon.camera = perspective*rotate*translate;
}

////////////////////////////////////////////////////

#define font_resolution 1024

void draw_to_screen()
{
}

void draw_bitmap(real_3 x, real scale, real theta, bitmap_t bitmap)
{
}

void draw_line(real_3 x, real_2 t, real r, real_4 color)
{
}

void draw_arc(real_3 x, real_2 d0, real_2 d1, real R, real r, real_4 color)
{
}

void draw_rounded_rectangle(real_3 x, real_2 r, real radius, real_4 color)
{
}

void draw_triangle(real_3 x0, real_3 x1, real_3 x2, real_4 color)
{
}

void draw_triangles(real_3* vertices, real_4* colors, int n_triangles, real_4x4 camera)
{
}

void draw_color_picker(real_3 x, real r, real_3 hsv)
{
}

void draw_background(render_context* rc)
{
}

#endif //GRAPHICS
