#version 330

layout (location = 0) in vec3 layout_position;
layout (location = 1) in vec3 layout_color;
layout (location = 2) in vec2 layout_texCoords;
layout (location = 3) in vec3 layout_normals;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;

out vec2 v_out_texCoords;
out vec3 v_out_normals;
out vec3 v_out_fragPos;

void main()
{
    // Position Coordinates
    gl_Position = u_projection * u_view * u_model * vec4(layout_position, 1.0);

    // Color coordinates (not used in this shader)

    // texture coordinates
    v_out_texCoords = layout_texCoords;

    // normal coordinates
    // Normal0 = (gWorld * vec4(layout_normals, 0.0)).xyz;
    v_out_normals = mat3(transpose(inverse(u_model))) * layout_normals;

    // fragment position in the world
    v_out_fragPos = vec3(u_model * vec4(layout_position, 1.0));
    // WorldPos0 = (gWorld * vec4(layout_position, 1.0)).xyz;
}