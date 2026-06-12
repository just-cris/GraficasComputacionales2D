#pragma once

namespace ECS 
{

  class Registry;

  /**
   * @class System
   * @brief Clase base abstracta para el procesamiento lógico de entidades y componentes.
   *
   * Los sistemas encapsulan el comportamiento y las reglas de juego del motor,
   * transformando los datos puros almacenados exclusivamente en los componentes.
   */
  class System 
  {
  public:
    /**
     * @brief Destructor virtual por defecto de System.
     */
    virtual ~System() = default;

    /**
     * @brief Ciclo de inicialización del sistema invocado al ser registrado.
     * @param registry Referencia al registro central de la arquitectura ECS.
     */
    virtual void OnStart(Registry& /*registry*/) {}

    /**
     * @brief Método de actualización recurrente ejecutado en cada frame del bucle.
     * @param registry Referencia al registro central de la arquitectura ECS.
     * @param deltaTime Tiempo fraccionario transcurrido desde el último frame.
     */
    virtual void OnUpdate(Registry& registry, float deltaTime) = 0;

    /**
     * @brief Ciclo de desinicialización ejecutado al remover el sistema o limpiar el motor.
     * @param registry Referencia al registro central de la arquitectura ECS.
     */
    virtual void OnDestroy(Registry& /*registry*/) {}

    /**
     * @brief Alterna el estado de activación o ejecución del sistema actual.
     * @param enable Estado lógico deseado (true para activar, false para pausar).
     */
    void SetEnable(bool enable) noexcept { m_enable = enable; }

    /**
     * @brief Evalúa si el sistema se encuentra habilitado para su actualización.
     * @return true si el sistema procesará el frame actual, false en caso contrario.
     */
    [[nodiscard]] bool IsEnable() const noexcept { return m_enable; }

  private:
    bool m_enable = true; ///< Bandera de control de ejecución del sistema.
  };

} // namespace ECS ?