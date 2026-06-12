#pragma once
#include "Prerequisites.h"
#include "Types.h"
#include "SparseSet.h"

/**
 * @namespace ECS
 * @brief Sistema de Entidades, Componentes y Sistemas para el motor 2D.
 */
namespace ECS 
{

  /**
   * @class IComponentPool
   * @brief Interfaz polimórfica base para gestionar contenedores de componentes.
   *
   * Permite la manipulación y eliminación de componentes vinculados a entidades
   * de forma genérica a través de la herencia de un Sparse Set sin conocer
   * el tipo de dato en tiempo de compilación.
   */
  class IComponentPool : public SparceSet 
  {
  public:
    /**
     * @brief Destructor virtual por defecto de IComponentPool.
     */
    virtual ~IComponentPool() = default;

    /**
     * @brief Elimina el componente asociado a una entidad específica de la pool.
     * @param entity Identificador único de la entidad (EntityID).
     */
    virtual void RemoveEntity(EntityID entity) = 0;

    /**
     * @brief Obtiene un puntero genérico sin tipo al componente de la entidad.
     * @param entity Identificador único de la entidad (EntityID).
     * @return Puntero nulo (void*) a la dirección de memoria del componente.
     */
    virtual void* GetRaw(EntityID entity) noexcept = 0;
  };

  /**
   * @class componentPool
   * @brief Contenedor contiguo optimizado para almacenar componentes de tipo T.
   * @tparam T Estructura o clase que define los datos del componente.
   *
   * Implementa un empaquetado denso que mitiga los fallos de caché (cache misses)
   * mapeando las entidades mediante la estructura base de SparceSet.
   */
  template<typename T>
  class componentPool final : public IComponentPool 
  {
  public:
    /**
     * @brief Construye e inserta un nuevo componente asociado a una entidad.
     * @tparam Arg Paquete de tipos de argumentos para la construcción de T.
     * @param entity Identificador único de la entidad (EntityID).
     * @param args Argumentos variables reenviados para el constructor de T.
     * @return Referencia al componente T recién creado e insertado.
     */
    template<typename... Arg>
    T& Add(EntityID entity, Arg&&... args) 
    {
      assert(!Contains(entity) && "La entidad ya tiene el componente");
      InsertEntity(entity);
      m_components.emplace_back(std::forward<Arg>(args)...);
      return m_components.back();
    }

    /**
     * @brief Obtiene una referencia modificable al componente de la entidad.
     * @param entity Identificador único de la entidad (EntityID).
     * @return Referencia al componente T mapeado.
     */
    [[nodiscard]] T& Get(EntityID entity) noexcept 
    {
      assert(Contains(entity) && "La entidad no tiene el componente");
      return m_components[m_sparse[GetEntityIndex(entity)]];
    }

    /**
     * @brief Obtiene una referencia de solo lectura al componente de la entidad.
     * @param entity Identificador único de la entidad (EntityID).
     * @return Referencia constante al componente T mapeado.
     */
    [[nodiscard]] const T& Get(EntityID entity) const noexcept 
    {
      assert(Contains(entity) && "La entidad no tiene el componente");
      return m_components[m_sparse[GetEntityIndex(entity)]];
    }

    /**
     * @brief Intenta obtener el componente de la entidad sin lanzar aserciones.
     * @param entity Identificador único de la entidad (EntityID).
     * @return Puntero al componente T si existe, nullptr en caso contrario.
     */
    [[nodiscard]] T* TryGet(EntityID entity) noexcept 
    {
      if (!Contains(entity)) return nullptr;
      return &m_components[m_sparse[GetEntityIndex(entity)]];
    }

    /**
     * @brief Elimina el componente de la entidad mediante la técnica swap and pop.
     * @param entity Identificador único de la entidad (EntityID).
     */
    void Remove(EntityID entity) override 
    {
      if (!Contains(entity)) return;
      const EntityIndex denseIdx = m_sparse[GetEntityIndex(entity)];

      m_components[denseIdx] = std::move(m_components.back());
      m_components.pop_back();

      SparceSet::Remove(entity);
    }

    /**
     * @brief Sobrecarga de la interfaz polimórfica para remover componentes.
     * @param entity Identificador único de la entidad (EntityID).
     */
    void RemoveEntity(EntityID entity) override 
    {
      Remove(entity);
    }

    /**
     * @brief Sobrecarga de la interfaz para recuperar memoria en crudo.
     * @param entity Identificador único de la entidad (EntityID).
     * @return Puntero genérico (void*) al bloque de memoria del componente.
     */
    void* GetRaw(EntityID entity) noexcept override 
    {
      return TryGet(entity);
    }

    /**
     * @brief Devuelve el vector denso que contiene todos los componentes T.
     * @return Referencia al std::vector subyacente de componentes.
     */
    [[nodiscard]] std::vector<T>& GetComponents() noexcept 
    {
      return m_components;
    }

    /**
     * @brief Devuelve el vector denso de componentes T en modo solo lectura.
     * @return Referencia constante al std::vector subyacente de componentes.
     */
    [[nodiscard]] const std::vector<T>& GetComponents() const noexcept 
    {
      return m_components;
    }

    /**
     * @brief Vacía completamente el contenedor liberando la memoria.
     */
    void Clear() override 
    {
      m_components.clear();
      SparceSet::Clear();
    }

  private:
    std::vector<T> m_components; ///< Arreglo denso y contiguo de componentes T.
  };
}