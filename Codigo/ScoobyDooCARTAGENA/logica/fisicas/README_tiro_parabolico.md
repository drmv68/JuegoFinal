# ESPECIFICACION TECNICA: tiro_parabolico.h / tiro_parabolico.cpp

## Archivo y Ubicacion
`logica/fisicas/tiro_parabolico.h`
`logica/fisicas/tiro_parabolico.cpp`

## Proposito y Responsabilidad Unica
Calcula la balistica analitica de las balas de cañon de la muralla resolviendo de forma exacta la ecuacion de segundo grado en el eje vertical (elevacion y gravedad) y las ecuaciones parametricas lineales en el plano horizontal.

## Dependencias
### Para tiro_parabolico.h
- `#include "fisicas/movimiento.h"`
- `#include <cmath>`

### Para tiro_parabolico.cpp
- `#include "fisicas/tiro_parabolico.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <cassert>`

## Definicion Exacta de Cabecera (tiro_parabolico.h)

```cpp
#ifndef TIRO_PARABOLICO_H
#define TIRO_PARABOLICO_H

#include "fisicas/movimiento.h"
#include <cmath>

class TiroParabolico final : public MovimientoBase {
public:
    static constexpr double gravedadTerrestreSimulada = 9.81;
    static constexpr double alturaSueloNivelZero = 0.0;
    static constexpr double tiempoMinimoDeVueloEnSegundos = 0.10;

    TiroParabolico();
    ~TiroParabolico() override = default;

    TiroParabolico(const TiroParabolico&) = delete;
    TiroParabolico& operator=(const TiroParabolico&) = delete;

    void configurarDisparoDesdeMuralla(double origenX, double origenY, double alturaCanonZ, double velocidad, double anguloElevacionRadianes, double anguloDireccionMapaRadianes, double gravedad);
    void configurarDisparoHaciaUnPunto(double origenX, double origenY, double alturaCanonZ, double destinoX, double destinoY, double segundosDeVuelo, double gravedad);
    void precalcularTiempoYPuntoDeImpacto();
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;

    CoordenadaFisicaTresEjes obtenerCoordenadaDeImpactoFinal() const;
    double obtenerProgresoDeVueloNormalizado() const;
    bool obtenerYaImpactoContraElSuelo() const;
    bool estaEnMomentoClave() const override;

private:
    double posicionDeDisparoX;
    double posicionDeDisparoY;
    double alturaInicialDelCanonZ;
    double velocidadInicialDeDisparo;
    double anguloDeElevacionEnRadianes;
    double anguloDeDireccionEnMapaEnRadianes;
    double gravedadSimulada;
    double tiempoTotalDeVueloCalculado;
    double puntoDeImpactoX;
    double puntoDeImpactoY;
    bool yaImpactoContraElSuelo;
};

#endif
```

## Algoritmo Paso a Paso (tiro_parabolico.cpp)

### Constructor
1. Inicializar todos los atributos numericos en `0.0`, `yaImpactoContraElSuelo = false`.

### configurarDisparoDesdeMuralla(double origenX, double origenY, double alturaCanonZ, double velocidad, double anguloElevacionRadianes, double anguloDireccionMapaRadianes, double gravedad)
1. `assert(velocidad > 0.0); assert(gravedad > 0.0);`
2. Asignar `posicionDeDisparoX = origenX; posicionDeDisparoY = origenY; alturaInicialDelCanonZ = alturaCanonZ;`.
3. Asignar `velocidadInicialDeDisparo = velocidad; anguloDeElevacionEnRadianes = anguloElevacionRadianes;`.
4. Asignar `anguloDeDireccionEnMapaEnRadianes = anguloDireccionMapaRadianes; gravedadSimulada = gravedad;`.
5. Invocar `precalcularTiempoYPuntoDeImpacto()`.
6. Invocar `reiniciarSimulacion()`.

### configurarDisparoHaciaUnPunto(double origenX, double origenY, double alturaCanonZ, double destinoX, double destinoY, double segundosDeVuelo, double gravedad)
1. `assert(segundosDeVuelo >= tiempoMinimoDeVueloEnSegundos); assert(gravedad > 0.0);`
2. Asignar `posicionDeDisparoX = origenX; posicionDeDisparoY = origenY; alturaInicialDelCanonZ = alturaCanonZ;`.
3. Asignar `puntoDeImpactoX = destinoX; puntoDeImpactoY = destinoY;`.
4. Asignar `tiempoTotalDeVueloCalculado = segundosDeVuelo; gravedadSimulada = gravedad;`.
5. Calcular componentes del plano horizontal:
   - `deltaX = destinoX - origenX; deltaY = destinoY - origenY;`
   - `distanciaHorizontal = std::sqrt((deltaX * deltaX) + (deltaY * deltaY));`
   - `anguloDeDireccionEnMapaEnRadianes = std::atan2(deltaY, deltaX);`
   - `velocidadHorizontal = distanciaHorizontal / segundosDeVuelo;`
