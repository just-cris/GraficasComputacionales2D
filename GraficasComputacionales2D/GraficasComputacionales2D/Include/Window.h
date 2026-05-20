#pragma once
#include "Prerequisites.h"

class Window {
public:
  Window() = default;
  Window(int width, int height, const std::string& title);
  ~Window();

  bool
  isOpen() const;

  void
  clear(const sf::Color& color = sf::Color(0,0,0,255));

  void
  draw(const sf::Drawable & drawable,
       const sf::RenderStates& states = sf::RenderStates::Default);

  void
  close();

  void
  update();

  void 
  render();

  void
  destroy();

private:
  sf::View m_view;
  sf::RenderWindow m_window;
  sf::Time deltaTime;
  sf::Clock clock;

};