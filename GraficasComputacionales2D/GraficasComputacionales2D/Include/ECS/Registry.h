#pragma once
#include "Types.h"
#include "ComponentPool.h"
#include "View.h"
#include "System.h"

namespace ECS 
{

  /**
   * @class Registry
   * @brief El concentrador central de datos del sistema ECS.
   *
   * Gestiona el ciclo de vida de las entidades, la asignación y búsqueda de
   * pools de componentes, y la ejecución y actualización secuencial de sistemas.
   */
  class Registry 
  {
  public:
    /**
     * @brief Crea una nueva entidad asignando un índice disponible o expandiendo el contenedor.
     * @return El identificador único completo de 64 bits (EntityID) de la nueva entidad.
     */
    EntityID CreateEntity() 
    {
      EntityIndex idx;
      if (!m_freeList.empty()) 
      {
        idx = m_freeList.front();
        m_freeList.pop();
      }
      else 
      {
        idx = static_cast<EntityIndex>(m_entities.size());
        m_versions.push_back(0);
        m_entities.push_back(NULL_ENTITY); // placeholder
      }
      EntityID id = MakeEntityID(idx, m_versions[idx]);
      m_entities[idx] = id;
      return id;
    }

    /**
     * @brief Destruye una entidad, borrando todos sus componentes y reciclando su índice.
     * @param entity Identificador único de la entidad (EntityID) a eliminar.
     */
    void DestroyEntity(EntityID entity) 
    {
      assert(IsAlive(entity) && "DestroyEntity: entidad inválida o ya destruida");

      // elimina todos los componentes de esta entidad
      for (auto& [typeID, pool] : m_componentPools)
        pool->RemoveEntity(entity);

      // incrementa version, invalida IDs viejos
      const EntityIndex idx = GetEntityIndex(entity);
      ++m_versions[idx];
      m_entities[idx] = NULL_ENTITY;
      m_freeList.push(idx);
    }

    /**
     * @brief Verifica si una entidad sigue viva en base a su versión de reciclaje.
     * @param entity Identificador único de la entidad (EntityID).
     * @return true si la entidad está activa, false en caso contrario.
     */
    [[nodiscard]] bool IsAlive(EntityID entity) const noexcept 
    {
      const EntityIndex idx = GetEntityIndex(entity);
      return idx < m_entities.size() && m_entities[idx] == entity;
    }

    /**
     * @brief Obtiene el número neto de entidades activas y vivas actualmente.
     * @return Cantidad de entidades válidas en el registro.
     */
    [[nodiscard]] std::size_t EntityCount() const noexcept 
    {
      return m_entities.size() - m_freeList.size();
    }

