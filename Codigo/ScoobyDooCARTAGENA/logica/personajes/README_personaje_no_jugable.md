# ESPECIFICACION TECNICA: personaje_no_jugable.h / personaje_no_jugable.cpp

## Archivo de Cabecera: `personaje_no_jugable.h`

### Guardas de inclusion
```cpp
#ifndef PERSONAJE_NO_JUGABLE_H
#define PERSONAJE_NO_JUGABLE_H
```

### Inclusiones requeridas
```cpp
#include "personajes/elemento_del_mapa.h"
#include "inteligencia/cerebro.h"
#include "fisicas/movimiento.h"
#include "herramientas/azar_con_semilla.h"
#include <QString>
```

### Declaraciones Adelantadas (Forward Declarations)
```cpp
class Scooby;
class CerebroBase;
struct BuzonCompartidoDeAvisos;
```

### Enumeraciones
```cpp
enum class TipoRolDelPersonaje {
    CiudadanoInformativo,
    AmigoVelma,
    AmigoFred,
    AmigoShaggy,
    AmigoDaphne,
    SecuazVigilante,
    CentinelaAlmacen,
    VillanoFantasma
};

enum class IconoSobreLaCabeza {
    Ninguno,
    SignoDeInterrogacion,
    SignoDeExclamacion,
    AuraDeFuria
};

enum class ServicioDeAmigo {
    Ninguno,
    SoloConversacion,
    CambiarGemaPorProvisiones,
    RecibirEvidenciasDeVelma,
    DarIndicioSutil,
    OcultarAScooby
};
```

### Constantes constexpr
```cpp
namespace ConstantesPNJ {
    constexpr double ANCHO_COLISION_PNJ = 0.70;
    constexpr double ALTO_COLISION_PNJ = 0.70;
    constexpr double VELOCIDAD_BASE_PNJ = 3.0;
}
```

### Declaracion de Clase `PersonajeNoJugable`
```cpp
class PersonajeNoJugable : public ElementoDelMapaBase {
private:
    TipoRolDelPersonaje rol;
    CerebroBase* cerebro;
    MovimientoBase* movimientoDeRecorrido;
    IconoSobreLaCabeza iconoSobreLaCabeza;
    ServicioDeAmigo servicioDisponible;
    double velocidadActualEnCasillasPorSegundo;
    CoordenadaCasilla destinoActual;
    QString claveDeDialogo;

public:
    PersonajeNoJugable(TipoRolDelPersonaje rolAsignado, double posX, double posY);
    ~PersonajeNoJugable() override;

    void avanzarUnCuadro(double segundosTranscurridos) override;
    void pensarYActuar(const Scooby& scooby, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos);

    void asignarCerebro(CerebroBase* nuevoCerebro);
    void asignarMovimientoDeRecorrido(MovimientoBase* nuevoMovimiento);

    TipoRolDelPersonaje obtenerRol() const;
    CerebroBase* obtenerCerebro() const;
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
};

#endif
```

---

## Archivo de Implementacion: `personaje_no_jugable.cpp`

### Inclusiones requeridas
```cpp
#include "personajes/personaje_no_jugable.h"
#include "personajes/scooby.h"
#include "inteligencia/cerebro.h"
#include "inteligencia/avisos_entre_agentes.h"
#include "herramientas/calculos_espaciales.h"
#include <cmath>
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `ElementoDelMapaBase(posX, posY, ConstantesPNJ::ANCHO_COLISION_PNJ, ConstantesPNJ::ALTO_COLISION_PNJ)`.
- `rol = rolAsignado; cerebro = nullptr; movimientoDeRecorrido = nullptr;`
- `iconoSobreLaCabeza = IconoSobreLaCabeza::Ninguno;`
- `velocidadActualEnCasillasPorSegundo = ConstantesPNJ::VELOCIDAD_BASE_PNJ;`
- `destinoActual = obtenerCasillaActual();`
- Asignacion de servicio inicial segun rol:
  - `AmigoVelma`: `ServicioDeAmigo::RecibirEvidenciasDeVelma;`
  - `AmigoFred`: `ServicioDeAmigo::CambiarGemaPorProvisiones;`
  - `AmigoShaggy`: `ServicioDeAmigo::DarIndicioSutil;`
  - `AmigoDaphne`: `ServicioDeAmigo::OcultarAScooby;`
  - Otros: `ServicioDeAmigo::Ninguno;`
- Si `rol == TipoRolDelPersonaje::VillanoFantasma`: `estaActivo = false; esVisible = false;` (inicia durmiendo de dia).

#### 2. Destructor
- Libera punteros dueños:
  - `if (cerebro != nullptr) { delete cerebro; cerebro = nullptr; }`
  - `if (movimientoDeRecorrido != nullptr) { delete movimientoDeRecorrido; movimientoDeRecorrido = nullptr; }`

#### 3. `avanzarUnCuadro`
- Si `!estaActivo` retorna.
- Si `movimientoDeRecorrido != nullptr`:
  - `CoordenadaFisicaTresEjes pos = movimientoDeRecorrido->avanzarYCalcularPosicion(segundosTranscurridos);`
  - `posicionX = pos.posicionX; posicionY = pos.posicionY;`

#### 4. `pensarYActuar`
- Si `!estaActivo` retorna.
- Si `cerebro != nullptr`:
  - `cerebro->actualizarCerebroEnCuadro(*this, scooby, mapa, buzon, azar, segundosTranscurridos);`

#### 5. `moverHacia`
- `double deltaX = destinoX - posicionX;`
- `double deltaY = destinoY - posicionY;`
- `double distancia = std::sqrt(deltaX * deltaX + deltaY * deltaY);`
- Si `distancia < 0.05` retorna.
- `double dirX = deltaX / distancia;`
- `double dirY = deltaY / distancia;`
- Actualiza `direccionMirada` segun componente dominante.
- Desplazamiento por ejes:
  - `double paso = velocidadActualEnCasillasPorSegundo * segundosTranscurridos;`
  - `double nuevaX = posicionX + dirX * paso;`
  - `bool puedeEntrarARestringidas = (rol == TipoRolDelPersonaje::SecuazVigilante || rol == TipoRolDelPersonaje::VillanoFantasma);`
  - Si `mapa.esPosicionTransitable(nuevaX, posicionY, puedeEntrarARestringidas)`: `posicionX = nuevaX;`
  - `double nuevaY = posicionY + dirY * paso;`
  - Si `mapa.esPosicionTransitable(posicionX, nuevaY, puedeEntrarARestringidas)`: `posicionY = nuevaY;`

#### 6. `actualizarPresenciaPorMomentoDelDia`
- Si `rol == TipoRolDelPersonaje::VillanoFantasma`:
  - Si `!esDeNoche`:
    - `estaActivo = false; esVisible = false;`
  - Si `esDeNoche && !estaActivo`:
    - Sortea casillas transitables fuera de `vistaDeScooby` usando `azar.barajarListaDeCasillas`.
    - Ubica al villano en la primera casilla transitable que no este dentro del rectangulo de vision de Scooby.
    - `estaActivo = true; esVisible = true;`

#### 7. `activarEstadoDePanico`
- Si `cerebro != nullptr`:
  - Notifica al cerebro pasar a `EstadoMentalDelAgente::HuyendoEnPanico`.