6. Resolver velocidad vertical analitica requerida para tocar suelo a `t = segundosDeVuelo`:
   - `velocidadVerticalInicial = ((0.5 * gravedad * segundosDeVuelo * segundosDeVuelo) - alturaInicialDelCanonZ) / segundosDeVuelo;`
7. Reconstruir magnitud de velocidad y angulo de elevacion:
   - `velocidadInicialDeDisparo = std::sqrt((velocidadHorizontal * velocidadHorizontal) + (velocidadVerticalInicial * velocidadVerticalInicial));`
   - `anguloDeElevacionEnRadianes = std::atan2(velocidadVerticalInicial, velocidadHorizontal);`
8. Asignar `yaImpactoContraElSuelo = false;`.
9. Invocar `reiniciarSimulacion()`.

### precalcularTiempoYPuntoDeImpacto()
1. Resolver analiticamente `-0.5 * g * t^2 + (v0 * sin(elevacion)) * t + z0 = 0`:
   - `coeficienteA = -0.5 * gravedadSimulada;`
   - `coeficienteB = velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes);`
   - `coeficienteC = alturaInicialDelCanonZ;`
   - `discriminante = (coeficienteB * coeficienteB) - (4.0 * coeficienteA * coeficienteC);`
   - `assert(discriminante >= 0.0);`
   - Tomar la raiz positiva: `tiempoTotalDeVueloCalculado = (-coeficienteB - std::sqrt(discriminante)) / (2.0 * coeficienteA);`
2. Proyectar punto de impacto en tierra:
   - `distanciaHorizontalTotal = velocidadInicialDeDisparo * std::cos(anguloDeElevacionEnRadianes) * tiempoTotalDeVueloCalculado;`
   - `puntoDeImpactoX = posicionDeDisparoX + (distanciaHorizontalTotal * std::cos(anguloDeDireccionEnMapaEnRadianes));`
   - `puntoDeImpactoY = posicionDeDisparoY + (distanciaHorizontalTotal * std::sin(anguloDeDireccionEnMapaEnRadianes));`
3. Asignar `yaImpactoContraElSuelo = false;`.

### reiniciarSimulacion()
1. Asignar `tiempoAcumuladoEnSegundos = 0.0; yaImpactoContraElSuelo = false; estaSimulacionActiva = true;`.

### avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior)
1. Sumar `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`.
2. Si `tiempoAcumuladoEnSegundos >= tiempoTotalDeVueloCalculado`:
   - `tiempoAcumuladoEnSegundos = tiempoTotalDeVueloCalculado;`
   - `yaImpactoContraElSuelo = true; estaSimulacionActiva = false;`
   - Retornar `CoordenadaFisicaTresEjes{ puntoDeImpactoX, puntoDeImpactoY, alturaSueloNivelZero }`.
3. Evaluar ecuaciones continuas exactas en `t = tiempoAcumuladoEnSegundos`:
   - `distanciaHorizontal = velocidadInicialDeDisparo * std::cos(anguloDeElevacionEnRadianes) * t;`
   - `posX = posicionDeDisparoX + (distanciaHorizontal * std::cos(anguloDeDireccionEnMapaEnRadianes));`
   - `posY = posicionDeDisparoY + (distanciaHorizontal * std::sin(anguloDeDireccionEnMapaEnRadianes));`
   - `posZ = alturaInicialDelCanonZ + (velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes) * t) - (0.5 * gravedadSimulada * t * t);`
   - Si `posZ < alturaSueloNivelZero`, asignar `posZ = alturaSueloNivelZero;`.
   - Retornar `CoordenadaFisicaTresEjes{ posX, posY, posZ }`.

### obtenerProgresoDeVueloNormalizado() const
1. Si `tiempoTotalDeVueloCalculado <= 0.0`, retornar 1.0.
2. Retornar `CalculosEspaciales::mantenerValorDentroDelRango(tiempoAcumuladoEnSegundos / tiempoTotalDeVueloCalculado, 0.0, 1.0)`.

### estaEnMomentoClave() const
1. Retornar `yaImpactoContraElSuelo`.
