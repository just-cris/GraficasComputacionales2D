#pragma once
#include "Prerequisites.h"
class window;

class
  CShape {
public:
  CShape() = default;
  explicit CShape(ShapeType type);
  ~CShape() = default;

  void
    draw(Window& window);
  sf::Shape*
    getShape();

private:
  static std::unique_ptr<sf::Shape>
    createShape(ShapeType shapetype);
private:
  std::unique_ptr<sf::Shape> m_shape = nullptr;
  ShapeType m_shapeType;
};