#pragma once
#include <SFML/Graphics.hpp>
#include "Dimentions.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class Mandelbrot
{
private:
  void Worker(unsigned int id);
  std::vector<std::thread *> threads;
  unsigned int number;

  std::condition_variable m_cvDoWork;

  std::condition_variable m_cvDoNotify;
  std::mutex m_mNotifyMutex;

  std::mutex mutex;

  bool isRun;
  unsigned int m_Ix;
  unsigned int m_Iy;
  unsigned int m_iter_max;

  Dimention<double> Screen;
  Dimention<double> Frac;
  unsigned int m_color;

  sf::Uint8 *pixels;

  unsigned int fThreads;

public:
  Mandelbrot(/* args */);
  ~Mandelbrot();
  void update(sf::Uint8 *pixels, Dimention<double> &screen, Dimention<double> &frac, unsigned int max_iter, int color);
};
