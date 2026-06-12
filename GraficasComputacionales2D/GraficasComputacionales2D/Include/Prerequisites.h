#pragma once

// Librerías estándar
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <map>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <limits>
#include <tuple>
#include <cstddef>
#include <queue>

// Librería externa: SFML
#include <SFML/Graphics.hpp>

/**
 * @def SAFE_PTR_RELEASE
 * @brief Macro para liberar de forma segura la memoria de un puntero crudo y setearlo a nullptr.
 * @param x Puntero que se desea evaluar y liberar.
 */
#define SAFE_PTR_RELEASE(x) if(x != nullptr) { delete x; x = nullptr; }

 /**
  * @def MESSAGE
  * @brief Imprime un mensaje informativo formateado en el flujo de errores estándar (std::cerr).
  * @param classObj Nombre de la clase u objeto que origina el mensaje.
  * @param method Nombre del método o función actual.
  * @param state Estado o descripción del recurso creado.
  */
#define MESSAGE(classObj, method, state)                      \
{                                                             \
    std::ostringstream os_;                                   \
    os_ << classObj << "::" << method << " : "                \
        << "[CREATION OF RESOURCE" << ": " << state << "]\n"; \
    std::cerr << os_.str();                                   \
}

  /**
   * @def ERROR
   * @brief Imprime un mensaje de error crítico en std::cerr y aborta la ejecución del programa.
   * @param classObj Nombre de la clase u objeto que origina el error.
   * @param method Nombre del método o función actual.
   * @param errorMSG Mensaje descriptivo del error o parámetro inválido.
   */
#define ERROR(classObj, method, errorMSG)                         \
{                                                                 \
    std::ostringstream os_;                                       \
    os_ << "ERROR : " << classObj << "::" << method << " : "      \
        << "  Error in data from params [" << errorMSG << "]\n";  \
    std::cerr << os_.str();                                       \
    exit(1);                                                      \
}

   /**
    * @enum ShapeType
    * @brief Tipos de figuras geométricas primitivas soportadas por el motor de renderizado.
    */
enum ShapeType 
{
  EMPTY = 0,     ///< Sin figura geométrica definida.
  CIRCLE = 1,    ///< Representa una figura circular (sf::CircleShape).
  RECTANGLE = 2, ///< Representa una figura rectangular (sf::RectangleShape).
  TRINAGLE = 3,  ///< Representa una figura triangular u homólogos de 3 vértices.
  POLYGON = 4,   ///< Representa una figura poligonal de N vértices (sf::ConvexShape).
  SQUARE = 5     ///< Representa un caso especializado de cuadrilátero equilátero.
};