#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL_opengl.h"
#include "SDL.h"

#include <ugdk/base/engine.h>
#include <ugdk/action/scene.h>
#include <ugdk/action/generictask.h>
#include <ugdk/input/inputmanager.h>
#include <ugdk/math/vector2D.h>
#include <ugdk/graphic/videomanager.h>
#include <ugdk/graphic/node.h>
#include <ugdk/graphic/texture.h>
#include <ugdk/graphic/drawable/texturedrectangle.h>
#include <ugdk/graphic/opengl/shader.h>
#include <ugdk/graphic/opengl/shaderprogram.h>

using namespace ugdk;
using namespace graphic;
using namespace action;

graphic::opengl::ShaderProgram* MAHSHADER;
float magic_value = 1.0;
float magic_value2 = 1.0;
graphic::opengl::ShaderProgram* make_shader() {
    opengl::Shader vertex_shader(GL_VERTEX_SHADER), fragment_shader(GL_FRAGMENT_SHADER);

    // VERTEX
    vertex_shader.AddCodeBlock("out vec2 position;" "\n"
                               "void calculateLightUV() {" "\n"
                               "   position = gl_Position.xy;" "\n"
                               "}" "\n");

    vertex_shader.AddLineInMain("	gl_Position =  geometry_matrix * vec4(vertexPosition,0,1);" "\n");
    vertex_shader.AddLineInMain("   calculateLightUV();" "\n");
    vertex_shader.GenerateSource();

    // FRAGMENT
    fragment_shader.AddCodeBlock("in vec2 position;" "\n"
                                 "uniform float magic_value;" "\n"
                                 "uniform float magic_value2;" "\n");

    
    fragment_shader.AddLineInMain("	float s1 = 1 + 0.5*sin(magic_value), s2 = 1 + 0.5*sin(magic_value2);" "\n");
    fragment_shader.AddLineInMain("	float dist = pow(position.x / s1 + position.y / s2, 2);" "\n");
    fragment_shader.AddLineInMain("	vec4 color = vec4(s1 * dist, s2 * dist, dist, 1.0);" "\n");
    fragment_shader.AddLineInMain(" gl_FragColor = color;" "\n");
    fragment_shader.GenerateSource();

    opengl::ShaderProgram* myprogram = new opengl::ShaderProgram;

    myprogram->AttachShader(vertex_shader);
    myprogram->AttachShader(fragment_shader);

    bool status = myprogram->SetupProgram();
    assert(status);
    return myprogram;
}

int main(int argc, char *argv[]) {
    auto eng = ugdk::Engine::reference();
    eng->Initialize();
    eng->video_manager()->SetVSync(true);

    MAHSHADER = make_shader();
    bool flags[2] = {true,false};
    eng->video_manager()->shaders().ReplaceShader(flags, MAHSHADER);
    
    bool light_system = false;
    Scene* scene = new Scene;
    scene->content_node()->AddChild(new graphic::Node(new TexturedRectangle(
        graphic::Texture::CreateFromFile("you_win.png"),
        math::Vector2D(800, 600)
    )));
    scene->AddTask(new GenericTask(
        [scene,&light_system](double dt) -> bool {
            input::InputManager* input = INPUT_MANAGER();
            if(input->KeyDown(input::K_UP))
                magic_value += dt;
            else if(input->KeyDown(input::K_DOWN))
                magic_value -= dt;
            if(input->KeyDown(input::K_LEFT))
                magic_value2 += dt;
            else if(input->KeyDown(input::K_RIGHT))
                magic_value2 -= dt;
            
            if(input->KeyPressed(input::K_TAB))
                VIDEO_MANAGER()->SetLightSystem(light_system = !light_system);

            {
                opengl::ShaderProgram::Use shader_use(MAHSHADER);
                shader_use.SendUniform("magic_value", magic_value);
                shader_use.SendUniform("magic_value2", magic_value2);
            }

            if(input->KeyPressed(ugdk::input::K_ESCAPE))
                scene->Finish();
            return true;
        }
    ));

    eng->PushScene(scene);
    eng->Run();
    eng->Release();
    return 0;
}
