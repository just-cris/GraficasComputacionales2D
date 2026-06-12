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

#define SAFE_PTR_RELEASE(x) if(x != nullptr) { delete x; x = nullptr; }

#define MESSAGE(classObj, method, state)                      \
{                                                             \
    std::ostringstream os_;                                   \
    os_ << classObj << "::" << method << " : "                \
        << "[CREATION OF RESOURCE" << ": " << state << "]\n"; \
    std::cerr << os_.str();                                   \
}

#define ERROR(classObj, method, errorMSG)                         \
{                                                                 \
    std::ostringstream os_;                                       \
    os_ << "ERROR : " << classObj << "::" << method << " : "      \
        << "  Error in data from params [" << errorMSG << "]\n";  \
    std::cerr << os_.str();                                       \
    exit(1);                                                      \
}

enum ShapeType {
  EMPTY = 0, ///< Sin figura
  CIRCLE = 1, ///< Círculo
  RECTANGLE = 2, ///< Rectángulo
  TRINAGLE = 3, ///< Triángulo (typo corregible si lo deseas)
  POLYGON = 4, ///< Polígono
  SQUARE = 5  ///< Cuadrado
};