    /**
     * @brief Obtiene el vector completo que representa el mapa denso de entidades.
     * @return Referencia constante al std::vector interno de EntityID.
     */
    [[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept 
    {
      return m_entities;
    }

    /**
     * @brief Añade o construye un nuevo componente asociado a una entidad.
     * @tparam T Tipo de componente que se desea adjuntar.
     * @tparam Args Tipos variables para inicializar los miembros del componente T.
     * @param entity Identificador de la entidad receptora.
     * @param args Argumentos reenviados directamente al constructor de T.
     * @return Referencia al componente T recién instanciado.
     */
    template<typename T, typename... Args>
    T& AddComponent(EntityId entity, Args&&... args) 
    {
      assert(IsAlive(entity) && "AddComponent: entidad inválida");
      return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Remueve de forma segura el componente de tipo T de la entidad indicada.
     * @tparam T Tipo de componente a desvincular.
     * @param entity Identificador de la entidad afectada.
     */
    template<typename T>
    void RemoveComponent(EntityId entity) 
    {
      if (auto* pool = GetPool<T>())
        pool->Remove(entity);
    }

    /**
     * @brief Asigna o actualiza el componente T de una entidad con un valor existente.
     * @tparam T Tipo de componente.
     * @param entity Identificador de la entidad.
     * @param value Instancia u objeto con los nuevos datos a copiar o mover.
     * @return Referencia al componente T actualizado.
     */
    template<typename T>
    T& SetComponent(EntityID entity, T value) 
    {
      assert(IsAlive(entity) && "SetComponent: entidad inválida");
      auto* pool = GetOrCreatePool<T>();
      if (pool->Contains(entity)) 
      {
        pool->Get(entity) = std::move(value);
        return pool->Get(entity);
      }
      return pool->Add(entity, std::move(value));
    }

    /**
     * @brief Evalúa si una entidad cuenta con el componente de tipo T adjunto.
     * @tparam T Tipo de componente a buscar.
     * @param entity Identificador de la entidad evaluada.
     * @return true si la entidad posee el componente, false de lo contrario.
     */
    template<typename T>
    [[nodiscard]] bool HasComponent(EntityId entity) const noexcept 
    {
      const auto* pool = GetPool<T>();
      return pool && pool->Contains(entity);
    }

    /**
     * @brief Obtiene una referencia editable al componente T de la entidad.
     * @tparam T Tipo de componente.
     * @param entity Identificador de la entidad.
     * @return Referencia directa al componente T.
     */
    template<typename T>
    [[nodiscard]] T& GetComponent(EntityID entity) 
    {
      assert(IsAlive(entity));
      auto* pool = GetPool<T>();
      assert(pool && "GetComponent: pool no existe para este tipo");
      return pool->Get(entity);
    }

    /**
     * @brief Obtiene una referencia constante de solo lectura al componente T de la entidad.
     * @tparam T Tipo de componente.
     * @param entity Identificador de la entidad.
     * @return Referencia constante al componente T.
     */
    template<typename T>
    [[nodiscard]] const T& GetComponent(EntityID entity) const 
    {
      assert(IsAlive(entity));
      const auto* pool = GetPool<T>();
      assert(pool && "GetComponent: pool no existe para este tipo");
      return pool->Get(entity);
    }

    /**
     * @brief Intenta recuperar un puntero al componente T si la entidad y la pool existen.
     * @tparam T Tipo de componente.
     * @param entity Identificador de la entidad.
     * @return Puntero a la dirección del componente T, o nullptr si no se localiza.
     */
    template<typename T>
    [[nodiscard]] T* TryGetComponent(EntityID entity) noexcept 
    {
      if (!IsAlive(entity)) return nullptr;
      auto* pool = GetPool<T>();
      return pool ? pool->TryGet(entity) : nullptr;
    }

    /**
     * @brief Crea una vista (View) para iterar sobre las entidades que tengan un conjunto de componentes.
     * @tparam Components Lista variádica de tipos de componentes necesarios.
     * @return Una instancia de View configurada con las pools requeridas.
     */
    template<typename... Components>
    [[nodiscard]] View<Components...> GetView() 
    {
      return View<Components...>(GetOrCreatePool<Components>()...);
    }

    /**
     * @brief Instancia e inicializa un nuevo Sistema derivado en la arquitectura.
     * @tparam T Clase o tipo del sistema derivado de ECS::System.
     * @tparam Args Tipos variádicos para el constructor del sistema T.
     * @param args Parámetros de inicialización para construir el sistema T.
     * @return Referencia al sistema T recién creado.
     */
    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) 
    {
      static_assert(std::is_base_of_v<System, T>, "T debe derivar de ECS::System");
      auto system = std::make_unique<T>(std::forward<Args>(args)...);
      T& ref = *system;
      system->OnStart(*this);
      m_systems.push_back(std::move(system));
      return ref;
    }

    /**
     * @brief Invoca el método OnUpdate en todos los sistemas activos y habilitados.
     * @param deltaTime Tiempo fraccionario transcurrido desde el último ciclo.
     */
    void UpdateSystems(float deltaTime) 
    {
      for (auto& system : m_systems)
        if (system->IsEnable())
          system->OnUpdate(*this, deltaTime);
    }

    /**
     * @brief Notifica el ciclo de destrucción y desvincula todos los sistemas registrados.
     */
    void RemoveAllSystems() 
    {
      for (auto& system : m_systems)
        system->OnDestroy(*this);
      m_systems.clear();
    }

    /**
     * @brief Limpia por completo el estado del motor eliminando sistemas, pools y entidades.
     */
    void Clear() 
    {
      RemoveAllSystems();

      for (auto& [typeID, pool] : m_componentPools)
        pool->Clear();
      m_entities.clear();
      m_versions.clear();

      while (!m_freeList.empty()) m_freeList.pop();
    }

    /**
     * @brief Expone el mapa de pools polimórficas (útil para sistemas de serialización).
     * @return Referencia constante al std::unordered_map de pools de componentes.
     */
    [[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>& GetPools() const noexcept 
    {
      return m_componentPools;
    }

  private:
    /**
     * @brief Recupera la pool especializada de tipo T o la crea si no existe previamente.
     * @tparam T Tipo de componente que almacena la pool.
     * @return Puntero a la pool densa ComponentPool<T>.
     */
    template<typename T>
    ComponentPool<T>* GetOrCreatePool() 
    {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      if (it == m_componentPools.end()) 
      {
        auto [newIt, ok] = m_componentPools.emplace(
          typeID, std::make_unique<ComponentPool<T>>());
        return static_cast<ComponentPool<T>*>(newIt->second.get());
      }
      return static_cast<ComponentPool<T>*>(it->second.get());
    }

    /**
     * @brief Intenta localizar la pool especializada del componente T de manera modificable.
     * @tparam T Tipo de componente.
     * @return Puntero a la pool si existe, de lo contrario nullptr.
     */
    template<typename T>
    ComponentPool<T>* GetPool() noexcept 
    {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);
      return (it != m_componentPools.end())
        ? static_cast<ComponentPool<T>*>(it->second.get())
        : nullptr;
    }

    /**
     * @brief Intenta localizar la pool especializada en modo constante de solo lectura.
     * @tparam T Tipo de componente.
     * @return Puntero constante a la pool si existe, de lo contrario nullptr.
     */
    template<typename T>
    const ComponentPool<T>* GetPoolConst() const noexcept 
    {
      const ComponentTypeID typeID = GetComponentTypeID<T>();
      auto it = m_componentPools.find(typeID);

      return (it != m_componentPools.end())
        ? static_cast<const ComponentPool<T>*>(it->second.get())
        : nullptr;
    }

  private:
    std::vector<EntityID>      m_entities;     ///< Registro contiguo de identificadores de entidad.
    std::vector<EntityVersion> m_versions;     ///< Historial incremental de versiones por índice.
    std::queue<EntityIndex>    m_freeList;     ///< Cola de índices reciclados listos para reutilizarse.

    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools; ///< Mapa de pools indexado por tipo de componente.

    std::vector<std::unique_ptr<System>> m_systems; ///< Contenedor secuencial de los sistemas lógicos.
  };
}