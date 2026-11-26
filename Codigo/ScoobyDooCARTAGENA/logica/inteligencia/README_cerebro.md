# ESPECIFICACION TECNICA: cerebro.h / cerebro.cpp

## Archivo y Ubicacion
`logica/inteligencia/cerebro.h`
`logica/inteligencia/cerebro.cpp`

## Proposito y Responsabilidad Unica
Clase madre abstracta de la inteligencia artificial descentralizada. Orquesta el patron de cuatro fases (percibir -> razonar -> actuar -> aprender) ejecutando razonamiento cada 0.15 s sin desfases temporales y movimiento suave cada cuadro.

## Dependencias
### Para cerebro.h
- `#include "inteligencia/memoria_del_agente.h"`
- `#include "inteligencia/avisos_entre_agentes.h"`
- `#include "herramientas/azar_con_semilla.h"`
- `#include "mapa/mapa.h"`
- `#include <cstdint>`

### Declaraciones adelantadas en cerebro.h
```cpp
class PersonajeNoJugable;
class Scooby;
```

### Para cerebro.cpp
- `#include "inteligencia/cerebro.h"`
- `#include "personajes/personaje_no_jugable.h"`
- `#include "personajes/scooby.h"`

## Definicion Exacta de Cabecera (cerebro.h)

```cpp
#ifndef CEREBRO_H
#define CEREBRO_H

#include "inteligencia/memoria_del_agente.h"
#include "inteligencia/avisos_entre_agentes.h"
#include "herramientas/azar_con_semilla.h"
#include "mapa/mapa.h"
#include <cstdint>

class PersonajeNoJugable;
class Scooby;

enum class EstadoMentalDelAgente : uint8_t {
    InactivoDurmiendo,
    PatrullandoSector,
    PausaDeVigilancia,
    SospechandoConDuda,
    PersiguiendoObjetivo,
    BuscandoEnUltimaPosicion,
    HuyendoEnPanico,
    PreparandoFuria,
    FuriaDeContraataque
};

struct EstimuloPercibido {
    bool veAScooby;
    bool scoobyEstaEnZonaRestringida;
    double distanciaAlCuadradoHaciaScooby;
    CoordenadaCasilla casillaDondeLoVio;
    bool recibioAvisoDeAlerta;
    CoordenadaCasilla casillaDelAvisoRecibido;
};

class CerebroBase {
public:
    static constexpr double intervaloEntreCiclosDePensamientoEnSegundos = 0.15;

    CerebroBase(double desfaseInicial, double rangoVision, double velPatrulla, double velPersecucion);
    virtual ~CerebroBase() = default;

    CerebroBase(const CerebroBase&) = delete;
    CerebroBase& operator=(const CerebroBase&) = delete;

    void actualizarCerebroEnCuadro(PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos);

    EstadoMentalDelAgente obtenerEstadoMentalActual() const;
    void establecerEstadoMentalActual(EstadoMentalDelAgente nuevoEstado);

    const MemoriaDelAgente& obtenerMemoriaPropia() const;
    MemoriaDelAgente& modificarMemoriaPropia();

protected:
    virtual EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) = 0;
    virtual void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) = 0;
    virtual void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) = 0;
    virtual void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) = 0;

    EstadoMentalDelAgente estadoMentalActual;
    MemoriaDelAgente memoriaPropia;
    double segundosAcumuladosParaPensar;
    double segundosRestantesEnElEstadoActual;
    double rangoDeVisionEnCasillas;
    double velocidadNormalDePatrulla;
    double velocidadDePersecucion;
};

#endif
```

## Algoritmo Paso a Paso (cerebro.cpp)

### Constructor
1. Asignar `estadoMentalActual = EstadoMentalDelAgente::PatrullandoSector;`.
2. Asignar `segundosAcumuladosParaPensar = desfaseInicial;` (para que los guardias no razonen al mismo milisegundo).
3. Asignar `segundosRestantesEnElEstadoActual = 0.0;`.
4. Asignar `rangoDeVisionEnCasillas = rangoVision;`.
5. Asignar `velocidadNormalDePatrulla = velPatrulla; velocidadDePersecucion = velPersecucion;`.

### actualizarCerebroEnCuadro
1. Si `segundosRestantesEnElEstadoActual > 0.0`:
   - `segundosRestantesEnElEstadoActual -= segundosTranscurridos;`
   - Si `segundosRestantesEnElEstadoActual < 0.0`, asignar `segundosRestantesEnElEstadoActual = 0.0;`.
2. Sumar acumulador: `segundosAcumuladosParaPensar += segundosTranscurridos;`.
3. Si `segundosAcumuladosParaPensar >= intervaloEntreCiclosDePensamientoEnSegundos`:
   - Descontar el intervalo exacto para no acumular error residual de tiempo:
     - `segundosAcumuladosParaPensar -= intervaloEntreCiclosDePensamientoEnSegundos;`
   - Ejecutar cascada de 3 fases pesadas pasando el delta de tiempo acumulado real:
     1. `EstimuloPercibido estimulo = percibirEntorno(cuerpo, scooby, mapa, buzon);`
     2. `razonarSiguienteDecision(estimulo, cuerpo, mapa, buzon, azar, intervaloEntreCiclosDePensamientoEnSegundos);`
     3. `actualizarMemoriaYAprendizaje(estimulo, cuerpo);`
4. Ejecutar la fase de movimiento a 60 FPS continuos en cada cuadro:
   - `ejecutarMovimientoOAccion(cuerpo, scooby, mapa, azar, segundosTranscurridos);`
