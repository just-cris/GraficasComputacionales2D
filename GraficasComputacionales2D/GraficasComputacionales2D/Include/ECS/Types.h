#pragma once
#include "Prerequisites.h"

/**
 * @namespace ECS
 * @brief Sistema de Entidades, Componentes y Sistemas para el motor 2D.
 *
 * Este namespace contiene las estructuras fundamentales para la gestión
 * de entidades mediante un identificador único de 64 bits divididos.
 */
namespace ECS {

  using EntityIndex = uint32_t;
  using EntityVersion = uint32_t;
  using EntityID = uint64_t;
  using ComponentTypeID = uint32_t;

  /// @brief Constante que representa una entidad nula o inválida.
  inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

  /**
   * @brief Extrae el índice (ID único) de un identificador de entidad.
   * @param id El identificador único completo de 64 bits (EntityID).
   * @return El índice de la entidad correspondiente a los 32 bits bajos.
   */
  [[nodiscard]] inline EntityIndex GetEntityIndex(EntityID id) noexcept {
    return static_cast<EntityIndex>(id & 0xFFF'FFFFFull);
  }

  /**
   * @brief Extrae la versión de reciclaje de un identificador de entidad.
   * @param id El identificador único completo de 64 bits (EntityID).
   * @return La versión de la entidad almacenada en los 32 bits altos.
   */
  [[nodiscard]] inline EntityVersion GetEntityVersion(EntityID id) noexcept {
    return static_cast<EntityVersion>((id >> 32) & 0xFFF'FFFFFull);
  }

  /**
   * @brief Combina un índice y una versión para construir un EntityID válido.
   * @param index El índice o slot asignado en el gestor de entidades.
   * @param version La versión actual para validar si la entidad sigue viva.
   * @return Un EntityID empaquetado de 64 bits listo para su uso.
   */
  [[nodiscard]] inline EntityID MakeEntityID(EntityIndex index, EntityVersion version) noexcept {
    return (static_cast<EntityID>(version) << 32) | static_cast<EntityID>(index);
  }

  /**
   * @brief Genera de forma secuencial un identificador único para nuevos componentes.
   * @return Un nuevo ComponentTypeID incremental.
   */
  [[nodiscard]] inline ComponentTypeID NextComponentTypeID() noexcept {
    static ComponentTypeID counter = 0;
    return counter++;
  }

  /**
   * @brief Obtiene el identificador único del tipo de componente en tiempo de ejecución.
   * @tparam T El tipo de la estructura o clase del componente (ej. Posicion, Velocidad).
   * @return El ComponentTypeID único asignado permanentemente a ese tipo T.
   */
  template<typename T>
  [[nodiscard]] ComponentTypeID GetComponentTypeID() noexcept {
    static const ComponentTypeID ID = NextComponentTypeID();
    return ID;
  }
}