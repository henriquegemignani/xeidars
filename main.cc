#include <ugdk/base/engine.h>
#include <ugdk/math/vector2D.h>
#include <ugdk/action/scene.h>
#include <ugdk/graphic/node.h>
#include <ugdk/graphic/drawable/texturedrectangle.h>
#include <ugdk/graphic/texture.h>
#include <ugdk/action/generictask.h>
#include <ugdk/input/inputmanager.h>
#include "SDL.h"

using namespace ugdk;
using namespace action;
using namespace graphic;

int main(int argc, char *argv[]) {
    ugdk::Configuration engine_config;
    engine_config.window_title = "Xeidas";
    engine_config.window_size  = ugdk::math::Vector2D(800, 600);
    engine_config.fullscreen   = false;

    engine_config.base_path = "./";
    auto engine = ugdk::Engine::reference();
    engine->Initialize(engine_config);

    auto t = new Scene;
    t->AddTask(new GenericTask(
        [t](double) -> bool {
            if(INPUT_MANAGER()->KeyPressed(ugdk::input::K_ESCAPE))
                t->Finish();
            return true;
        }
    ));
    auto texture = Texture::CreateFromFile("you_win.png");

    t->content_node()->set_drawable(new TexturedRectangle(texture));
    engine->PushScene(t);

	engine->Run();
    engine->Release();
    return 0;
}
