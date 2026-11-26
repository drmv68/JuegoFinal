# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: MISIÓN BASE

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase abstracta `MisionBase` define el contrato polimórfico de las misiones que componen los casos de investigación en Cartagena. Representa una meta intermedia que el jugador debe completar para obtener pistas, avanzar el diálogo con los ciudadanos o desbloquear áreas del mapa. No contiene dependencias gráficas y se comunica con el estado del juego mediante referencias a `MundoDeCartagena` y eventos de dominio.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "herramientas/tipos_espaciales.h"
#include <string>
#include <cstdint>

class MundoDeCartagena;
```

## 3. ENUMERACIONES Y TIPOS AUXILIARES
```cpp
enum class TipoDeMision : uint8_t {
    HablarConPersonaje = 0,
    RecolectarYEntregarObjeto = 1,
    ExaminarZonaOElemento = 2,
    SincronizacionConPendulo = 3
};

enum class EstadoDeMision : uint8_t {
    NoIniciada = 0,
    EnProgreso = 1,
    CompletadaConExito = 2,
    Fallida = 3
};
```

## 4. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class MisionBase {
public:
    MisionBase(
        int identificadorUnico,
        TipoDeMision tipo,
        std::string tituloDescriptivo,
        std::string descripcionDetallada,
        int recompensaDePuntaje
    );

    virtual ~MisionBase() = default;

    virtual void iniciar(MundoDeCartagena& mundo) = 0;
    virtual void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos) = 0;
    virtual bool verificarCondicionDeCompletitud(MundoDeCartagena& mundo) = 0;
    virtual void completar(MundoDeCartagena& mundo);
    virtual void reiniciar() = 0;

    int obtenerIdentificadorUnico() const;
    TipoDeMision obtenerTipoDeMision() const;
    EstadoDeMision obtenerEstadoDeMision() const;
    const std::string& obtenerTituloDescriptivo() const;
    const std::string& obtenerDescripcionDetallada() const;
    int obtenerRecompensaDePuntaje() const;

    bool estaCompletada() const;
    bool estaEnProgreso() const;

protected:
    void cambiarEstado(EstadoDeMision nuevoEstado);

    int identificadorUnico;
    TipoDeMision tipo;
    EstadoDeMision estado;
    std::string tituloDescriptivo;
    std::string descripcionDetallada;
    int recompensaDePuntaje;
};
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. El `identificadorUnico` debe ser estrictamente positivo (> 0).
2. La transición de estados debe seguir un flujo unidireccional por instancia:
   - `NoIniciada` -> `EnProgreso` -> `CompletadaConExito` o `Fallida`.
3. `completar(MundoDeCartagena& mundo)` marca el estado como `CompletadaConExito` y garantiza que no se procese doble recompensa en `Caso`.
4. Los métodos no asignan memoria dinámica en cada ciclo de actualización.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `MisionBase::MisionBase`
Asigna los miembros por lista de inicialización. Inicializa `estado` en `EstadoDeMision::NoIniciada`.

### `MisionBase::completar`
1. Verifica que `estado == EstadoDeMision::EnProgreso`. Si no lo está, retorna de inmediato.
2. Invoca `cambiarEstado(EstadoDeMision::CompletadaConExito)`.
3. Notifica al mundo o registra la finalización en el historial del caso correspondiente.

### `MisionBase::cambiarEstado`
Asigna el nuevo estado protegiendo la variable de transiciones inválidas.
