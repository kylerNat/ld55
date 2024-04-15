#ifndef UBO_BINDING
#define UBO_BINDING 0
#endif

layout(std140, binding = UBO_BINDING) uniform ubo {
    mat4 camera;
    mat3 camera_axes;
    vec3 camera_pos;
    float fov;
    float aspect_ratio;
    float time;
    int frame_number;

    int selected;
    int hovered;
};
