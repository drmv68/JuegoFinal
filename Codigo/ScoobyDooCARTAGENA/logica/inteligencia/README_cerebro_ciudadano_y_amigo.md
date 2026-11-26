# ESPECIFICACION TECNICA: cerebro_ciudadano_y_amigo.h / cerebro_ciudadano_y_amigo.cpp

## Archivo y Ubicacion
`logica/inteligencia/cerebro_ciudadano_y_amigo.h`
`logica/inteligencia/cerebro_ciudadano_y_amigo.cpp`

## Proposito y Responsabilidad Unica
Implementacion liviana de inteligencia para ciudadanos de la ciudad amurallada y los amigos de Scooby (Velma, Fred, Shaggy, Daphne). Gira la mirada hacia el jugador y provee servicios sin trazado de linea de vista ni busqueda de caminos A*.

## Dependencias
### Para cerebro_ciudadano_y_amigo.h
- `#include "inteligencia/cerebro.h"`

### Para cerebro_ciudadano_y_amigo.cpp
- `#include "inteligencia/cerebro_ciudadano_y_amigo.h"`
- `#include "personajes/personaje_no_jugable.h"`
- `#include "personajes/scooby.h"`
- `#include "herramientas/calculos_espaciales.h"`

## Tabla de Transicion de Estados
| Estado Actual | Estimulo / Condicion | Estado Siguiente | Accion en la Transicion |
|---|---|---|---|
| `PatrullandoSector` | Scooby a menos de 2.5 casillas | `PausaDeVigilancia` | Detener caminata y mirar de frente a Scooby |
| `PausaDeVigilancia` | Scooby se aleja a mas de 2.5 casillas | `PatrullandoSector` | Reanudar patrulla corta |

## Definicion Exacta de Cabecera (cerebro_ciudadano_y_amigo.h)

```cpp
#ifndef CEREBRO_CIUDADANO_Y_AMIGO_H
#define CEREBRO_CIUDADANO_Y_AMIGO_H

#include "inteligencia/cerebro.h"

class CerebroCiudadanoYAmigo final : public CerebroBase {
public:
    static constexpr double radioDeteccionSaludoEnCasillas = 3.5;
    static constexpr double distanciaAtencionConversacionEnCasillas = 2.5;
    static constexpr double velocidadPaseoCiudadano = 1.5;

    CerebroCiudadanoYAmigo(double desfaseInicial, bool caminar);
    ~CerebroCiudadanoYAmigo() override = default;

    CerebroCiudadanoYAmigo(const CerebroCiudadanoYAmigo&) = delete;
    CerebroCiudadanoYAmigo& operator=(const CerebroCiudadanoYAmigo&) = delete;

protected:
    EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) override;
    void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) override;
    void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) override;
    void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) override;

private:
    double radioDeDeteccionDeSaludoEnCasillas;
    bool puedeCaminarPorRutaCorta;
};

#endif
```

## Algoritmo Paso a Paso (cerebro_ciudadano_y_amigo.cpp)

### Constructor
1. Invocar `CerebroBase(desfaseInicial, radioDeteccionSaludoEnCasillas, velocidadPaseoCiudadano, velocidadPaseoCiudadano)`.
2. Asignar `radioDeDeteccionDeSaludoEnCasillas = radioDeteccionSaludoEnCasillas; puedeCaminarPorRutaCorta = caminar;`.

### percibirEntorno
1. Calcular distancia al cuadrado hacia Scooby mediante `CalculosEspaciales::calcularDistanciaAlCuadrado(cuerpo.obtenerPosicionX(), cuerpo.obtenerPosicionY(), scooby.obtenerPosicionX(), scooby.obtenerPosicionY())`.
2. `umbralCuadrado = radioDeDeteccionDeSaludoEnCasillas * radioDeDeteccionDeSaludoEnCasillas;`
3. Retornar `EstimuloPercibido{ distanciaCuadrado <= umbralCuadrado, false, distanciaCuadrado, scooby.obtenerCasillaActual(), false, CoordenadaCasilla{0, 0} }`.

### razonarSiguienteDecision
1. `umbralConversacionCuadrado = distanciaAtencionConversacionEnCasillas * distanciaAtencionConversacionEnCasillas;`
2. Si `estimulo.distanciaAlCuadradoHaciaScooby <= umbralConversacionCuadrado`:
   - `estadoMentalActual = EstadoMentalDelAgente::PausaDeVigilancia;`
3. Si no:
   - `estadoMentalActual = EstadoMentalDelAgente::PatrullandoSector;`

### ejecutarMovimientoOAccion
1. Si `estadoMentalActual == EstadoMentalDelAgente::PausaDeVigilancia`:
   - Orientar mirada directamente hacia Scooby:
     - `deltaX = scooby.obtenerPosicionX() - cuerpo.obtenerPosicionX();`
     - `deltaY = scooby.obtenerPosicionY() - cuerpo.obtenerPosicionY();`
     - Si `std::fabs(deltaX) >= std::fabs(deltaY)`: `cuerpo.establecerDireccionMirada((deltaX > 0.0) ? DireccionMirada::Este : DireccionMirada::Oeste);`
     - En caso contrario: `cuerpo.establecerDireccionMirada((deltaY > 0.0) ? DireccionMirada::Sur : DireccionMirada::Norte);`
2. Si `puedeCaminarPorRutaCorta && estadoMentalActual == EstadoMentalDelAgente::PatrullandoSector`:
   - Si `memoriaPropia.tieneRutaFijaDefinida()`:
     - `CoordenadaCasilla destino = memoriaPropia.obtenerPuntoDePatrullaObjetivoActual();`
     - `cuerpo.moverHacia(destino.columna + 0.5, destino.fila + 0.5, mapa, segundosTranscurridos);`
     - Si `CalculosEspaciales::calcularDistanciaAlCuadrado(cuerpo.obtenerPosicionX(), cuerpo.obtenerPosicionY(), destino.columna + 0.5, destino.fila + 0.5) < 0.04`:
       - `memoriaPropia.avanzarAlSiguientePuntoDePatrulla();`

### actualizarMemoriaYAprendizaje
1. `memoriaPropia.descontarEnfriamientoDeServicio(intervaloEntreCiclosDePensamientoEnSegundos);`
