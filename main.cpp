#include <iostream>
#include <SFML/Graphics.hpp>
#include "Dimentions.h"
#include "Mandelbrot.hpp"

#define SIZE_X 960
#define SIZE_Y 720

int main()
{
    Dimention<double> screen(0, SIZE_X, 0, SIZE_Y);
    Dimention<double> fract(-1.5, 1.5, -1.5, 1.5);

    sf::RenderWindow window(sf::VideoMode(SIZE_X, SIZE_Y), "OpenGL", sf::Style::Default);

    sf::Vector2i startpos;
    bool Click = false;
    double m_Ddx = 0;
    double m_Ddy = 0;
    int max_iter = 100;
    bool dirty = true;
    int smooth = false;
    bool adaptive = false;
    float zoom = 0;
    bool fullscreen = false;
    bool vsync = false;
    sf::Uint8 *pixels = new sf::Uint8[SIZE_X * SIZE_Y * 4];

    sf::Image m_Iimage;
    sf::Sprite m_Ssprite;
    sf::Texture m_Ttexture;

    Mandelbrot mb;

    window.setVerticalSyncEnabled(vsync);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Add)
                {
                    max_iter += 10;
                    dirty = true;
                    std::cout << "Max: " << max_iter << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Subtract)
                {
                    max_iter -= 10;
                    dirty = true;
                    std::cout << "Max: " << max_iter << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Up)
                {
                    max_iter += 1;
                    dirty = true;
                    std::cout << "Max: " << max_iter << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    max_iter -= 1;
                    dirty = true;
                    std::cout << "Max: " << max_iter << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    smooth++;
                    if (smooth >= 3)
                    {
                        smooth = 0;
                    }
                    std::cout << "Smooth Colors: " << smooth << std::endl;
                    dirty = true;
                }
                else if (event.key.code == sf::Keyboard::R)
                {
                    std::cout << "Reset" << std::endl;
                    double ratio = (double)window.getSize().x / (double)window.getSize().y;
                    zoom = 0;
                    max_iter = 100;
                    fract.reset(-1.5 * ratio, 1.5 * ratio, -1.5, 1.5);
                    dirty = true;
                }
                else if (event.key.code == sf::Keyboard::A)
                {
                    adaptive = !adaptive;
                    std::cout << "Adaptive iterations: " << (adaptive ? "ON" : "OFF") << std::endl;
                    if (adaptive)
                    {
                        max_iter = Dimention<float>::mmap(zoom, 0, 30, 100, 1100);
                        dirty = true;
                    }
                }
                else if (event.key.code == sf::Keyboard::F11)
                {
                    fullscreen = !fullscreen;
                    std::cout << "Fullscreen: " << (fullscreen ? "ON" : "OFF") << std::endl;
                    if (fullscreen)
                    {
                        window.create(sf::VideoMode::getFullscreenModes()[0], "OpenGL", sf::Style::Fullscreen);
                    }
                    else
                    {
                        window.create(sf::VideoMode(SIZE_X, SIZE_Y), "OpenGL", sf::Style::Default);
                    }
                }
                else if (event.key.code == sf::Keyboard::V)
                {
                    vsync = !vsync;
                    window.setVerticalSyncEnabled(vsync);
                    std::cout << "VSync: " << (vsync ? "ON" : "OFF") << std::endl;
                }
                
            }
            else if (event.type == sf::Event::Resized)
            {
                screen.reset(0, event.size.width, 0, event.size.height);
                double ratio = (double)event.size.width / (double)event.size.height;
                fract.reset(-1.5 * ratio, 1.5 * ratio, -1.5, 1.5);
                delete[] pixels;
                pixels = nullptr;
                pixels = new sf::Uint8[event.size.width * event.size.height * 4];
                dirty = true;
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                sf::Vector2i mousexy = sf::Mouse::getPosition(window);

                double px = ((double)mousexy.x / (double)screen.width() * fract.width()) + fract.x_min();
                double py = ((double)mousexy.y / (double)screen.height() * fract.height()) + fract.y_min();

                double t = 0.1 * event.mouseWheelScroll.delta;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    t *= 3;
                }
                zoom += t;
                double xmax = std::abs(fract.x_max() - px);
                double xmin = std::abs(fract.x_min() - px);
                double ymax = std::abs(fract.y_max() - py);
                double ymin = std::abs(fract.y_min() - py);

                fract.reset(fract.x_min() + t * xmin, fract.x_max() - t * xmax, fract.y_min() + t * ymin, fract.y_max() - t * ymax);
                if (adaptive)
                {
                    max_iter = Dimention<float>::mmap(zoom, 0, 30, 100, 1100);
                }
                dirty = true;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            if (!Click)
            {
                startpos = sf::Mouse::getPosition(window);
            }

            sf::Vector2i mp = sf::Mouse::getPosition(window);
            if (mp.x >= 0 && mp.y >= 0)
            {
                double dx = (Dimention<double>::mmap(mp.x - startpos.x, screen.x_min(), screen.x_max(), fract.x_min(), fract.x_max()) - fract.x_min());
                double dy = (Dimention<double>::mmap(mp.y - startpos.y, screen.y_min(), screen.y_max(), fract.y_min(), fract.y_max()) - fract.y_min());

                fract.reset(fract.x_min() + m_Ddx - dx, fract.x_max() + m_Ddx - dx, fract.y_min() + m_Ddy - dy, fract.y_max() + m_Ddy - dy);
                dirty = true;
                m_Ddx = dx;
                m_Ddy = dy;
            }
            Click = true;
        }
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && Click)
        {
            m_Ddx = 0;
            m_Ddy = 0;
            Click = false;
        }
        if (dirty)
        {
            mb.update(pixels, screen, fract, max_iter, smooth);
            m_Iimage.create(screen.width(), screen.height(), pixels);
            m_Ttexture.loadFromImage(m_Iimage);
            m_Ssprite.setTextureRect(sf::IntRect(0, 0, screen.width(), screen.height()));
            m_Ssprite.setTexture(m_Ttexture);

            window.draw(m_Ssprite);

            window.display();
            dirty = false;
        }
        else
        {
            sf::sleep(sf::milliseconds(1));
        }
    }
    delete[] pixels;
    pixels = nullptr;
    std::cout << "Goodbye :)" << std::endl;
    return 0;
}