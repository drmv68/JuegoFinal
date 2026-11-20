# ESPECIFICACION TECNICA: cerebro.h / cerebro.cpp

## Archivo de Cabecera: `cerebro.h`

### Guardas de inclusion
```cpp
#ifndef CEREBRO_H
#define CEREBRO_H
```

### Inclusiones requeridas
```cpp
#include "inteligencia/memoria_del_agente.h"
#include "inteligencia/avisos_entre_agentes.h"
#include "herramientas/azar_con_semilla.h"
#include "mapa/mapa.h"
```

### Declaraciones Adelantadas
```cpp
class PersonajeNoJugable;
class Scooby;
```

### Enumeraciones y Estructuras
```cpp
enum class EstadoMentalDelAgente {
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
```

### Declaracion de Clase Madre Abstracta `CerebroBase`
```cpp
class CerebroBase {
protected:
    EstadoMentalDelAgente estadoMentalActual;
    MemoriaDelAgente memoriaPropia;
    double segundosAcumuladosParaPensar;
    double intervaloEntrePensamientos;
    double segundosRestantesEnElEstadoActual;
    double rangoDeVisionEnCasillas;
    double velocidadNormalDePatrulla;
    double velocidadDePersecucion;

public:
    CerebroBase(double desfaseInicial, double rangoVision, double velPatrulla, double velPersecucion);
    virtual ~CerebroBase() = default;

    void actualizarCerebroEnCuadro(PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos);

    EstadoMentalDelAgente obtenerEstadoMentalActual() const;
    void establecerEstadoMentalActual(EstadoMentalDelAgente nuevoEstado);

    MemoriaDelAgente& obtenerMemoriaPropia();
    const MemoriaDelAgente& obtenerMemoriaPropia() const;

protected:
    virtual EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) = 0;
    virtual void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) = 0;
    virtual void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) = 0;
    virtual void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) = 0;
};

#endif
```

---

## Archivo de Implementacion: `cerebro.cpp`

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro.h"
#include "personajes/personaje_no_jugable.h"
#include "personajes/scooby.h"
```

### Implementacion de Metodos

#### 1. Constructor
- `estadoMentalActual = EstadoMentalDelAgente::PatrullandoSector;`
- `segundosAcumuladosParaPensar = desfaseInicial;`
- `intervaloEntrePensamientos = 0.15;`
- `segundosRestantesEnElEstadoActual = 0.0;`
- `rangoDeVisionEnCasillas = rangoVision;`
- `velocidadNormalDePatrulla = velPatrulla;`
- `velocidadDePersecucion = velPersecucion;`

#### 2. `actualizarCerebroEnCuadro`
- Descuenta `segundosRestantesEnElEstadoActual -= segundosTranscurridos;`
- Suma acumulador: `segundosAcumuladosParaPensar += segundosTranscurridos;`
- Si `segundosAcumuladosParaPensar >= intervaloEntrePensamientos`:
  - `segundosAcumuladosParaPensar = 0.0;`
  - Ejecuta cascada de pensamiento:
    1. `EstimuloPercibido estimulo = percibirEntorno(cuerpo, scooby, mapa, buzon);`
    2. `razonarSiguienteDecision(estimulo, cuerpo, mapa, buzon, azar, intervaloEntrePensamientos);`
    3. `actualizarMemoriaYAprendizaje(estimulo, cuerpo);`
- En todos los cuadros (a 60 fps continuos) ejecuta:
  - `ejecutarMovimientoOAccion(cuerpo, mapa, azar, segundosTranscurridos);`
