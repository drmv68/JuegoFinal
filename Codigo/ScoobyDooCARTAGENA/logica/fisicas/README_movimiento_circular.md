# ESPECIFICACION TECNICA: movimiento_circular.h / movimiento_circular.cpp

## Archivo y Ubicacion
`logica/fisicas/movimiento_circular.h`
`logica/fisicas/movimiento_circular.cpp`

## Proposito y Responsabilidad Unica
Calcula la trayectoria circular parametrica del centinela del almacen de provisiones mediante ecuaciones trigonometricas continuas, administrando pausas de observacion, interpolacion suave de radio y acotamiento de parametros para impedir derivas infinitas.

## Dependencias
### Para movimiento_circular.h
- `#include "fisicas/movimiento.h"`
- `#include "herramientas/azar_con_semilla.h"`

### Para movimiento_circular.cpp
- `#include "fisicas/movimiento_circular.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <cmath>`
- `#include <cassert>`

## Definicion Exacta de Cabecera (movimiento_circular.h)

```cpp
#ifndef MOVIMIENTO_CIRCULAR_H
#define MOVIMIENTO_CIRCULAR_H

#include "fisicas/movimiento.h"
#include "herramientas/azar_con_semilla.h"

class MovimientoCircular final : public MovimientoBase {
public:
    static constexpr double duracionPausaDeCambioEnSegundos = 0.8;
    static constexpr double tiempoMinimoEntreCambiosEnSegundos = 6.0;
    static constexpr double tiempoMaximoEntreCambiosEnSegundos = 12.0;
    static constexpr double factorVariacionVelocidadAngular = 0.10;
    static constexpr double incrementoRadioEnCasillas = 0.40;
    static constexpr double radioMinimoPermitidoEnCasillas = 1.5;
    static constexpr double radioMaximoPermitidoEnCasillas = 5.0;
    static constexpr double velocidadAngularMinimaPermitida = 0.5;
    static constexpr double velocidadAngularMaximaPermitida = 2.0;

    MovimientoCircular();
    ~MovimientoCircular() override = default;

    MovimientoCircular(const MovimientoCircular&) = delete;
    MovimientoCircular& operator=(const MovimientoCircular&) = delete;

    void configurarParametrosCirculares(double centroX, double centroY, double radioInicial, double velocidadAngular);
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;
    void variarUnSoloParametroDePatrulla(AzarConSemilla& azar);

    double obtenerRadioActualEnCasillas() const;
    double obtenerVelocidadAngularEnRadianesPorSegundo() const;
    bool obtenerEstaEnPausaDeCambio() const;

private:
    double centroDeGiroX;
    double centroDeGiroY;
    double radioActualEnCasillas;
    double radioObjetivoEnCasillas;
    double velocidadAngularEnRadianesPorSegundo;
    double faseInicialEnRadianes;
    int sentidoDeGiro;
    double segundosParaElProximoCambio;
    double segundosRestantesDePausa;
    bool estaEnPausaDeCambio;
};

#endif
```

## Algoritmo Paso a Paso (movimiento_circular.cpp)

### Constructor
1. Inicializar atributos numericos en `0.0`, `sentidoDeGiro = 1`, `estaEnPausaDeCambio = false`.

### configurarParametrosCirculares(double centroX, double centroY, double radioInicial, double velocidadAngular)
1. `assert(radioInicial >= radioMinimoPermitidoEnCasillas); assert(velocidadAngular > 0.0);`
2. Asignar `centroDeGiroX = centroX; centroDeGiroY = centroY;`.
3. Asignar `radioActualEnCasillas = radioInicial; radioObjetivoEnCasillas = radioInicial;`.
4. Asignar `velocidadAngularEnRadianesPorSegundo = velocidadAngular;`.
5. Asignar `faseInicialEnRadianes = 0.0; sentidoDeGiro = 1;`.
6. Asignar `segundosParaElProximoCambio = tiempoMinimoEntreCambiosEnSegundos;`.
7. Asignar `segundosRestantesDePausa = 0.0; estaEnPausaDeCambio = false;`.
8. Invocar `reiniciarSimulacion()`.

### reiniciarSimulacion()
1. Asignar `tiempoAcumuladoEnSegundos = 0.0; estaSimulacionActiva = true;`.

### avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior)
1. Si `estaEnPausaDeCambio`:
   - Descontar `segundosRestantesDePausa -= segundosDesdeElCuadroAnterior;`.
   - Interpolar suavemente el radio hacia `radioObjetivoEnCasillas` proporcional al tiempo de pausa transcurrido.
   - Si `segundosRestantesDePausa <= 0.0`:
     - `estaEnPausaDeCambio = false;`
     - `segundosRestantesDePausa = 0.0;`
     - `radioActualEnCasillas = radioObjetivoEnCasillas;`
   - Retornar la coordenada fija actual sin avanzar el tiempo angular acumulado.
2. Si no esta en pausa:
   - Sumar `tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;`.
   - Descontar `segundosParaElProximoCambio -= segundosDesdeElCuadroAnterior;`.
   - Calcular angulo instantaneo: `anguloActual = faseInicialEnRadianes + (static_cast<double>(sentidoDeGiro) * velocidadAngularEnRadianesPorSegundo * tiempoAcumuladoEnSegundos)`.
   - Calcular `posicionX = centroDeGiroX + (radioActualEnCasillas * std::cos(anguloActual))`.
   - Calcular `posicionY = centroDeGiroY + (radioActualEnCasillas * std::sin(anguloActual))`.
   - Retornar `CoordenadaFisicaTresEjes{ posicionX, posicionY, 0.0 }`.

### variarUnSoloParametroDePatrulla(AzarConSemilla& azar)
1. Calcular y congelar el angulo actual exacto: `anguloActual = faseInicialEnRadianes + (static_cast<double>(sentidoDeGiro) * velocidadAngularEnRadianesPorSegundo * tiempoAcumuladoEnSegundos)`.
2. Asignar `faseInicialEnRadianes = anguloActual; tiempoAcumuladoEnSegundos = 0.0;`.
3. Activar pausa: `estaEnPausaDeCambio = true; segundosRestantesDePausa = duracionPausaDeCambioEnSegundos;`.
4. Sortear tiempo para el proximo cambio: `segundosParaElProximoCambio = azar.generarDecimalEnRango(tiempoMinimoEntreCambiosEnSegundos, tiempoMaximoEntreCambiosEnSegundos);`.
5. Sortear una sola variable entre tres opciones (0, 1 o 2):
   - Opcion 0 (Modificar radio):
     - `variacion = azar.generarDecimalEnRango(-incrementoRadioEnCasillas, incrementoRadioEnCasillas);`
     - `radioObjetivoEnCasillas = CalculosEspaciales::mantenerValorDentroDelRango(radioActualEnCasillas + variacion, radioMinimoPermitidoEnCasillas, radioMaximoPermitidoEnCasillas);`
   - Opcion 1 (Invertir sentido de giro):
     - `sentidoDeGiro = -sentidoDeGiro;`
   - Opcion 2 (Ajustar velocidad angular):
     - `multiplicador = azar.generarDecimalEnRango(1.0 - factorVariacionVelocidadAngular, 1.0 + factorVariacionVelocidadAngular);`
     - `velocidadAngularEnRadianesPorSegundo = CalculosEspaciales::mantenerValorDentroDelRango(velocidadAngularEnRadianesPorSegundo * multiplicador, velocidadAngularMinimaPermitida, velocidadAngularMaximaPermitida);`
