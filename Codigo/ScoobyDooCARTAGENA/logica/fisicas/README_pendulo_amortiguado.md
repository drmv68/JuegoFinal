# ESPECIFICACION TECNICA: pendulo_amortiguado.h / pendulo_amortiguado.cpp

## Archivo y Ubicacion
`logica/fisicas/pendulo_amortiguado.h`
`logica/fisicas/pendulo_amortiguado.cpp`

## Proposito y Responsabilidad Unica
Simula el movimiento armonico simple amortiguado de la lampara de la taberna evaluando la ecuacion analitica exacta en funcion del tiempo continuo, con reposicion continua de energia sin saltos visuales de fase.

## Dependencias
### Para pendulo_amortiguado.h
- `#include "fisicas/movimiento.h"`
- `#include <cmath>`

### Para pendulo_amortiguado.cpp
- `#include "fisicas/pendulo_amortiguado.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <cassert>`

## Definicion Exacta de Cabecera (pendulo_amortiguado.h)

```cpp
#ifndef PENDULO_AMORTIGUADO_H
#define PENDULO_AMORTIGUADO_H

#include "fisicas/movimiento.h"
#include <cmath>

class PenduloAmortiguado final : public MovimientoBase {
public:
    static constexpr double constanteDosPi = 6.28318530717958647692;
    static constexpr double umbralPorcentualMomentoClave = 0.90;
    static constexpr double umbralReinicioAmortiguamiento = 0.40;
    static constexpr double duracionRampaImpulsoEnSegundos = 1.0;

    PenduloAmortiguado();
    ~PenduloAmortiguado() override = default;

    PenduloAmortiguado(const PenduloAmortiguado&) = delete;
    PenduloAmortiguado& operator=(const PenduloAmortiguado&) = delete;

    void configurarParametrosPendulo(double anclajeX, double anclajeY, double longitud, double amplitudRadianes, double frecuenciaHertz, double amortiguamiento);
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;

    double obtenerAnguloActualEnRadianes() const;
    double obtenerAmplitudActualEnRadianes() const;
    bool estaEnMomentoClave() const override;

private:
    double puntoAnclajeX;
    double puntoAnclajeY;
    double longitudCuerdaEnCasillas;
    double amplitudMaximaEnRadianes;
    double frecuenciaOscilacionEnHertz;
    double factorDeAmortiguamiento;
    double anguloActualEnRadianes;
    double amplitudBaseActual;
    double segundosDesdeUltimoImpulso;
};

#endif
```

## Algoritmo Paso a Paso (pendulo_amortiguado.cpp)

### Constructor
1. Inicializar todas las variables numericas en `0.0`.

### configurarParametrosPendulo(double anclajeX, double anclajeY, double longitud, double amplitudRadianes, double frecuenciaHertz, double amortiguamiento)
1. `assert(longitud > 0.0); assert(frecuenciaHertz > 0.0); assert(amortiguamiento >= 0.0);`
2. Asignar `puntoAnclajeX = anclajeX; puntoAnclajeY = anclajeY;`.
3. Asignar `longitudCuerdaEnCasillas = longitud; amplitudMaximaEnRadianes = amplitudRadianes;`.
4. Asignar `frecuenciaOscilacionEnHertz = frecuenciaHertz; factorDeAmortiguamiento = amortiguamiento;`.
5. Invocar `reiniciarSimulacion()`.

### reiniciarSimulacion()
1. Asignar `tiempoAcumuladoEnSegundos = 0.0;`.
2. Asignar `segundosDesdeUltimoImpulso = 0.0;`.
3. Asignar `amplitudBaseActual = amplitudMaximaEnRadianes;`.
4. Asignar `anguloActualEnRadianes = amplitudMaximaEnRadianes;`.
5. Asignar `estaSimulacionActiva = true;`.

### avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior)
1. Si `!estaSimulacionActiva`, retornar `CoordenadaFisicaTresEjes{ puntoAnclajeX, puntoAnclajeY, 0.0 }`.
2. Sumar `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`.
3. Sumar `segundosDesdeUltimoImpulso += segundosDesdeElCuadroAnterior;`.
4. Calcular factor de decaimiento exponencial: `envolventeExponencial = std::exp(-factorDeAmortiguamiento * segundosDesdeUltimoImpulso)`.
5. Si `envolventeExponencial < umbralReinicioAmortiguamiento`:
   - En lugar de resetear el tiempo a cero y provocar un salto en la funcion coseno, reiniciar unicamente la envolvente suave:
   - `segundosDesdeUltimoImpulso = 0.0;`
   - `envolventeExponencial = 1.0;`
6. Calcular la amplitud instantanea: `amplitudInstantanea = amplitudMaximaEnRadianes * envolventeExponencial`.
7. Evaluar oscilacion armonica en funcion del tiempo continuo acumulado total para preservar la continuidad de fase:
   - `faseTemporal = constanteDosPi * frecuenciaOscilacionEnHertz * tiempoAcumuladoEnSegundos;`
   - `anguloActualEnRadianes = amplitudInstantanea * std::cos(faseTemporal);`
8. Calcular proyeccion espacial:
   - `desplazamientoHorizontalX = longitudCuerdaEnCasillas * std::sin(anguloActualEnRadianes);`
   - `alturaRelativaZ = longitudCuerdaEnCasillas * (1.0 - std::cos(anguloActualEnRadianes));`
9. Retornar `CoordenadaFisicaTresEjes{ puntoAnclajeX + desplazamientoHorizontalX, puntoAnclajeY, alturaRelativaZ }`.

### estaEnMomentoClave() const
1. Calcular `amplitudInstantanea = amplitudMaximaEnRadianes * std::exp(-factorDeAmortiguamiento * segundosDesdeUltimoImpulso)`.
2. Calcular el valor absoluto del angulo actual: `anguloSinSigno = std::fabs(anguloActualEnRadianes)`.
3. Retornar `anguloSinSigno >= (amplitudInstantanea * umbralPorcentualMomentoClave)`.
