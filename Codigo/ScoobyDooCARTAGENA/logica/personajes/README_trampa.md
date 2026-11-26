# ESPECIFICACION TECNICA: trampa.h / trampa.cpp

## Archivo y Ubicacion
`logica/personajes/trampa.h`
`logica/personajes/trampa.cpp`

## Proposito y Responsabilidad Unica
Mecanismo de captura del villano oculto en el escenario. Maneja el proceso sostenido de armado (3.5 segundos con tecla de interaccion) y evalua el ingreso a su zona activa de 5x5 casillas.

## Dependencias
### Para trampa.h
- `#include "personajes/elemento_del_mapa.h"`
- `#include <cstdint>`

### Declaraciones adelantadas en trampa.h
```cpp
class Scooby;
class PersonajeNoJugable;
```

### Para trampa.cpp
- `#include "personajes/trampa.h"`
- `#include "personajes/scooby.h"`
- `#include "personajes/personaje_no_jugable.h"`
- `#include "herramientas/calculos_espaciales.h"`

## Definicion Exacta de Cabecera (trampa.h)

```cpp
#ifndef TRAMPA_H
#define TRAMPA_H

#include "personajes/elemento_del_mapa.h"
#include <cstdint>

class Scooby;
class PersonajeNoJugable;

enum class EstadoDeLaTrampa : uint8_t {
    OcultaSinArmar,
    ArmadaInvisible,
    DisparadaConCaptura
};

class Trampa final : public ElementoDelMapaBase {
public:
    static constexpr double mitadDelLadoDeLaZonaEnCasillas = 2.5;
    static constexpr double segundosNecesariosParaArmar = 3.5;
    static constexpr double distanciaMaximaParaIniciarArmado = 1.5;
    static constexpr double anchoColisionTrampa = 5.0;
    static constexpr double altoColisionTrampa = 5.0;

    Trampa(double posicionInicialX, double posicionInicialY);
    ~Trampa() override = default;

    Trampa(const Trampa&) = delete;
    Trampa& operator=(const Trampa&) = delete;

    void avanzarUnCuadro(double segundosTranscurridos) override;

    EstadoDeLaTrampa obtenerEstadoActual() const;
    double obtenerSegundosAcumuladosParaArmar() const;
    double obtenerProgresoDeArmadoNormalizado() const;
    bool obtenerVillanoCayoEnLaTrampa() const;
    bool obtenerScoobyCayoEnLaTrampa() const;

    bool armarSiScoobyMantieneLaInteraccion(const Scooby& scooby, bool interaccionSostenida, double segundosTranscurridos);
    void revisarSiScoobyOElVillanoPisanLaZona(const Scooby& scooby, const PersonajeNoJugable& villano);

private:
    EstadoDeLaTrampa estadoActual;
    double segundosAcumuladosParaArmar;
    bool villanoCayoEnLaTrampa;
    bool scoobyCayoEnLaTrampa;
};

#endif
```

## Algoritmo Paso a Paso (trampa.cpp)

### Constructor
1. Invocar `ElementoDelMapaBase(posicionInicialX, posicionInicialY, anchoColisionTrampa, altoColisionTrampa)`.
2. Asignar `estadoActual = EstadoDeLaTrampa::OcultaSinArmar; segundosAcumuladosParaArmar = 0.0;`.
3. Asignar `villanoCayoEnLaTrampa = false; scoobyCayoEnLaTrampa = false;`.
4. Asignar `esVisible = true; estaActivo = true;`.

### avanzarUnCuadro(double segundosTranscurridos)
1. Si `estadoActual == EstadoDeLaTrampa::ArmadaInvisible`:
   - `esVisible = false;` (permanece invisible mientras espera a su presa).

### armarSiScoobyMantieneLaInteraccion(const Scooby& scooby, bool interaccionSostenida, double segundosTranscurridos)
1. Si `estadoActual != EstadoDeLaTrampa::OcultaSinArmar`, retornar `false`.
2. Calcular distancias axiales independientes:
   - `distanciaEjeX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, scooby.obtenerPosicionX());`
   - `distanciaEjeY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, scooby.obtenerPosicionY());`
3. Si `distanciaEjeX <= distanciaMaximaParaIniciarArmado && distanciaEjeY <= distanciaMaximaParaIniciarArmado && interaccionSostenida`:
   - `segundosAcumuladosParaArmar += segundosTranscurridos;`
   - Si `segundosAcumuladosParaArmar >= segundosNecesariosParaArmar`:
     - `estadoActual = EstadoDeLaTrampa::ArmadaInvisible;`
     - `esVisible = false;`
     - Retornar `true;`
4. Si no se sostiene la interaccion continua:
   - `segundosAcumuladosParaArmar = 0.0;`
5. Retornar `false`.

### revisarSiScoobyOElVillanoPisanLaZona(const Scooby& scooby, const PersonajeNoJugable& villano)
1. Si `estadoActual != EstadoDeLaTrampa::ArmadaInvisible`, retornar.
2. Evaluar entrada del villano si esta activo en el mapa:
   - `distanciaVillanoX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, villano.obtenerPosicionX());`
   - `distanciaVillanoY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, villano.obtenerPosicionY());`
   - Si `distanciaVillanoX <= mitadDelLadoDeLaZonaEnCasillas && distanciaVillanoY <= mitadDelLadoDeLaZonaEnCasillas`:
     - `villanoCayoEnLaTrampa = true;`
     - `estadoActual = EstadoDeLaTrampa::DisparadaConCaptura;`
     - `esVisible = true;`
     - Retornar;
3. Evaluar entrada accidental de Scooby:
   - `distanciaScoobyX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, scooby.obtenerPosicionX());`
   - `distanciaScoobyY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, scooby.obtenerPosicionY());`
   - Si `distanciaScoobyX <= mitadDelLadoDeLaZonaEnCasillas && distanciaScoobyY <= mitadDelLadoDeLaZonaEnCasillas`:
     - `scoobyCayoEnLaTrampa = true;`
     - `estadoActual = EstadoDeLaTrampa::DisparadaConCaptura;`
     - `esVisible = true;`

### obtenerProgresoDeArmadoNormalizado() const
1. Retornar `CalculosEspaciales::mantenerValorDentroDelRango(segundosAcumuladosParaArmar / segundosNecesariosParaArmar, 0.0, 1.0)`.
