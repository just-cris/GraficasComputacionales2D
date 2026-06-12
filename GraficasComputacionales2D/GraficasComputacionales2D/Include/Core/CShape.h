#pragma once
#include "Prerequisites.h"

class Window;

/**
 * @class CShape
 * @brief Clase envoltorio para la gestión y renderizado de figuras geométricas de SFML.
 *
 * Administra el ciclo de vida de una figura de SFML mediante punteros inteligentes,
 * abstrayendo su creación y facilitando su dibujado en la ventana de la aplicación.
 */
class CShape 
{
public:
  /**
   * @brief Constructor por defecto de CShape.
   */
  CShape() = default;

  /**
   * @brief Constructor explícito que inicializa la figura según su tipo.
   * @param type El tipo de figura geométrica a instanciar (ShapeType).
   */
  explicit CShape(ShapeType type);

  /**
   * @brief Destructor por defecto de CShape.
   */
  ~CShape() = default;

  /**
   * @brief Renderiza la figura geométrica en la ventana especificada.
   * @param window Referencia a la ventana de la aplicación encargada del dibujado.
   */
  void draw(Window& window);

  /**
   * @brief Obtiene el puntero subyacente de la figura de SFML.
   * @return Un puntero crudo a la instancia de sf::Shape manejada externamente.
   */
  sf::Shape* getShape();

private:
  /**
   * @brief Método de factoría estático encargado de crear la figura de SFML adecuada.
   * @param shapetype El tipo geométrico de la figura que se desea construir.
   * @return Un std::unique_ptr que contiene la figura de SFML inicializada.
   */
  static std::unique_ptr<sf::Shape> createShape(ShapeType shapeType);

private:
  std::unique_ptr<sf::Shape> m_shape = nullptr; ///< Puntero inteligente a la figura de SFML.
  ShapeType m_shapeType;                         ///< Tipo de la figura geométrica actual.
};