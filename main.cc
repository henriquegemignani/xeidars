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
#include <ugdk/graphic/texture.h>
#include <ugdk/graphic/drawable/texturedrectangle.h>

using namespace ugdk;
using namespace action;

int main(int argc, char *argv[]) {
    auto eng = ugdk::Engine::reference();
    eng->Initialize();
    eng->video_manager()->SetVSync(true);

    graphic::TexturedRectangle* mahdrawable = new graphic::TexturedRectangle(
        graphic::Texture::CreateFromFile("you_win.png"), math::Vector2D(800, 600));

    auto scene = new Scene;
    scene->content_node()->AddChild(new graphic::Node(mahdrawable));
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
