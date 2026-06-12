#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"

namespace ECS 
{

  /**
   * @class SparceSet
   * @brief Estructura de datos Sparse Set para el mapeo O(1) de entidades.
   *
   * Mantiene un arreglo denso para iteración secuencial compacta y cache-friendly,
   * indexado por un arreglo disperso (sparse) que mapea los IDs de las entidades.
   */
  class SparceSet 
  {
  public:
    /**
     * @brief Constructor por defecto de SparceSet.
     */
    SparceSet() = default;

    /**
     * @brief Destructor virtual por defecto de SparceSet.
     */
    virtual ~SparceSet() = default;

    /**
     * @brief Comprueba si una entidad pertenece actualmente al Sparse Set.
     * @param entity Identificador único de la entidad (EntityID).
     * @return true si la entidad se encuentra registrada, false en caso contrario.
     */
    [[nodiscard]] bool Contains(EntityID entity) const noexcept 
    {
      const EntityIndex idx = GetEntityIndex(entity);
      if (idx >= m_sparse.size()) return false;
      const EntityIndex denseIdx = m_sparse[idx];
      return denseIdx < m_dense.size() && m_dense[denseIdx] == entity;
    }

    /**
     * @brief Obtiene la cantidad de entidades almacenadas en el conjunto denso.
     * @return Número total de elementos en el set.
     */
    [[nodiscard]] size_t size() const noexcept 
    {
      return m_dense.size();
    }

    /**
     * @brief Evalúa si el conjunto se encuentra vacío.
     * @return true si no hay entidades registradas, false de lo contrario.
     */
    [[nodiscard]] bool Empty() const noexcept 
    {
      return m_dense.empty();
    }

    /**
     * @brief Obtiene acceso directo al contenedor denso de entidades.
     * @return Referencia constante al std::vector denso.
     */
    [[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept 
    {
      return m_dense;
    }

    /**
     * @brief Elimina una entidad del conjunto manteniendo la contigüidad mediante swap and pop.
     * @param entity Identificador único de la entidad (EntityID) a remover.
     */
    virtual void Remove(EntityID entity) 
    {
      if (!Contains(entity)) return;

      const EntityIndex sparseIdx = GetEntityIndex(entity);
      const EntityIndex denseIdx = m_sparse[sparseIdx];
      const EntityID last = m_dense.back();

      m_dense[denseIdx] = last;
      m_sparse[GetEntityIndex(last)] = denseIdx;

      m_dense.pop_back();
      m_sparse[sparseIdx] = INVALID;
    }

    /**
     * @brief Limpia por completo los contenedores disperso y denso.
     */
    virtual void Clear() 
    {
      m_sparse.clear();
      m_dense.clear();
    }

  protected:
    /**
     * @brief Inserta una entidad en el conjunto, redimensionando el arreglo disperso si es necesario.
     * @param entity Identificador único de la entidad (EntityID) a añadir.
     * @return El índice asignado dentro del contenedor denso.
     */
    EntityIndex InsertEntity(EntityID entity) 
    {
      const EntityIndex sparseIdx = GetEntityIndex(entity);
      const EntityIndex denseIdx = static_cast<EntityIndex>(m_dense.size());

      if (sparseIdx >= m_sparse.size()) 
      {
        m_sparse.resize(sparseIdx + 1, INVALID);
      }

      assert(m_sparse[sparseIdx] == INVALID && "Entity already exists in the set");

      m_sparse[sparseIdx] = denseIdx;
      m_dense.push_back(entity);

      return denseIdx;
    }

  protected:
    static constexpr EntityIndex INVALID = std::numeric_limits<EntityIndex>::max(); ///< Valor centinela para posiciones vacías en el arreglo disperso.
    std::vector<EntityIndex> m_sparse; ///< Arreglo disperso indexado por el índice de la entidad.
    std::vector<EntityID> m_dense;     ///< Arreglo contiguo de IDs de entidades activas.
  };
}