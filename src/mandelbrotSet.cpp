#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

const int width = 1280;
const int heigth = 720;
int itr=64;

float zoom = 300.f;
sf::Vector2f offset = -sf::Vector2f(width, heigth) / 2.f / zoom;

sf::Vector2f screenToWorld(sf::Vector2f v)
{
    return v / zoom + offset;
};

sf::Vector2f worldToScreen(sf::Vector2f v)
{
    return (v - offset)*zoom;
};


int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Mandelbrot Set",sf::Style::Close,settings);
    window.setFramerateLimit(60);

    const char* shaderCode = R"(
    #version 330 core

    uniform vec2 offset;
    uniform float zoom;
    uniform vec3 colors[16];
    uniform int maxIterations;

    out vec4 FragColor;

    void main()
    {
        vec2 c =(vec2(gl_FragCoord.xy)-vec2(0,720)) /zoom + vec2(offset.x,-offset.y);
        vec2 z = vec2(0.0, 0.0);
        int iterations = 0;

        while (iterations < maxIterations && z.x * z.x + z.y * z.y < 4.0)
        {
            vec2 temp = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
            //vec2 temp = vec2(z.x * z.x * z.x - 3.0* z.y * z.y *z.x, 3.0 * z.x * z.x * z.y - (z.y * z.y * z.y) ) + c;
            if (z == temp )
            {
                iterations = maxIterations;
                break;
            }
            z = temp;
            iterations++;
        }



        //float normalizedIterations = float(iterations) / float(maxIterations);
       // int colorIndex = int(normalizedIterations * float(16));
        //vec3 color = colors[colorIndex];
        vec3 color;

        if (iterations == maxIterations) 
        {
            color = vec3(0.0, 0.0, 0.0);
        } 
        else 
        {
            float t = float(iterations) / float(maxIterations);
            float r = 9.0 * (1.0 - t) * t * t * t;
            float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
            float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;
            color = vec3(r, g, b);
        }


        FragColor = vec4(color, 1.0);
    }
)"
;


    sf::Shader mandelbrotShader;
    mandelbrotShader.loadFromMemory(shaderCode, sf::Shader::Fragment);

    sf::Glsl::Vec3 colors[16] = {
        sf::Glsl::Vec3(0.000000, 0.000000, 0.000000),
        sf::Glsl::Vec3(0.098039, 0.027451, 0.101961),
        sf::Glsl::Vec3(0.035294, 0.003922, 0.184314),
        sf::Glsl::Vec3(0.015686, 0.015686, 0.286275),
        sf::Glsl::Vec3(0.000000, 0.027451, 0.392157),
        sf::Glsl::Vec3(0.047059, 0.172549, 0.541176),
        sf::Glsl::Vec3(0.094118, 0.321569, 0.694118),
        sf::Glsl::Vec3(0.223529, 0.490196, 0.819608),
        sf::Glsl::Vec3(0.525490, 0.709804, 0.898039),
        sf::Glsl::Vec3(0.827451, 0.925490, 0.972549),
        sf::Glsl::Vec3(0.945098, 0.913725, 0.749020),
        sf::Glsl::Vec3(0.972549, 0.788235, 0.372549),
        sf::Glsl::Vec3(1.000000, 0.666667, 0.000000),
        sf::Glsl::Vec3(0.800000, 0.501961, 0.000000),
        sf::Glsl::Vec3(0.600000, 0.341176, 0.000000),
        sf::Glsl::Vec3(0.415686, 0.203922, 0.011765)
    };
    sf::Glsl::Vec3 colors2[16] = {
     sf::Glsl::Vec3(0.192157, 0.211765, 0.584314),
     sf::Glsl::Vec3(0.090196, 0.745098, 0.811765),
     sf::Glsl::Vec3(0.992157, 0.454902, 0.180392),
     sf::Glsl::Vec3(0.960784, 0.898039, 0.317647),
     sf::Glsl::Vec3(0.992157, 0.070588, 0.811765),
     sf::Glsl::Vec3(0.317647, 0.972549, 0.196078),
     sf::Glsl::Vec3(0.992157, 0.905882, 0.074510),
     sf::Glsl::Vec3(0.325490, 0.529412, 0.792157),
     sf::Glsl::Vec3(0.078431, 0.898039, 0.317647),
     sf::Glsl::Vec3(0.992157, 0.129412, 0.070588),
     sf::Glsl::Vec3(0.576471, 0.207843, 0.992157),
     sf::Glsl::Vec3(0.070588, 0.972549, 0.956863),
     sf::Glsl::Vec3(0.972549, 0.564706, 0.070588),
     sf::Glsl::Vec3(0.960784, 0.960784, 0.960784),
     sf::Glsl::Vec3(0.549020, 0.549020, 0.549020),
     sf::Glsl::Vec3(1.0, 1.0, 1.0)
    };
    mandelbrotShader.setUniformArray("colors", colors, 16);


    sf::Clock clock;
    double dt = 0;
    double onSecondTime = 0;
    int FPScounter = 0;
    double FPSUpdatesPerSecond = 2;
    int FPS = 0;

    float deltaWheel=0;
    sf::Vector2f panningdis;
    bool keys[] = {0,0,0,0,0,0};
    
    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseWheelScrolled)deltaWheel=event.mouseWheelScroll.delta;
        }



        dt = clock.restart().asSeconds();
        onSecondTime += dt;
        FPScounter++;
        if (onSecondTime >= (1.0 / (double)FPSUpdatesPerSecond))
        {
            FPS = int(FPScounter * FPSUpdatesPerSecond);
            //printf("%s", "\nFPS : ");
            //printf("%u",FPS );

            FPScounter = 0;
            onSecondTime -= (1.0 / (double)FPSUpdatesPerSecond);

        };

        window.setTitle("Mandelbrot Set at "+std::to_string(itr)+" iteration  " +std::to_string(FPS )+" FPS");


        bool A = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        bool D = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        bool W = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        bool S = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        bool X = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        bool Z = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        bool R = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
        bool Up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        bool Down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        bool mouseLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        bool mouseRight = sf::Mouse::isButtonPressed(sf::Mouse::Right);

        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        
        if (deltaWheel >0)
        {
            sf::Vector2f mposworld = screenToWorld(mousePos);
            zoom += zoom*deltaWheel/8;
            sf::Vector2f afterzoom = screenToWorld(mousePos);
            sf::Vector2f offsetDis = mposworld - afterzoom;
            deltaWheel = 0;
            offset += offsetDis;
        };
        if (deltaWheel < 0)
        {
            sf::Vector2f mposworld = screenToWorld(mousePos);
            zoom += zoom * deltaWheel / 8;
            sf::Vector2f afterzoom = screenToWorld(mousePos);
            sf::Vector2f offsetDis = mposworld - afterzoom;
            deltaWheel = 0;
            offset += offsetDis;
        };

        if (Z)
        {
            sf::Vector2f mposworld = screenToWorld(mousePos);
            zoom *= 1.01f;           
            sf::Vector2f afterzoom = screenToWorld(mousePos);
            sf::Vector2f offsetDis = mposworld - afterzoom;  
            offset += offsetDis;

        };
        if (X)
        {
            sf::Vector2f mposworld = screenToWorld(mousePos);
            zoom *= 0.99f;
            sf::Vector2f afterzoom = screenToWorld(mousePos);
            sf::Vector2f offsetDis = mposworld - afterzoom;
            offset += offsetDis;
        };
        
        if (mouseLeft and not keys[0]) 
        {
            panningdis =  screenToWorld(mousePos);
            
        };
        if (mouseLeft)
        {
            
            offset = panningdis - mousePos/zoom;
        };
        keys[0] = mouseLeft;

        if (R)
        {
            zoom = 300.f;
            itr=64;
            offset = -sf::Vector2f(width,heigth)/2.f/zoom;
        };

        if (Up and not keys[1])itr+=64;
        if (Down and not keys[2])itr-=64;
        if (itr < 0)itr = 0;
        keys[1] = Up and not sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
        keys[2] = Down and not sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

        float speed = (500.0*dt);
        if (A)
        {
            offset.x -= speed/zoom;

        };
        if (D)
        {
            offset.x += speed /zoom;
        };
        if (W)
        {
            offset.y -= speed /zoom;

        };
        if (S)
        {
            offset.y += speed /zoom;

        };
        mandelbrotShader.setUniform("maxIterations", itr);
        mandelbrotShader.setUniform("offset", offset);
        mandelbrotShader.setUniform("zoom", zoom);

        window.clear(sf::Color(30,30,30));

        sf::RectangleShape fullscreenRect(sf::Vector2f(width,heigth));
        fullscreenRect.setFillColor(sf::Color::White);
        window.draw(fullscreenRect, &mandelbrotShader);
        
        window.display();
    }

    return 0;
}
