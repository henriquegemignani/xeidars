#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL_opengl.h"
#include "SDL.h"

#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <ugdk/base/engine.h>
#include <ugdk/math/vector2D.h>
#include <ugdk/action/scene.h>
#include <ugdk/graphic/node.h>
#include <ugdk/graphic/drawable/texturedrectangle.h>
#include <ugdk/graphic/texture.h>
#include <ugdk/action/generictask.h>
#include <ugdk/input/inputmanager.h>
#include <ugdk/graphic/shader/shader.h>
#include <ugdk/graphic/shader/shaderprogram.h>

namespace ugdk {
namespace graphic {
extern bool ConvertSurfaceToTexture(SDL_Surface* data, GLuint* texture_, int* texture_width_, int* texture_height_);
}
}

using namespace ugdk;
using namespace action;
using namespace graphic;

bool setupprogram(shader::ShaderProgram& program, const char* vertex, const char* fragment) {
    shader::Shader vertex_shader(GL_VERTEX_SHADER), fragment_shader(GL_FRAGMENT_SHADER);

    auto f = [](const char* filename) -> std::string {
        std::ifstream in(filename, std::ios::in | std::ios::binary);
        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return(contents.str());
    };

    vertex_shader.CompileSource(f(vertex).c_str());
    fragment_shader.CompileSource(f(fragment).c_str());
    
    program.AttachShader(vertex_shader);
    program.AttachShader(fragment_shader);
    
    return program.SetupProgram();
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

    glewInit();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    shader::ShaderProgram myprogram;
    setupprogram(myprogram, "SimpleTransform.vertexshader", "SingleColor.fragmentshader");

    GLuint programID = myprogram.id();

    // Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};
	static const GLushort g_element_buffer_data[] = { 0, 1, 2 };

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    bool quit = false;
    while(!quit) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: quit = true; break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                default: break;
            }
        }
		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

        SDL_GL_SwapBuffers();
    }

    glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
    return 0;
}
