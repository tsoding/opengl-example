#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>

#include <png.h>

#define OPENGL_WIDTH 800
#define OPENGL_HEIGHT 600

const char *gl_shader_type_as_cstr(GLenum shader_type)
{
    switch (shader_type) {
    case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
    case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
    default: return "UNKNOWN";
    }
}

GLuint gl_create_and_compile_shader(GLenum shader_type, const char *source)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        const size_t MESSAGE_CAPACITY = 1024;
        GLsizei log_length = 0;
        GLchar message[MESSAGE_CAPACITY];
        glGetShaderInfoLog(shader, MESSAGE_CAPACITY, &log_length, message);
        fprintf(stderr, "%s failed compilation: %.*s",
                gl_shader_type_as_cstr(shader_type),
                log_length, message);
        abort();
    }

    return shader;
}

GLuint gl_create_and_link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        const size_t MESSAGE_CAPACITY = 1024;
        GLsizei log_length = 0;
        GLchar message[MESSAGE_CAPACITY];
        glGetProgramInfoLog(program, MESSAGE_CAPACITY, &log_length, message);
        fprintf(stderr, "Program failed linkage: %.*s", log_length, message);
        abort();
    }

    return program;
}

int main(int argc, char *argv[])
{
    if (!glfwInit()) {
        fprintf(stderr, "Error: Could not initialize GLFW!\n");
        abort();
    }

    GLFWwindow *window = glfwCreateWindow(
        OPENGL_WIDTH, OPENGL_HEIGHT, "OpenGL in 2020 KEKW",
        NULL, NULL);

    if (!window) {
        fprintf(stderr, "Error: Could not create GLFW Window!\n");
        glfwTerminate();
        abort();
    }

    glfwMakeContextCurrent(window);

    float mesh[6][4] = {
        // position,    texcoord
        {-0.5f, -0.5f, 0.0f, 1.0f},
        { 0.5f, -0.5f, 1.0f, 1.0f},
        { 0.5f,  0.5f, 1.0f, 0.0f},
        { 0.5f,  0.5f, 1.0f, 0.0f},
        {-0.5f,  0.5f, 0.0f, 0.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f},
    };

    const size_t mesh_count = sizeof(mesh) / sizeof(mesh[0]);
    const size_t mesh_component_count = sizeof(mesh[0]) / sizeof(mesh[0][0]);

    // BUFFERS
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);

    const GLuint position_index = 0;
    glEnableVertexAttribArray(position_index);
    glVertexAttribPointer(position_index, mesh_component_count, GL_FLOAT, GL_FALSE, 0, NULL);

    // TEXTURES
    GLuint texture = 0;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const char *image_filename = "./cakew.png";
    png_image image = {};
    image.version = PNG_IMAGE_VERSION;
    if (!png_image_begin_read_from_file(&image, image_filename)) {
        fprintf(stderr, "Could not read file `%s`: %s\n", image_filename, image.message);
        abort();
    }
    image.format = PNG_FORMAT_RGBA;

    uint32_t *image_pixels = malloc(sizeof(uint32_t) * image.width * image.height);
    if (image_pixels == NULL) {
        fprintf(stderr, "Could not allocate memory for an image\n");
        abort();
    }

    if (!png_image_finish_read(&image, NULL, image_pixels, 0, NULL)) {
        fprintf(stderr, "libpng pooped itself: %s\n", image.message);
        abort();
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image.width,
                 image.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image_pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // SHADERS
    GLuint vertex_shader = gl_create_and_compile_shader(
        GL_VERTEX_SHADER,
        "#version 130\n"
        "\n"
        "in vec4 position;\n"
        "out vec2 texcoord;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vec4(position.xy, 0.0f, 1.0f);\n"
        "    texcoord = position.zw;\n"
        "}\n"
        "\n");

    GLuint fragment_shader = gl_create_and_compile_shader(
        GL_FRAGMENT_SHADER,
        "#version 130\n"
        "\n"
        "in vec2 texcoord;\n"
        "uniform sampler2D tex;\n"
        "\n"
        "void main() {\n"
        "    gl_FragColor = mix(texture(tex, texcoord), vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.25f);\n"
        "}\n"
        "\n");

    GLuint program = gl_create_and_link_program(vertex_shader, fragment_shader);

    // TODO: clean up stuff properly
    //    Deleting probably requires detaching the shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glUseProgram(program);

    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.0f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, position_index, mesh_count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
