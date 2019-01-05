#include "Mandelbrot.hpp"
#include <cmath>
#include <iostream>
struct uchar4
{
    uchar4(char r, char g, char b, char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    char r, g, b, a;
};

uchar4 convert_one_pixel_to_rgb(float h, float s, float v)
{
    float r, g, b;

    float f = h / 60.0f;
    float hi = floorf(f);
    f = f - hi;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    if (hi == 0.0f || hi == 6.0f)
    {
        r = v;
        g = t;
        b = p;
    }
    else if (hi == 1.0f)
    {
        r = q;
        g = v;
        b = p;
    }
    else if (hi == 2.0f)
    {
        r = p;
        g = v;
        b = t;
    }
    else if (hi == 3.0f)
    {
        r = p;
        g = q;
        b = v;
    }
    else if (hi == 4.0f)
    {
        r = t;
        g = p;
        b = v;
    }
    else
    {
        r = v;
        g = p;
        b = q;
    }

    unsigned char red = 255.0f * r;
    unsigned char green = 255.0f * g;
    unsigned char blue = 255.0f * b;
    unsigned char alpha = 255;
    return uchar4(red, green, blue, alpha);
}

uchar4 GetColor(int n, int max, int color)
{
    if (color == 0)
    {
        double t = (double)n / (double)max;

        unsigned int r = (int)(9 * (1 - t) * t * t * t * 255);
        unsigned int g = (int)(15 * (1 - t) * (1 - t) * t * t * 255);
        unsigned int b = (int)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);

        return uchar4(r, g, b, 255);
    }
    else if (color == 1)
    {
        int N = 256;
        int N3 = N * N * N;
        double t = (double)n / (double)max;
        n = (int)(t * (double)N3);
        int b = n / (N * N);
        int nn = n - b * N * N;
        int r = nn / N;
        int g = nn - r * N;
        return uchar4(r, g, b, 255);
    }
    else
    {
        float mmm = ((float)n / (float)max);
        return convert_one_pixel_to_rgb(mmm * 250, 1.0, 1.0);
    }
}

Mandelbrot::Mandelbrot(/* args */) : Screen(0, 0, 0, 0), Frac(0, 0, 0, 0)
{
    number = std::thread::hardware_concurrency();
    isRun = true;
    for (unsigned int y = 0; y < number; y++)
    {
        threads.push_back(new std::thread(&Mandelbrot::Worker, this, y));
    }
}

Mandelbrot::~Mandelbrot()
{
    isRun = false;
    m_cvDoWork.notify_all();
    for (auto x = threads.begin(); x != threads.end(); x++)
    {
        (*x)->join();
        delete (*x);
    }
    threads.clear();
}
void Mandelbrot::Worker(unsigned int id)
{
    while (isRun)
    {
        auto mmm = std::unique_lock<std::mutex>(m_mWorkMutex);
        m_cvDoWork.wait(mmm);
        if (!isRun)
        {
            break;
        }
        double zx, zy, zx2, zy2;
        unsigned int n = 0;
        for (unsigned int i = id; i < m_Ix * m_Iy; i += number)
        {
            zx = zy = zx2 = zy2 = 0;
            n = 0;
            int xc = i % (int)(Screen.width());
            int yc = (int)((i - xc) / Screen.width());
            double x = (double)xc / (double)Screen.width() * Frac.width() + Frac.x_min();  // x1
            double y = (double)yc / (double)Screen.height() * Frac.height() + Frac.y_min(); // x2

            for (; n < m_iter_max && zx2 + zy2 < 4; n++)
            {
                zy = 2 * zx * zy + y;
                zx = zx2 - zy2 + x;
                zx2 = zx * zx;
                zy2 = zy * zy;
            }
            uchar4 color = GetColor(n, m_iter_max, m_color);
            pixels[i * 4] = color.r;
            pixels[i * 4 + 1] = color.g;
            pixels[i * 4 + 2] = color.b;
            pixels[i * 4 + 3] = color.a;
        }
        mutex.lock();
        fThreads++;
        if (fThreads >= number)
        {
            m_cvDoNotify.notify_all();
        }
        mutex.unlock();
    }
}

void Mandelbrot::update(sf::Uint8 *pixels, Dimention<double> &screen, Dimention<double> &frac, unsigned int max_iter, int color)
{
    m_Ix = screen.width();
    m_Iy = screen.height();

    Screen = screen;
    Frac = frac;
    m_iter_max = max_iter;
    m_color = color;
    this->pixels = pixels;
    fThreads = 0;
    m_cvDoWork.notify_all();
    auto mmm = std::unique_lock<std::mutex>(m_mNotifyMutex);
    m_cvDoNotify.wait(mmm);
}