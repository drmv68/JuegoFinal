# ESPECIFICACION TECNICA: personaje_no_jugable.h / personaje_no_jugable.cpp

## Archivo y Ubicacion
`logica/personajes/personaje_no_jugable.h`
`logica/personajes/personaje_no_jugable.cpp`

## Proposito y Responsabilidad Unica
Representa a ciudadanos, amigos de la pandilla, centinelas, secuaces y al villano colonial. Es dueño de su cerebro de inteligencia artificial (`CerebroBase*`) y delega su comportamiento mediante composicion limpia.

## Dependencias
### Para personaje_no_jugable.h
- `#include "personajes/elemento_del_mapa.h"`
- `#include "herramientas/azar_con_semilla.h"`
- `#include <QString>`
- `#include <cstdint>`

### Declaraciones adelantadas en personaje_no_jugable.h
```cpp
class Scooby;
class CerebroBase;
class MovimientoBase;
class Mapa;
struct BuzonCompartidoDeAvisos;
```

### Para personaje_no_jugable.cpp
- `#include "personajes/personaje_no_jugable.h"`
- `#include "personajes/scooby.h"`
- `#include "inteligencia/cerebro.h"`
- `#include "fisicas/movimiento.h"`
- `#include "mapa/mapa.h"`
- `#include "inteligencia/avisos_entre_agentes.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <cmath>`
- `#include <cassert>`

## Definicion Exacta de Cabecera (personaje_no_jugable.h)

```cpp
#ifndef PERSONAJE_NO_JUGABLE_H
#define PERSONAJE_NO_JUGABLE_H

#include "personajes/elemento_del_mapa.h"
#include "herramientas/azar_con_semilla.h"
#include <QString>
#include <cstdint>

class Scooby;
class CerebroBase;
class MovimientoBase;
class Mapa;
struct BuzonCompartidoDeAvisos;

enum class TipoRolDelPersonaje : uint8_t {
    CiudadanoInformativo,
    AmigoVelma,
    AmigoFred,
    AmigoShaggy,
    AmigoDaphne,
    SecuazVigilante,
    CentinelaAlmacen,
    VillanoFantasma
};

enum class IconoSobreLaCabeza : uint8_t {
    Ninguno,
    SignoDeInterrogacion,
    SignoDeExclamacion,
    AuraDeFuria
};

enum class ServicioDeAmigo : uint8_t {
    Ninguno,
    SoloConversacion,
    CambiarGemaPorProvisiones,
    RecibirEvidenciasDeVelma,
    DarIndicioSutil,
    OcultarAScooby
};

class PersonajeNoJugable final : public ElementoDelMapaBase {
public:
    static constexpr double anchoColisionPersonaje = 0.70;
    static constexpr double altoColisionPersonaje = 0.70;
    static constexpr double velocidadBasePersonajeNoJugable = 3.0;
    static constexpr double distanciaToleranciaLlegada = 0.05;

    PersonajeNoJugable(TipoRolDelPersonaje rolAsignado, double posicionInicialX, double posicionInicialY);
    ~PersonajeNoJugable() override;

    PersonajeNoJugable(const PersonajeNoJugable&) = delete;
    PersonajeNoJugable& operator=(const PersonajeNoJugable&) = delete;

    void avanzarUnCuadro(double segundosTranscurridos) override;
    void pensarYActuar(const Scooby& scooby, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos);

    void asignarCerebro(CerebroBase* nuevoCerebroDueno);
    void asignarMovimientoDeRecorrido(MovimientoBase* nuevoMovimientoDueno);

    TipoRolDelPersonaje obtenerRol() const;
    const CerebroBase* obtenerCerebroObservador() const;
    CerebroBase* modificarCerebro();

    IconoSobreLaCabeza obtenerIconoSobreLaCabeza() const;
    void establecerIconoSobreLaCabeza(IconoSobreLaCabeza icono);

    ServicioDeAmigo obtenerServicioDisponible() const;
    void establecerServicioDisponible(ServicioDeAmigo servicio);

    double obtenerVelocidadActualEnCasillasPorSegundo() const;
    void establecerVelocidadActualEnCasillasPorSegundo(double velocidad);

    CoordenadaCasilla obtenerDestinoActual() const;
    void establecerDestinoActual(CoordenadaCasilla destino);

    QString obtenerClaveDeDialogo() const;
    void establecerClaveDeDialogo(const QString& clave);

    void moverHacia(double destinoX, double destinoY, const Mapa& mapa, double segundosTranscurridos);
    void actualizarPresenciaPorMomentoDelDia(bool esDeNoche, const RectanguloVision& vistaDeScooby, const Mapa& mapa, AzarConSemilla& azar);
    void activarEstadoDePanico();

private:
    TipoRolDelPersonaje rol;
    CerebroBase* cerebro;
    MovimientoBase* movimientoDeRecorrido;
    IconoSobreLaCabeza iconoSobreLaCabeza;
    ServicioDeAmigo servicioDisponible;
    double velocidadActualEnCasillasPorSegundo;
    CoordenadaCasilla destinoActual;
    QString claveDeDialogo;
};

#endif
```

