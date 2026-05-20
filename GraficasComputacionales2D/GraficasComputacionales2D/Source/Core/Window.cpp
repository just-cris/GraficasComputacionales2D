#include "Core/Window.h"
#include "Prerequisites.h"

Window::Window(int width, int height, const std::string& title) {
  m_window = new sf::RenderWindow(sf::VideoMode({static_cast<unsigned int>(width),
                                                 static_cast<unsigned int>(height)}),
            title,
            sf::Style::Default);
  if (m_window) {
    m_window->setFramerateLimit(60);
    MESSAGE("Window","Window", "Window created succesfully");
  }
  else {
    ERROR("Window", "Window", "Failed creating window");
  }
}

Window::~Window() {
  destroy();
}

bool
Window::isOpen() const {
  //check if the window is not null
  if (!m_window) {
    return m_window && m_window->isOpen();
  }else{
    ERROR("Window", "isOpen", "is null");
    return false;
  }
}

void 
Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
  if (m_window) {
    m_window->draw(drawable, states);
  }
  else {
    ERROR("Window", "draw", "Failed drawing, window is null");
  }
}

void
Window::display() {
  if (m_window) {
    m_window->display();
  }
  else {
    ERROR("Window", "display", "window is null");
  }
}

void 
Window::update() {
  //almacena deltaTime una vez
  deltaTime = clock.restart();
}

void
Window::render() {
}

void
Window::destroy() {
  SAFE_PTR_RELEASE(m_window);
}