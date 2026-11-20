# ESPECIFICACION TECNICA: tiro_parabolico.h / tiro_parabolico.cpp

## Archivo de Cabecera: `tiro_parabolico.h`

### Guardas de inclusion
```cpp
#ifndef TIRO_PARABOLICO_H
#define TIRO_PARABOLICO_H
```

### Inclusiones requeridas
```cpp
#include "fisicas/movimiento.h"
#include <cmath>
```

### Declaracion de Clase `TiroParabolico`
```cpp
class TiroParabolico : public MovimientoBase {
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

public:
    TiroParabolico();
    void configurarDisparoDesdeMuralla(double origenX, double origenY, double alturaCanonZ, double velocidad, double anguloElevacionRadianes, double anguloDireccionMapaRadianes, double gravedad);
    void configurarDisparoHaciaUnPunto(double origenX, double origenY, double alturaCanonZ, double destinoX, double destinoY, double segundosDeVuelo, double gravedad);
    void precalcularTiempoYPuntoDeImpacto();
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;
    CoordenadaFisicaTresEjes obtenerCoordenadaDeImpactoFinal() const;
    double obtenerProgresoDeVueloNormalizado() const;
    bool obtenerYaImpactoContraElSuelo() const;
    bool estaEnMomentoClave() const override;
};

#endif
```

---

## Archivo de Implementacion: `tiro_parabolico.cpp`

### Inclusiones requeridas
```cpp
#include "fisicas/tiro_parabolico.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa variables en 0.0, `yaImpactoContraElSuelo = false`.

#### 2. `configurarDisparoDesdeMuralla`
- `posicionDeDisparoX = origenX; posicionDeDisparoY = origenY; alturaInicialDelCanonZ = alturaCanonZ;`
- `velocidadInicialDeDisparo = velocidad;`
- `anguloDeElevacionEnRadianes = anguloElevacionRadianes;`
- `anguloDeDireccionEnMapaEnRadianes = anguloDireccionMapaRadianes;`
- `gravedadSimulada = gravedad;`
- Llama a `precalcularTiempoYPuntoDeImpacto();`
- Llama a `reiniciarSimulacion();`

#### 3. `configurarDisparoHaciaUnPunto`
- `posicionDeDisparoX = origenX; posicionDeDisparoY = origenY; alturaInicialDelCanonZ = alturaCanonZ;`
- `puntoDeImpactoX = destinoX; puntoDeImpactoY = destinoY;`
- `tiempoTotalDeVueloCalculado = segundosDeVuelo; gravedadSimulada = gravedad;`
- `double deltaX = destinoX - origenX; double deltaY = destinoY - origenY;`
- `double distanciaHorizontal = std::sqrt(deltaX * deltaX + deltaY * deltaY);`
- `anguloDeDireccionEnMapaEnRadianes = std::atan2(deltaY, deltaX);`
- `double velocidadHorizontal = distanciaHorizontal / segundosDeVuelo;`
- `double velocidadVerticalInicial = (0.5 * gravedad * segundosDeVuelo * segundosDeVuelo - alturaInicialDelCanonZ) / segundosDeVuelo;`
- `velocidadInicialDeDisparo = std::sqrt(velocidadHorizontal * velocidadHorizontal + velocidadVerticalInicial * velocidadVerticalInicial);`
- `anguloDeElevacionEnRadianes = std::atan2(velocidadVerticalInicial, velocidadHorizontal);`
- `yaImpactoContraElSuelo = false;`
- Llama a `reiniciarSimulacion();`

#### 4. `precalcularTiempoYPuntoDeImpacto`
- Resuelve ecuacion de segundo grado para altura z(t) = 0:
  - `-0.5 * gravedad * t^2 + (v0 * sin(elevacion)) * t + z0 = 0`
  - `a = -0.5 * gravedadSimulada;`
  - `b = velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes);`
  - `c = alturaInicialDelCanonZ;`
  - `discriminante = b * b - 4.0 * a * c;`
  - `t1 = (-b - std::sqrt(discriminante)) / (2.0 * a);`
  - `tiempoTotalDeVueloCalculado = t1;`
- `double distanciaHorizontalTotal = velocidadInicialDeDisparo * std::cos(anguloDeElevacionEnRadianes) * tiempoTotalDeVueloCalculado;`
- `puntoDeImpactoX = posicionDeDisparoX + distanciaHorizontalTotal * std::cos(anguloDeDireccionEnMapaEnRadianes);`
- `puntoDeImpactoY = posicionDeDisparoY + distanciaHorizontalTotal * std::sin(anguloDeDireccionEnMapaEnRadianes);`
- `yaImpactoContraElSuelo = false;`

#### 5. `reiniciarSimulacion`
- `tiempoAcumuladoEnSegundos = 0.0; yaImpactoContraElSuelo = false; estaSimulacionActiva = true;`

#### 6. `avanzarYCalcularPosicion`
- `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`
- Si `tiempoAcumuladoEnSegundos >= tiempoTotalDeVueloCalculado`:
  - `tiempoAcumuladoEnSegundos = tiempoTotalDeVueloCalculado;`
  - `yaImpactoContraElSuelo = true; estaSimulacionActiva = false;`
  - Retorna `CoordenadaFisicaTresEjes{ puntoDeImpactoX, puntoDeImpactoY, 0.0 };`
- Calculo parametrico de posicion:
  - `double t = tiempoAcumuladoEnSegundos;`
  - `double distanciaRecorridaHorizontal = velocidadInicialDeDisparo * std::cos(anguloDeElevacionEnRadianes) * t;`
  - `double posX = posicionDeDisparoX + distanciaRecorridaHorizontal * std::cos(anguloDeDireccionEnMapaEnRadianes);`
  - `double posY = posicionDeDisparoY + distanciaRecorridaHorizontal * std::sin(anguloDeDireccionEnMapaEnRadianes);`
  - `double posZ = alturaInicialDelCanonZ + (velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes) * t) - (0.5 * gravedadSimulada * t * t);`
  - Si `posZ < 0.0` se fija en 0.0.
  - Retorna `CoordenadaFisicaTresEjes{ posX, posY, posZ };`

#### 7. `obtenerProgresoDeVueloNormalizado`
- Si `tiempoTotalDeVueloCalculado <= 0.0` retorna 1.0.
- Retorna `CalculosEspaciales::mantenerValorDentroDelRango(tiempoAcumuladoEnSegundos / tiempoTotalDeVueloCalculado, 0.0, 1.0);`

#### 8. `estaEnMomentoClave`
- Retorna `yaImpactoContraElSuelo;`
