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
#include <ugdk/graphic/videomanager.h>
#include <ugdk/graphic/node.h>
#include <ugdk/graphic/drawable.h>
#include <ugdk/graphic/drawable/texturedrectangle.h>
#include <ugdk/graphic/texture.h>
#include <ugdk/action/generictask.h>
#include <ugdk/input/inputmanager.h>
#include <ugdk/graphic/opengl/shader.h>
#include <ugdk/graphic/opengl/shaderprogram.h>
#include <ugdk/graphic/opengl/vertexbuffer.h>

using namespace ugdk;
using namespace action;
using namespace graphic;

bool setupprogram(opengl::ShaderProgram& program, const char* vertex, const char* fragment) {
    opengl::Shader vertex_shader(GL_VERTEX_SHADER), fragment_shader(GL_FRAGMENT_SHADER);

    auto f = [](const char* filename) -> std::string {
        std::ifstream in(filename, std::ios::in | std::ios::binary);
        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return(contents.str());
    };

    vertex_shader.CompileSource(f(vertex));
    fragment_shader.CompileSource(f(fragment));
    
    program.AttachShader(vertex_shader);
    program.AttachShader(fragment_shader);
    
    return program.SetupProgram();
}

class MahDrawable : public Drawable {
  public:
    MahDrawable(opengl::ShaderProgram* program, opengl::VertexBuffer* _vertexbuffer, opengl::VertexBuffer* _uvbuffer) 
            : program_(program), vertexbuffer_(_vertexbuffer), uvbuffer_(_uvbuffer) {

        // Load the texture using any two methods
        texture_ = Texture::CreateFromFile("uvtemplate.tga");
    }
    ~MahDrawable() {
        delete texture_;
        delete vertexbuffer_;
        delete uvbuffer_;
    }

    void Update(double dt) {}
    void Draw(const Geometry& geometry, const VisualEffect&) const {
		// Use our shader
        program_->Use();

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        program_->SendGeometry(geometry);

		// Bind our texture in Texture Unit 0
        program_->SendTexture(0, texture_);

		// 1rst attribute buffer : vertices
        GLuint vertex_attrib = program_->SendVertexBuffer(vertexbuffer_, opengl::VERTEX, 0);

		// 2nd attribute buffer : UVs
        GLuint texture_attrib = program_->SendVertexBuffer(uvbuffer_, opengl::TEXTURE, 0);

		// Draw the triangle !
        glDrawArrays(GL_QUADS, 0, 4); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(vertex_attrib);
		glDisableVertexAttribArray(texture_attrib);
    }

    const ugdk::math::Vector2D& size() const {
        return size_;
    }

  private:
    ugdk::math::Vector2D size_;
    opengl::ShaderProgram* program_;
    Texture* texture_;
    opengl::VertexBuffer* vertexbuffer_;
    opengl::VertexBuffer* uvbuffer_;
};

opengl::VertexBuffer* make_vertex_buffer() {
    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = { 
		   0.0f,-100.0f,
		 800.0f,-100.0f,
		 800.0f, 600.0f,
		 200.0f, 350.0f
	};
	
    opengl::VertexBuffer* buffer = opengl::VertexBuffer::Create(sizeof(g_vertex_buffer_data), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    {
        opengl::VertexBuffer::Bind bind(*buffer);
        opengl::VertexBuffer::Mapper mapper(*buffer);

        GLfloat *indices = static_cast<GLfloat*>(mapper.get());
        if (indices)
            memcpy(indices, g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    }

    return buffer;
}

opengl::VertexBuffer* make_uv_buffer() {
	// Two UV coordinatesfor each vertex. They were created withe Blender.
	static const GLfloat g_uv_buffer_data[] = { 
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

    opengl::VertexBuffer* buffer = opengl::VertexBuffer::Create(sizeof(g_uv_buffer_data), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    {
        opengl::VertexBuffer::Bind bind(*buffer);
        opengl::VertexBuffer::Mapper mapper(*buffer);

        GLfloat *indices = static_cast<GLfloat*>(mapper.get());
        if (indices)
            memcpy(indices, g_uv_buffer_data, sizeof(g_uv_buffer_data));
    }
    return buffer;
}

int main(int argc, char *argv[]) {
    auto eng = ugdk::Engine::reference();
    eng->Initialize();
    eng->video_manager()->SetVSync(true);

    opengl::ShaderProgram* myprogram = new opengl::ShaderProgram;
    setupprogram(*myprogram, "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    MahDrawable* mahdrawable = new MahDrawable(myprogram, make_vertex_buffer(), make_uv_buffer());

    auto scene = new Scene;
    scene->content_node()->set_drawable(mahdrawable);
    scene->AddTask(new GenericTask(
        [scene](double) -> bool {
            if(INPUT_MANAGER()->KeyPressed(ugdk::input::K_ESCAPE))
                scene->Finish();
            return true;
        }
    ));

    eng->PushScene(scene);
    bool ugdk_engine = true;

    if(ugdk_engine) {
        eng->Run();
    } else {
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

            scene->content_node()->Render(Geometry(), VisualEffect());

            SDL_GL_SwapBuffers();
            
            // Clear the screen
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        }
    }
    return 0;
}
