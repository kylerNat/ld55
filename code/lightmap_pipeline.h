#include "../shaders/include/lightmap_constants.h"

VkDescriptorSetLayout lightmap_descriptor_set_layout;
VkDescriptorSet lightmap_descriptor_sets[MAX_QUEUED_FRAMES];

VkImage lightmap_color_images[MAX_QUEUED_FRAMES];
VkImageView lightmap_color_image_views[MAX_QUEUED_FRAMES];
VkDeviceMemory lightmap_color_images_memory[MAX_QUEUED_FRAMES];
VkImage lightmap_depth_images[MAX_QUEUED_FRAMES];
VkDeviceMemory lightmap_depth_images_memory[MAX_QUEUED_FRAMES];
VkImageView lightmap_depth_image_views[MAX_QUEUED_FRAMES];

VkBuffer lightmap_cast_buffers[MAX_QUEUED_FRAMES];
VkDeviceMemory lightmap_cast_buffers_memory[MAX_QUEUED_FRAMES];

#define LIGHTPROBE_COLOR_FORMAT VK_FORMAT_B10G11R11_UFLOAT_PACK32
#define LIGHTPROBE_DEPTH_FORMAT VK_FORMAT_R16G16_SFLOAT

// struct lightmap_push_constants
// {
//     int frame_number;
//     float time;
// };

void create_lightmap_descriptor_sets()
{
    int error = 0;

    //create images
    VkCommandBuffer command_buffer = begin_single_time_command_buffer();
    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        create_image(LIGHTMAP_COLOR_TEXTURE_RESOLUTION, LIGHTMAP_COLOR_TEXTURE_RESOLUTION,
                     LIGHTPROBE_COLOR_FORMAT,
                     VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     &lightmap_color_images[i],
                     &lightmap_color_images_memory[i]);
        lightmap_color_image_views[i] = create_image_view(lightmap_color_images[i], LIGHTPROBE_COLOR_FORMAT);

        create_image(LIGHTMAP_DEPTH_TEXTURE_RESOLUTION, LIGHTMAP_DEPTH_TEXTURE_RESOLUTION,
                     LIGHTPROBE_DEPTH_FORMAT,
                     VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     &lightmap_depth_images[i],
                     &lightmap_depth_images_memory[i]);
        lightmap_depth_image_views[i] = create_image_view(lightmap_depth_images[i], LIGHTPROBE_DEPTH_FORMAT);

        VkImageMemoryBarrier barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = lightmap_color_images[i],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
        };
        vkCmdPipelineBarrier(command_buffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                             0,
                             0, 0,
                             0, 0,
                             1, &barrier);
        barrier.image = lightmap_depth_images[i];
        vkCmdPipelineBarrier(command_buffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                             0,
                             0, 0,
                             0, 0,
                             1, &barrier);
    }
    end_single_time_command_buffer();

    //raycast buffer
    VkDeviceSize lightmap_cast_buffer_size = 4*sizeof(float)*N_LIGHTPROBES*LIGHTPROBE_CASTS_PER_FRAME;
    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        create_vk_buffer(lightmap_cast_buffer_size,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         &lightmap_cast_buffers[i],
                         &lightmap_cast_buffers_memory[i]);
    }

    //descriptor set layout
    VkDescriptorSetLayoutBinding layout_bindings[] = {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 4,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 5,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }, {
            .binding = 6,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        }
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = len(layout_bindings),
        .pBindings = layout_bindings,
    };

    error = vkCreateDescriptorSetLayout(vkon.device, &layout_info, 0, &lightmap_descriptor_set_layout);
    vk_assert("could not create lightmap descriptor set layout");

    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        VkDescriptorSetAllocateInfo desc_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkon.descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &lightmap_descriptor_set_layout,
        };

        error = vkAllocateDescriptorSets(vkon.device, &desc_alloc_info, &lightmap_descriptor_sets[i]);
        vk_assert("could not allocate lightmap descriptor sets");

        VkDescriptorImageInfo image_infos[] = {
            {
                .imageView = lightmap_color_image_views[i],
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            }, {
                .imageView = lightmap_depth_image_views[i],
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            }, {
                .sampler = vkon.texture_sampler,
                .imageView = lightmap_color_image_views[1-i],
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            }, {
                .sampler = vkon.texture_sampler,
                .imageView = lightmap_depth_image_views[1-i],
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            },
        };

        VkDescriptorBufferInfo ssbo_info = {
            .buffer = vkon.shader_storage_buffers[i],
            .offset = 0,
            .range = sizeof(global_ssbo),
        };

        VkDescriptorBufferInfo cast_buffer_info {
            .buffer = lightmap_cast_buffers[i],
            .offset = 0,
            .range = lightmap_cast_buffer_size,
        };

        VkDescriptorBufferInfo global_buffer_info = {
            .buffer = vkon.uniform_buffers[i],
            .offset = 0,
            .range = sizeof(global_uniform_buffer),
        };

        VkWriteDescriptorSet descriptor_writes[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .pImageInfo = &image_infos[0],
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .pImageInfo = &image_infos[1],
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &ssbo_info,
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 3,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &image_infos[2],
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 4,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &image_infos[3],
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 5,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &cast_buffer_info,
            }, {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = lightmap_descriptor_sets[i],
                .dstBinding = 6,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &global_buffer_info,
            }
        };

        vkUpdateDescriptorSets(vkon.device, len(descriptor_writes), descriptor_writes, 0, 0);
    }
}

