#pragma once
#include "ECS/ComponentPool.h"

namespace ESC 
{

  /**
   * @class View
   * @brief Filtro en tiempo de compilación para iterar entidades con componentes específicos.
   * @tparam Components Paquete variádico de tipos de componentes que deben coincidir.
   *
   * Una vista optimiza las consultas del motor ECS localizando la pool más pequeña
   * para reducir drásticamente el número de validaciones y saltos en memoria.
   */
  template<typename... Components>
  class View 
  {
  public:
    /**
     * @brief Constructor explícito que enlaza y evalúa las pools de los componentes requeridos.
     * @param pools Referencias variádicas a las pools densas de cada componente.
     */
    explicit View(ComponentPool<Components>&... pools) noexcept
      : m_pool(&pools...) 
    {
      FindSmallest();
    }

    /**
     * @brief Itera las entidades válidas aplicando una función lambda que recibe la entidad y sus componentes.
     * @tparam Func Tipo deducido del objeto ejecutable (función o lambda).
     * @param func Callback con firma `void(EntityID, Components&...)`.
     */
    template<typename Func>
    void Each(Func&& func) 
    {
      if (!m_smallest) return;
      const auto& entities = m_smallest->GetEntities();
      for (std::size_t i = entities.size(); i > 0; --i) 
      {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity)) 
        {
          std::apply(
            [&](auto*... pools) 
            {
              func(entity, pools->Get(entity)...);
            },
            m_pools);
        }
      }
    }

    /**
     * @brief Itera de forma exclusiva los identificadores de entidad que cumplen con el filtro.
     * @tparam Func Tipo deducido del objeto ejecutable (función o lambda).
     * @param func Callback con firma `void(EntityID)`.
     */
    template<typename Func>
    void EachEntity(Func&& func) 
    {
      if (!m_smallest) return;
      const auto& entities = m_smallest->GetEntities();
      for (std::size_t i = entities.size(); i > 0; --i) 
      {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity)) 
        {
          func(entity);
        }
      }
    }

    /**
     * @brief Evalúa si la consulta actual no contiene ninguna entidad coincidente.
     * @return true si la pool pivote está vacía o no existe, false en caso contrario.
     */
    [[nodiscard]] bool Empty() const noexcept { return !m_smallest || m_smallest->Empty(); }

    /**
     * @brief Devuelve el tamaño base o número de elementos de la pool más pequeña detectada.
     * @return Cantidad potencial de elementos a evaluar en el peor de los casos.
     */
    [[nodiscard]] size_t Size() const noexcept { return m_smallest ? m_smallest->size() : 0; }

  private:
    /**
     * @brief Desenrolla de forma recursiva en tiempo de compilación las pools para hallar la menor.
     * @tparam I Índice actual de la tupla evaluada.
     */
    template<std::size_t I = 0>
    void FindSmallest() noexcept 
    {
      if constexpr (I < sizeof...(Components)) 
      {
        auto* pool = std::get<I>(m_pool);
        if (pool && (!m_smallest || pool->size() < m_smallest->size())) 
        {
          m_smallest = pool;
        }
        FindSmallest<I + 1>();
      }
    }

    /**
     * @brief Verifica mediante una operación fold expression si todos los contenedores poseen la entidad.
     * @param entity Identificador único de la entidad (EntityID).
     * @return true si la entidad contiene la firma completa de componentes de la vista.
     */
    [[nodiscard]] bool AllHave(EntityID entity) const noexcept 
    {
      return std::apply(
        [entity](auto*... pools) {
          return (... && (pools && pools->Contains(entity)));
        },
        m_pools);
    }

  private:
    std::tuple<ComponentPool<Components>*...> m_pool; ///< Tupla que almacena punteros a las pools de componentes.
    const SparceSet* m_smallest = nullptr;             ///< Puntero al conjunto base más ópt