# ESPECIFICACION TECNICA: movimiento_circular.h / movimiento_circular.cpp

## Archivo de Cabecera: `movimiento_circular.h`

### Guardas de inclusion
```cpp
#ifndef MOVIMIENTO_CIRCULAR_H
#define MOVIMIENTO_CIRCULAR_H
```

### Inclusiones requeridas
```cpp
#include "fisicas/movimiento.h"
#include "herramientas/azar_con_semilla.h"
```

### Constantes constexpr
```cpp
namespace ConstantesCircular {
    constexpr double DURACION_PAUSA_CAMBIO_EN_SEGUNDOS = 0.8;
    constexpr double TIEMPO_MINIMO_ENTRE_CAMBIOS_EN_SEGUNDOS = 6.0;
    constexpr double TIEMPO_MAXIMO_ENTRE_CAMBIOS_EN_SEGUNDOS = 12.0;
    constexpr double FACTOR_VARIACION_VELOCIDAD = 0.10;
    constexpr double FACTOR_VARIACION_RADIO = 0.50;
}
```

### Declaracion de Clase `MovimientoCircular`
```cpp
class MovimientoCircular : public MovimientoBase {
private:
    double centroDeGiroX;
    double centroDeGiroY;
    double radioActualEnCasillas;
    double velocidadAngularEnRadianesPorSegundo;
    double faseInicialEnRadianes;
    int sentidoDeGiro;
    double segundosParaElProximoCambio;
    double segundosRestantesDePausa;
    bool estaEnPausaDeCambio;

public:
    MovimientoCircular();
    void configurarParametrosCirculares(double centroX, double centroY, double radioInicial, double velocidadAngular);
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;
    void variarUnSoloParametroDePatrulla(AzarConSemilla& azar);

    double obtenerRadioActualEnCasillas() const;
    double obtenerVelocidadAngularEnRadianesPorSegundo() const;
    bool obtenerEstaEnPausaDeCambio() const;
};

#endif
```

---

## Archivo de Implementacion: `movimiento_circular.cpp`

### Inclusiones requeridas
```cpp
#include "fisicas/movimiento_circular.h"
#include <cmath>
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa variables en 0.0, `sentidoDeGiro = 1`, `estaEnPausaDeCambio = false`.

#### 2. `configurarParametrosCirculares`
- `centroDeGiroX = centroX; centroDeGiroY = centroY;`
- `radioActualEnCasillas = radioInicial;`
- `velocidadAngularEnRadianesPorSegundo = velocidadAngular;`
- `faseInicialEnRadianes = 0.0; sentidoDeGiro = 1;`
- `segundosParaElProximoCambio = ConstantesCircular::TIEMPO_MINIMO_ENTRE_CAMBIOS_EN_SEGUNDOS;`
- `segundosRestantesDePausa = 0.0; estaEnPausaDeCambio = false;`
- Llama a `reiniciarSimulacion()`.

#### 3. `reiniciarSimulacion`
- `tiempoAcumuladoEnSegundos = 0.0;`
- `estaSimulacionActiva = true;`

#### 4. `avanzarYCalcularPosicion`
- Si `estaEnPausaDeCambio`:
  - `segundosRestantesDePausa -= segundosDesdeElCuadroAnterior;`
  - Si `segundosRestantesDePausa <= 0.0`:
    - `estaEnPausaDeCambio = false;`
    - `segundosRestantesDePausa = 0.0;`
  - En pausa, el tiempo de movimiento no avanza.
- Si no esta en pausa:
  - `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`
  - `segundosParaElProximoCambio -= segundosDesdeElCuadroAnterior;`
- Calculo de angulo actual:
  - `double anguloActual = faseInicialEnRadianes + (sentidoDeGiro * velocidadAngularEnRadianesPorSegundo * tiempoAcumuladoEnSegundos);`
  - `double posicionCalculadaX = centroDeGiroX + radioActualEnCasillas * std::cos(anguloActual);`
  - `double posicionCalculadaY = centroDeGiroY + radioActualEnCasillas * std::sin(anguloActual);`
  - Retorna `CoordenadaFisicaTresEjes{ posicionCalculadaX, posicionCalculadaY, 0.0 };`

#### 5. `variarUnSoloParametroDePatrulla`
- Congela posicion angular actual:
  - `double anguloActual = faseInicialEnRadianes + (sentidoDeGiro * velocidadAngularEnRadianesPorSegundo * tiempoAcumuladoEnSegundos);`
  - `faseInicialEnRadianes = anguloActual;`
  - `tiempoAcumuladoEnSegundos = 0.0;`
- Entra en pausa:
  - `estaEnPausaDeCambio = true;`
  - `segundosRestantesDePausa = ConstantesCircular::DURACION_PAUSA_CAMBIO_EN_SEGUNDOS;`
  - `segundosParaElProximoCambio = azar.generarDecimalEnRango(ConstantesCircular::TIEMPO_MINIMO_ENTRE_CAMBIOS_EN_SEGUNDOS, ConstantesCircular::TIEMPO_MAXIMO_ENTRE_CAMBIOS_EN_SEGUNDOS);`
- Sortea 1 variable entre 3 (indice 0, 1 o 2):
  - Caso 0 (cambiar radio):
    - `radioActualEnCasillas += azar.generarDecimalEnRango(-ConstantesCircular::FACTOR_VARIACION_RADIO, ConstantesCircular::FACTOR_VARIACION_RADIO);`
    - Si `radioActualEnCasillas < 1.5` se ajusta a 1.5.
  - Caso 1 (invertir sentido):
    - `sentidoDeGiro = -sentidoDeGiro;`
  - Caso 2 (ajustar velocidad angular):
    - `double multiplicador = azar.generarDecimalEnRango(1.0 - ConstantesCircular::FACTOR_VARIACION_VELOCIDAD, 1.0 + ConstantesCircular::FACTOR_VARIACION_VELOCIDAD);`
    - `velocidadAngularEnRadianesPorSegundo *= multiplicador;`
