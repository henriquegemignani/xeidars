#include <ugdk/base/engine.h>
#include <ugdk/math/vector2D.h>
#include "SDL.h"

ugdk::Engine* engine() {
    return ugdk::Engine::reference();
}

int main(int argc, char *argv[]) {
    ugdk::Configuration engine_config;
    engine_config.window_title = "Xeidas";
    engine_config.window_size  = ugdk::math::Vector2D(800, 600);
    engine_config.fullscreen   = false;

    engine_config.base_path = "./";
    engine()->Initialize(engine_config);

	engine()->Run();
    engine()->Release();
    return 0;
}
