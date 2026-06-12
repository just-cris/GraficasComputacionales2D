#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Gestiona la ventana de renderizado de SFML y el ciclo de vida del contexto gráfico.
 *
 * Esta clase encapsula la ventana de renderizado principal (sf::RenderWindow), el control
 * del tiempo de juego (deltaTime) y la vista de la cámara para la simulación 2D.
 */
class Window 
{
public:
  /**
   * @brief Constructor por defecto de Window.
   */
  Window() = default;

  /**
   * @brief Constructor que inicializa la ventana con dimensiones y título específicos.
   * @param width Ancho de la ventana en píxeles.
   * @param height Alto de la ventana en píxeles.
   * @param title Cadena de texto que se mostrará en la barra de título de la ventana.
   */
  Window(int width, int height, const std::string& title);

  /**
   * @brief Destructor de la clase Window encargado de liberar recursos del contexto gráfico.
   */
  ~Window();

  /**
   * @brief Comprueba si la ventana se encuentra abierta y activa.
   * @return true si la ventana está abierta, false en caso contrario.
   */
  bool isOpen() const;

  /**
   * @brief Limpia el búfer de la pantalla rellenándolo con un color uniforme.
   * @param color El color base de limpieza (por defecto es negro absoluto: 0, 0, 0, 255).
   */
  void clear(const sf::Color& color = sf::Color(0, 0, 0, 255));

  /**
   * @brief Dibuja un objeto renderizable de SFML utilizando los estados de renderizado proporcionados.
   * @param drawable Referencia al objeto gráfico que hereda de sf::Drawable que se desea dibujar.
   * @param states Configuración y estados de renderizado (shaders, transformaciones, texturas, etc.).
   */
  void draw(const sf::Drawable& drawable,
    const sf::RenderStates& states = sf::RenderStates::Default);

  /**
   * @brief Solicita el cierre inmediato de la ventana gráfica.
   */
  void close();

  /**
   * @brief Actualiza la lógica interna de la ventana y calcula el deltaTime del frame.
   */
  void update();

  /**
   * @brief Procesa el renderizado y despliega los elementos dibujados en la pantalla.
   */
  void render();

  /**
   * @brief Destruye el contexto de la ventana y libera de forma explícita los recursos gráficos asociados.
   */
  void destroy();

private:
  sf::View m_view;             ///< Vista de cámara de SFML para definir el área de visualización 2D.
  sf::RenderWindow m_window;   ///< Instancia nativa de la ventana de renderizado de SFML.
  sf::Time deltaTime;          ///< Tiempo transcurrido acumulado del frame actual.
  sf::Clock clock;             ///< Reloj de alta resolución de SFML para medir el tiempo transcurrido.
};