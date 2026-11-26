# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: CEREBRO SECUAZ

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `CerebroSecuaz` hereda de `CerebroBase` e implementa la inteligencia artificial de los secuaces patrulleros y guardianes. Su objetivo es patrullar zonas asignadas, detectar a Scooby mediante un cono de visión o proximidad sonora, confirmar sospechas tras una latencia deliberada de reacción (0.4 segundos), emitir avisos acústicos al sistema global de alarmas y perseguir al jugador mediante el algoritmo de caminos mínimos A*. Si pierde el rastro, inspecciona la última posición conocida y regresa metódicamente a su ruta de patrulla.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "inteligencia/cerebro.h"
#include <vector>
#include <cstdint>
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class CerebroSecuaz : public CerebroBase {
public:
    explicit CerebroSecuaz(int identificadorDeRutaInicial);
    ~CerebroSecuaz() override = default;

    void actualizar(
        PersonajeNoJugable& personajePortador,
        MundoDeCartagena& mundo,
        float segundosTranscurridos
    ) override;

    void reiniciar() override;

    void agregarPuntoDePatrulla(const CoordenadaEntera& punto);
    void vaciarPuntosDePatrulla();

private:
    void ejecutarPatrulla(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarSospecha(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarPersecucion(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarBusquedaEnUltimoRastro(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);

    void avanzarHaciaPuntoObjetivo(PersonajeNoJugable& personajePortador, const CoordenadaEntera& objetivo, float velocidad, float segundosTranscurridos);

    std::vector<CoordenadaEntera> puntosDePatrulla;
    int indiceDePatrullaActual;
    int identificadorDeRutaAsignada;

    float temporizadorDeSospechaAcumulado;
    float temporizadorDeInspeccionEnRastro;
    float temporizadorDeRecalculoDeCaminoAEstrella;

    bool avisoDeAlertaEmitido;
    CoordenadaEntera ultimaPosicionVisualConfirmada;
};
```

## 4. CONSTANTES DE COMPORTAMIENTO
```cpp
namespace ConstantesCerebroSecuaz {
    inline constexpr float TIEMPO_REQUERIDO_DE_CONFIRMACION_DE_SOSPECHA = 0.40f;
    inline constexpr float TIEMPO_DE_INSPECCION_EN_ULTIMO_RASTRO = 3.50f;
    inline constexpr float INTERVALO_DE_RECALCULO_A_ESTRELLA = 0.50f;
    inline constexpr float DISTANCIA_DE_LLEGADA_A_PUNTO_CUADRADA = 16.0f;
    inline constexpr float VELOCIDAD_DE_PATRULLA = 50.0f;
    inline constexpr float VELOCIDAD_DE_PERSECUCION = 95.0f;
    inline constexpr float DISTANCIA_MAXIMA_DE_VISION = 160.0f;
    inline constexpr float ANGULO_DE_VISION_RADIANES = 1.308996939f;
}
```

## 5. TABLA DE TRANSICIÓN DE ESTADOS
1. `Patrullando`:
   - Si detecta a Scooby en cono de visión y Scooby no está camuflado: transiciona a `SospechandoConDuda`. Reinicia `temporizadorDeSospechaAcumulado = 0.0f`.
   - Si recibe un aviso acústico relevante de `AvisosEntreAgentes`: orienta la vista hacia la coordenada del aviso y transiciona a `SospechandoConDuda`.
2. `SospechandoConDuda`:
   - Incrementa `temporizadorDeSospechaAcumulado += segundosTranscurridos`.
   - Si Scooby sigue visible y `temporizadorDeSospechaAcumulado >= TIEMPO_REQUERIDO_DE_CONFIRMACION_DE_SOSPECHA`:
     - Emite aviso en `mundo.obtenerAvisosEntreAgentes().registrarAviso(TipoDeAvisoAcustico::ScoobyAvistado, ...)`.
     - Transiciona a `Persiguiendo`.
     - Solicita camino A* a la posición de Scooby.
   - Si Scooby desaparece del cono antes de completar el tiempo: transiciona a `Patrullando`.
3. `Persiguiendo`:
   - Recalcula camino A* cada 0.5 segundos acumulados.
   - Si Scooby es visible: actualiza `ultimaPosicionVisualConfirmada` y consume nodos de la ruta A* para desplazarse a `VELOCIDAD_DE_PERSECUCION`.
   - Si Scooby no es visible y la ruta A* se agota o llega al último nodo: transiciona a `BuscandoEnUltimoRastro` e inicia `temporizadorDeInspeccionEnRastro = 0.0f`.
4. `BuscandoEnUltimoRastro`:
   - Incrementa `temporizadorDeInspeccionEnRastro += segundosTranscurridos`.
   - Rota la orientación cada 1.0 segundo para simular búsqueda en el entorno.
   - Si detecta a Scooby: transiciona a `Persiguiendo`.
   - Si `temporizadorDeInspeccionEnRastro >= TIEMPO_DE_INSPECCION_EN_ULTIMO_RASTRO`: transiciona a `Patrullando`, recalculando camino A* hacia el punto de patrulla más cercano.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `actualizar`
1. Descuenta `acumuladorDeTiempoTranscurrido` de la clase base según el intervalo configurado.
2. Consulta el estado actual de la máquina de estados.
3. Invoca el método privado correspondiente según el estado.
4. Sincroniza la orientación calculada con `personajePortador.establecerOrientacion()`.
5. Actualiza la coordenada lógica y visual del personaje portador.

### `avanzarHaciaPuntoObjetivo`
1. Obtiene la posición flotante actual del personaje portador.
2. Calcula el vector hacia el centro de la casilla objetivo `(objetivo.x * TAMANO_CASILLA + MITAD, objetivo.y * TAMANO_CASILLA + MITAD)`.
3. Calcula la distancia euclidiana al cuadrado.
4. Si la distancia al cuadrado es menor a `DISTANCIA_DE_LLEGADA_A_PUNTO_CUADRADA`, da por alcanzado el nodo y avanza el índice del camino A*.
5. Si no, normaliza la dirección y aplica el desplazamiento escalado por la velocidad y `segundosTranscurridos`.
6. Verifica colisiones con `mundo.obtenerMapa().esCasillaSolida()`. Si hay colisión, detiene el avance frontal y fuerza el recalculo de ruta en el siguiente ciclo.