## Algoritmo Paso a Paso (personaje_no_jugable.cpp)

### Constructor
1. Invocar `ElementoDelMapaBase(posicionInicialX, posicionInicialY, anchoColisionPersonaje, altoColisionPersonaje)`.
2. Asignar `rol = rolAsignado; cerebro = nullptr; movimientoDeRecorrido = nullptr;`.
3. Asignar `iconoSobreLaCabeza = IconoSobreLaCabeza::Ninguno;`.
4. Asignar `velocidadActualEnCasillasPorSegundo = velocidadBasePersonajeNoJugable;`.
5. Asignar `destinoActual = obtenerCasillaActual();`.
6. Configurar servicio de amigo segun rol:
   - `AmigoVelma`: `servicioDisponible = ServicioDeAmigo::RecibirEvidenciasDeVelma;`
   - `AmigoFred`: `servicioDisponible = ServicioDeAmigo::CambiarGemaPorProvisiones;`
   - `AmigoShaggy`: `servicioDisponible = ServicioDeAmigo::DarIndicioSutil;`
   - `AmigoDaphne`: `servicioDisponible = ServicioDeAmigo::OcultarAScooby;`
   - Otros roles: `servicioDisponible = ServicioDeAmigo::Ninguno;`
7. Si `rol == TipoRolDelPersonaje::VillanoFantasma`:
   - `estaActivo = false; esVisible = false;` (nace durmiendo de dia fuera del mapa).

### Destructor
1. Si `cerebro != nullptr`: `delete cerebro; cerebro = nullptr;`
2. Si `movimientoDeRecorrido != nullptr`: `delete movimientoDeRecorrido; movimientoDeRecorrido = nullptr;`

### avanzarUnCuadro(double segundosTranscurridos)
1. Si `!estaActivo`, terminar.
2. Si `movimientoDeRecorrido != nullptr`:
   - `CoordenadaFisicaTresEjes posicion = movimientoDeRecorrido->avanzarYCalcularPosicion(segundosTranscurridos);`
   - `posicionX = posicion.posicionX; posicionY = posicion.posicionY;`

### pensarYActuar(const Scooby& scooby, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos)
1. Si `!estaActivo`, terminar.
2. Si `cerebro != nullptr`:
   - `cerebro->actualizarCerebroEnCuadro(*this, scooby, mapa, buzon, azar, segundosTranscurridos);`

### moverHacia(double destinoX, double destinoY, const Mapa& mapa, double segundosTranscurridos)
1. `deltaX = destinoX - posicionX; deltaY = destinoY - posicionY;`
2. `distancia = std::sqrt((deltaX * deltaX) + (deltaY * deltaY));`
3. Si `distancia < distanciaToleranciaLlegada`, terminar.
4. `dirX = deltaX / distancia; dirY = deltaY / distancia;`
5. Actualizar `direccionMirada`:
   - Si `std::fabs(dirX) >= std::fabs(dirY)`: `direccionMirada = (dirX > 0.0) ? DireccionMirada::Este : DireccionMirada::Oeste;`
   - En caso contrario: `direccionMirada = (dirY > 0.0) ? DireccionMirada::Sur : DireccionMirada::Norte;`
6. `paso = velocidadActualEnCasillasPorSegundo * segundosTranscurridos;`
7. `puedeEntrarAZonasRestringidas = (rol == TipoRolDelPersonaje::SecuazVigilante || rol == TipoRolDelPersonaje::VillanoFantasma);`
8. Validar por ejes separados:
   - `nuevaX = posicionX + (dirX * paso);`
   - Si `mapa.esPosicionTransitable(nuevaX, posicionY, puedeEntrarAZonasRestringidas)`, asignar `posicionX = nuevaX;`.
   - `nuevaY = posicionY + (dirY * paso);`
   - Si `mapa.esPosicionTransitable(posicionX, nuevaY, puedeEntrarAZonasRestringidas)`, asignar `posicionY = nuevaY;`.

### actualizarPresenciaPorMomentoDelDia
1. Si `rol != TipoRolDelPersonaje::VillanoFantasma`, terminar.
2. Si `!esDeNoche`:
   - `estaActivo = false; esVisible = false;`
3. Si `esDeNoche && !estaActivo`:
   - Obtener lista de casillas aptas para trampa o transitables del mapa.
   - Barajar lista con `azar.barajarListaDeCasillas`.
   - Buscar la primera casilla `casilla` que no este contenida dentro de `vistaDeScooby`.
   - Asignar `posicionX = casilla.columna + 0.5; posicionY = casilla.fila + 0.5;`.
   - `estaActivo = true; esVisible = true;`
