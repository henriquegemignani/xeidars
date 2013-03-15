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
#include <ugdk/action/scene.h>
#include <ugdk/action/generictask.h>
#include <ugdk/input/inputmanager.h>
#include <ugdk/math/vector2D.h>
#include <ugdk/graphic/videomanager.h>
#include <ugdk/graphic/node.h>
#include <ugdk/graphic/drawable.h>
#include <ugdk/graphic/texture.h>
#include <ugdk/graphic/opengl/shader.h>
#include <ugdk/graphic/opengl/shaderprogram.h>
#include <ugdk/graphic/opengl/vertexbuffer.h>
#include <ugdk/math/integer2D.h>

using namespace ugdk;
using namespace action;
using namespace graphic;

class MahDrawable : public Drawable {
  public:
    MahDrawable(Texture* texture) : size_(math::Integer2D(texture->width(), texture->height())), texture_(texture) {
        vertexbuffer_ = createBuffer();
        uvbuffer_ = createBuffer();
    }
    MahDrawable(Texture* texture, const math::Vector2D& _size) : size_(_size), texture_(texture) {
        vertexbuffer_ = createBuffer();
        uvbuffer_ = createBuffer();
    }
    ~MahDrawable() {
        delete vertexbuffer_;
        delete uvbuffer_;
    }

    void Update(double dt) {}
    void Draw(const Geometry& geometry, const VisualEffect&) const {
		// Use our shader
        opengl::ShaderProgram::Use shader_use(VIDEO_MANAGER()->default_shader());

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        shader_use.SendGeometry(geometry * Geometry(math::Vector2D(), size_));

		// Bind our texture in Texture Unit 0
        shader_use.SendTexture(0, texture_);

		// 1rst attribute buffer : vertices
        shader_use.SendVertexBuffer(vertexbuffer_, opengl::VERTEX, 0);

		// 2nd attribute buffer : UVs
        shader_use.SendVertexBuffer(uvbuffer_, opengl::TEXTURE, 0);

		// Draw the triangle !
        glDrawArrays(GL_QUADS, 0, 4); // 12*3 indices starting at 0 -> 12 triangles
    }

    const ugdk::math::Vector2D& size() const {
        return size_;
    }

  private:
    opengl::VertexBuffer* createBuffer() {
        static const GLfloat buffer_data[] = { 
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
        };
        opengl::VertexBuffer* buffer = opengl::VertexBuffer::Create(sizeof(buffer_data), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        {
            opengl::VertexBuffer::Bind bind(*buffer);
            opengl::VertexBuffer::Mapper mapper(*buffer);

            GLfloat *indices = static_cast<GLfloat*>(mapper.get());
            if (indices)
                memcpy(indices, buffer_data, sizeof(buffer_data));
        }
        return buffer;
    }

    ugdk::math::Vector2D size_;
    Texture* texture_;
    opengl::VertexBuffer* vertexbuffer_;
    opengl::VertexBuffer* uvbuffer_;
};

int main(int argc, char *argv[]) {
    auto eng = ugdk::Engine::reference();
    eng->Initialize();
    eng->video_manager()->SetVSync(true);

    MahDrawable* mahdrawable = new MahDrawable(Texture::CreateFromFile("uvtemplate.tga"), math::Vector2D(800, 600));

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
    eng->Run();
    eng->Release();
    return 0;
}
