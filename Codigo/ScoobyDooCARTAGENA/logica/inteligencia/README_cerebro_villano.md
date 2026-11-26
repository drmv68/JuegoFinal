# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: CEREBRO VILLANO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `CerebroVillano` hereda de `CerebroBase` y modela el comportamiento del antagonista principal en las fases nocturnas o de confrontación directa. Opera como un depredador calculador: patrulla áreas críticas, reacciona a avisos de sus secuaces, persigue incansablemente a Scooby aumentando su velocidad progresivamente, y ante la presencia de trampas armadas, evalúa el riesgo o cae en ellas si está en estado de furia. Puede solicitar al mundo el bloqueo temporal de accesos mediante eventos visuales/espaciales sin mutar directamente la geometría del mapa.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "inteligencia/cerebro.h"
#include <vector>
#include <cstdint>
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class CerebroVillano : public CerebroBase {
public:
    explicit CerebroVillano(int identificadorDeZonaDeAcecho);
    ~CerebroVillano() override = default;

    void actualizar(
        PersonajeNoJugable& personajePortador,
        MundoDeCartagena& mundo,
        float segundosTranscurridos
    ) override;

    void reiniciar() override;

    void agregarPuntoDeAcecho(const CoordenadaEntera& punto);
    void activarModoFuria();
    void desactivarModoFuria();

    bool estaEnModoFuria() const;

private:
    void ejecutarAcecho(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarPersecucionImplacable(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarEmboscada(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);
    void ejecutarAturdimiento(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float segundosTranscurridos);

    void avanzarSiguiendoCamino(PersonajeNoJugable& personajePortador, MundoDeCartagena& mundo, float velocidad, float segundosTranscurridos);

    std::vector<CoordenadaEntera> puntosDeAcecho;
    int indiceDeAcechoActual;
    int identificadorDeZona;

    float temporizadorDeFuriaRestante;
    float temporizadorDeAturdimientoRestante;
    float temporizadorDeRecalculoDeRuta;
    float acumuladorDeVelocidadIncremental;

    bool furiaActiva;
    CoordenadaEntera coordenadaDeEmboscadaObjetivo;
};
```

## 4. CONSTANTES DE COMPORTAMIENTO
```cpp
namespace ConstantesCerebroVillano {
    inline constexpr float VELOCIDAD_BASE_DE_ACECHO = 60.0f;
    inline constexpr float VELOCIDAD_BASE_DE_PERSECUCION = 105.0f;
    inline constexpr float VELOCIDAD_EN_MODO_FURIA = 135.0f;
    inline constexpr float ACELERACION_POR_SEGUNDO_EN_PERSECUCION = 4.0f;
    inline constexpr float LIMITE_SUPERIOR_VELOCIDAD_PERSECUCION = 145.0f;
    inline constexpr float DURACION_TOTAL_DE_FURIA = 8.0f;
    inline constexpr float DURACION_DE_ATURDIMIENTO_POR_TRAMPA = 5.0f;
    inline constexpr float DISTANCIA_MAXIMA_DETECCION_NOCTURNA = 200.0f;
    inline constexpr float INTERVALO_RECALCULO_RUTA_PERSECUCION = 0.35f;
    inline constexpr float DISTANCIA_LLEGADA_A_NODO_CUADRADA = 20.0f;
}
```

## 5. TABLA DE TRANSICIÓN DE ESTADOS
1. `Acechando`:
   - El villano recorre sus puntos de acecho a velocidad moderada.
   - Si detecta visualmente a Scooby en radio de 200 unidades: emite rugido de persecución y transiciona a `Persiguiendo`.
   - Si un secuaz emite un aviso de tipo `ScoobyAvistado`: calcula la posición proyectada del jugador e intenta transicionar a `Emboscando`.
2. `Emboscando`:
   - El villano navega hacia un punto estratégico cercano a la ruta de escape de Scooby en lugar de su posición directa.
   - Si entra en contacto visual con Scooby: transiciona inmediatamente a `Persiguiendo`.
   - Si llega al punto de emboscada y no halla a Scooby: transiciona a `Acechando`.
3. `Persiguiendo`:
   - Navega consumiendo nodos A* hacia la posición exacta de Scooby recalculando cada 0.35 segundos.
   - Incrementa progresivamente la velocidad con `ACELERACION_POR_SEGUNDO_EN_PERSECUCION`.
   - Si pisa una casilla ocupada por una `Trampa` armada:
     - La trampa se activa y consume su estado.
     - El villano transiciona a `Aturdido` por `DURACION_DE_ATURDIMIENTO_POR_TRAMPA` segundos.
     - Se reinicia el acumulador de velocidad.
   - Si Scooby escapa de su radio por más de 6 segundos continuos: transiciona a `Acechando`.
4. `Aturdido`:
   - Permanece inmóvil. La velocidad efectiva es cero.
   - Decrementa `temporizadorDeAturdimientoRestante`.
   - Al expirar el tiempo, transiciona a `Acechando` o entra en `ModoFuria` temporal si la partida está en dificultad avanzada.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `actualizar`
1. Verifica si el personaje portador o el cerebro están congelados por evento cinematográfico; de ser así, retorna de inmediato.
2. Si `furiaActiva` es verdadero:
   - Decrementa `temporizadorDeFuriaRestante -= segundosTranscurridos`.
   - Si expira el tiempo, invoca `desactivarModoFuria()`.
3. Evalúa el estado interno (`Acechando`, `Persiguiendo`, `Emboscando`, `Aturdido`) y ejecuta la subrutina correspondiente.
4. Ajusta la dirección de la animación y orientación según el vector de movimiento resultante.
5. Gestiona la proximidad crítica: si la distancia a Scooby es menor a 24 unidades y Scooby no está protegido por una trampa, invoca la notificación de captura al controlador de juego.
