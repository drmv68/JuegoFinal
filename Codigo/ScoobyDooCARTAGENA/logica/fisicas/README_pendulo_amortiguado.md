# ESPECIFICACION TECNICA: pendulo_amortiguado.h / pendulo_amortiguado.cpp

## Archivo de Cabecera: `pendulo_amortiguado.h`

### Guardas de inclusion
```cpp
#ifndef PENDULO_AMORTIGUADO_H
#define PENDULO_AMORTIGUADO_H
```

### Inclusiones requeridas
```cpp
#include "fisicas/movimiento.h"
#include <cmath>
```

### Constantes constexpr
```cpp
namespace ConstantesPendulo {
    constexpr double DOS_PI = 6.28318530717958647692;
    constexpr double UMBRAL_PORCENTUAL_MOMENTO_CLAVE = 0.90;
    constexpr double UMBRAL_REINICIO_AMORTIGUAMIENTO = 0.40;
}
```

### Declaracion de Clase `PenduloAmortiguado`
```cpp
class PenduloAmortiguado : public MovimientoBase {
private:
    double puntoAnclajeX;
    double puntoAnclajeY;
    double longitudCuerdaEnCasillas;
    double amplitudMaximaEnRadianes;
    double frecuenciaOscilacionEnHertz;
    double factorDeAmortiguamiento;
    double anguloActualEnRadianes;

public:
    PenduloAmortiguado();
    void configurarParametrosPendulo(double anclajeX, double anclajeY, double longitud, double amplitudRadianes, double frecuenciaHertz, double amortiguamiento);
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;
    double obtenerAnguloActualEnRadianes() const;
    bool estaEnMomentoClave() const override;
};

#endif
```

---

## Archivo de Implementacion: `pendulo_amortiguado.cpp`

### Inclusiones requeridas
```cpp
#include "fisicas/pendulo_amortiguado.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa todos los atributos numericos en 0.0.

#### 2. `configurarParametrosPendulo`
- Asigna `puntoAnclajeX = anclajeX`, `puntoAnclajeY = anclajeY`, `longitudCuerdaEnCasillas = longitud`, `amplitudMaximaEnRadianes = amplitudRadianes`, `frecuenciaOscilacionEnHertz = frecuenciaHertz`, `factorDeAmortiguamiento = amortiguamiento`.
- Llama a `reiniciarSimulacion()`.

#### 3. `reiniciarSimulacion`
- `tiempoAcumuladoEnSegundos = 0.0;`
- `anguloActualEnRadianes = amplitudMaximaEnRadianes;`
- `estaSimulacionActiva = true;`

#### 4. `avanzarYCalcularPosicion`
- Si `!estaSimulacionActiva` retorna la posicion estatica en reposo.
- `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`
- Calculo de amplitud actual decaida:
  - `double amplitudActual = amplitudMaximaEnRadianes * std::exp(-factorDeAmortiguamiento * tiempoAcumuladoEnSegundos);`
- Si `amplitudActual < (amplitudMaximaEnRadianes * ConstantesPendulo::UMBRAL_REINICIO_AMORTIGUAMIENTO)`:
  - Llama a `reiniciarSimulacion();`
  - `amplitudActual = amplitudMaximaEnRadianes;`
- Calculo de oscilacion:
  - `double faseTemporal = ConstantesPendulo::DOS_PI * frecuenciaOscilacionEnHertz * tiempoAcumuladoEnSegundos;`
  - `anguloActualEnRadianes = amplitudActual * std::cos(faseTemporal);`
- Calculo de posicion fisica:
  - `double desplazamientoHorizontal = longitudCuerdaEnCasillas * std::sin(anguloActualEnRadianes);`
  - `double elevacionVerticalZ = longitudCuerdaEnCasillas * (1.0 - std::cos(anguloActualEnRadianes));`
  - Retorna `CoordenadaFisicaTresEjes{ puntoAnclajeX + desplazamientoHorizontal, puntoAnclajeY, elevacionVerticalZ };`

#### 5. `obtenerAnguloActualEnRadianes`
- Retorna `anguloActualEnRadianes;`

#### 6. `estaEnMomentoClave`
- `double amplitudActual = amplitudMaximaEnRadianes * std::exp(-factorDeAmortiguamiento * tiempoAcumuladoEnSegundos);`
- `double anguloSinSigno = CalculosEspaciales::obtenerValorSinSigno(anguloActualEnRadianes);`
- Retorna `anguloSinSigno >= (amplitudActual * ConstantesPendulo::UMBRAL_PORCENTUAL_MOMENTO_CLAVE);`