void cleanup_lightmap_data()
{
    vkDestroyDescriptorSetLayout(vkon.device, lightmap_descriptor_set_layout, 0);

    for(int i = 0; i < MAX_QUEUED_FRAMES; i++) {
        vkDestroyImage(vkon.device, lightmap_color_images[i], 0);
        vkDestroyImageView(vkon.device, lightmap_color_image_views[i], 0);
        vkFreeMemory(vkon.device, lightmap_color_images_memory[i], 0);

        vkDestroyImage(vkon.device, lightmap_depth_images[i], 0);
        vkDestroyImageView(vkon.device, lightmap_depth_image_views[i], 0);
        vkFreeMemory(vkon.device, lightmap_depth_images_memory[i], 0);

        vkDestroyBuffer(vkon.device, lightmap_cast_buffers[i], 0);
        vkFreeMemory(vkon.device, lightmap_cast_buffers_memory[i], 0);
    }
}

void create_lightmap_cast_pipeline(pipeline_info* pipeline)
{
    int error;

    //pipeline layout
    // VkPushConstantRange push_constant = {
    //     .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    //     .offset = 0,
    //     .size = sizeof(lightmap_push_constants),
    // };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &lightmap_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule comp_shader = create_shader_module_from_file("data/shaders/lightmap_cast.spv");

    VkPipelineShaderStageCreateInfo comp_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = comp_shader,
        .pName = "main",
    };

    VkComputePipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = comp_info,
        .layout = pipeline->pipeline_layout,
    };

    error = vkCreateComputePipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create compute pipeline");

    vkDestroyShaderModule(vkon.device, comp_shader, 0);
}
pipeline_info* lightmap_cast_pipeline = add_pipeline(create_lightmap_cast_pipeline);

void create_lightmap_accumulate_color_pipeline(pipeline_info* pipeline)
{
    int error;

    //pipeline layout
    // VkPushConstantRange push_constant = {
    //     .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    //     .offset = 0,
    //     .size = sizeof(lightmap_push_constants),
    // };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &lightmap_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule comp_shader = create_shader_module_from_file("data/shaders/lightmap_accumulate_color.spv");

    VkPipelineShaderStageCreateInfo comp_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = comp_shader,
        .pName = "main",
    };

    VkComputePipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = comp_info,
        .layout = pipeline->pipeline_layout,
    };

    error = vkCreateComputePipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create compute pipeline");

    vkDestroyShaderModule(vkon.device, comp_shader, 0);
}
pipeline_info* lightmap_accumulate_color_pipeline = add_pipeline(create_lightmap_accumulate_color_pipeline);

void create_lightmap_accumulate_depth_pipeline(pipeline_info* pipeline)
{
    int error;

    //pipeline layout
    // VkPushConstantRange push_constant = {
    //     .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    //     .offset = 0,
    //     .size = sizeof(lightmap_push_constants),
    // };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &lightmap_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
    };

    error = vkCreatePipelineLayout(vkon.device, &pipeline_layout_info, 0, &pipeline->pipeline_layout);
    vk_assert("could not create pipeline layout");

    //pipeline
    VkShaderModule comp_shader = create_shader_module_from_file("data/shaders/lightmap_accumulate_depth.spv");

    VkPipelineShaderStageCreateInfo comp_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = comp_shader,
        .pName = "main",
    };

    VkComputePipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = comp_info,
        .layout = pipeline->pipeline_layout,
    };

    error = vkCreateComputePipelines(vkon.device, VK_NULL_HANDLE, 1, &pipeline_info, 0, &pipeline->pipeline);
    vk_assert("could not create compute pipeline");

    vkDestroyShaderModule(vkon.device, comp_shader, 0);
}
pipeline_info* lightmap_accumulate_depth_pipeline = add_pipeline(create_lightmap_accumulate_depth_pipeline);

void update_lightmap()
{
    uint32 f = vkon.current_frame;

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = lightmap_color_images[f],
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
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         0,
                         0, 0,
                         0, 0,
                         1, &barrier);
    barrier.image = lightmap_depth_images[f];
    vkCmdPipelineBarrier(vkon.command_buffers[f],
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0, 0,
                         0, 0,
                         1, &barrier);

    vkCmdBindDescriptorSets(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_COMPUTE, lightmap_cast_pipeline->pipeline_layout, 0, 1, &lightmap_descriptor_sets[f], 0, 0);

    static int32 frame_number = 0;
    // lightmap_push_constants pcs = {
    //     frame_number,
    //     vkon.time,
    // };
    // frame_number++;

    // vkCmdPushConstants(vkon.command_buffers[f], lightmap_cast_pipeline->pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int), &pcs);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_COMPUTE, lightmap_cast_pipeline->pipeline);
    vkCmdDispatch(vkon.command_buffers[f], LIGHTMAP_CAST_DISPATCH_W, LIGHTMAP_CAST_DISPATCH_H, LIGHTMAP_CAST_DISPATCH_D);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_COMPUTE, lightmap_accumulate_color_pipeline->pipeline);
    vkCmdDispatch(vkon.command_buffers[f], LIGHTMAP_ACCUMULATE_COLOR_DISPATCH_W, LIGHTMAP_ACCUMULATE_COLOR_DISPATCH_H, 1);

    vkCmdBindPipeline(vkon.command_buffers[f], VK_PIPELINE_BIND_POINT_COMPUTE, lightmap_accumulate_depth_pipeline->pipeline);
    vkCmdDispatch(vkon.command_buffers[f], LIGHTMAP_ACCUMULATE_DEPTH_DISPATCH_W, LIGHTMAP_ACCUMULATE_DEPTH_DISPATCH_H, 1);

    barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = lightmap_color_images[f],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
    };
    vkCmdPipelineBarrier(vkon.command_buffers[f],
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0, 0,
                         0, 0,
                         1, &barrier);
    barrier.image = lightmap_depth_images[f];
    vkCmdPipelineBarrier(vkon.command_buffers[f],
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0, 0,
                         0, 0,
                         1, &barrier);
}
