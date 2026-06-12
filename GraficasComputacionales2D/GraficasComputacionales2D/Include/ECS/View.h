#pragma once
#include "ECS/ComponentPool.h"

namespace ESC {
  template<typename... Components>
  class View {
  public:
    explicit View(ComponentPool<Components>&... pools) noexcept
      : m_pool(&pools...) {
      FindSmallest();
    }

    template<typename Func>
    void Each(Func&& func) {
      if (!m_smallest) return;
      const auto& entities = m_smallest->GetEntities();
      for (std::size_t i = entities.size(); i > 0; --i) {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity)) {
          std::apply(
            [&](auto*... pools) {
              func(entity, pools->Get(entity)...);
            },
            m_pools);
        }
      }
    }

    template<typename Func>
    void EachEntity(Func&& func) {
      if (!m_smallest) return;
      const auto& entities = m_smallest->GetEntities();
      for (std::size_t i = entities.size(); i > 0; --i) {
        const EntityID entity = entities[i - 1];
        if (AllHave(entity)) {
          func(entity);
        }
      }
    }

    [[nodiscard]] bool Empty() const noexcept { return !m_smallest || m_smallest->Empty(); }
    [[nodiscard]] size_t Size() const noexcept { return m_smallest ? m_smallest->size() : 0; }

  private:
    template<std::size_t I = 0>
    void FindSmallest() noexcept {
      if constexpr (I < sizeof...(Components)) {
        auto* pool = std::get<I>(m_pool);
        if (pool && (!m_smallest || pool->size() < m_smallest->size())) {
          m_smallest = pool;
        }
        FindSmallest<I + 1>();
      }
    }

    [[nodiscard]] bool AllHave(EntityID entity) const noexcept {
      return std::apply(
        [entity](auto*... pools) {
          return (... && (pools && pools->Contains(entity)));
        },
        m_pools);
    }

  private:
    std::tuple<ComponentPool<Components>*...> m_pool;
    const SparceSet* m_smallest = nullptr;
  };
}