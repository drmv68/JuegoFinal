# ESPECIFICACION TECNICA: calculos_espaciales.h / calculos_espaciales.cpp

## Archivo y Ubicacion
`logica/herramientas/calculos_espaciales.h`
`logica/herramientas/calculos_espaciales.cpp`

## Proposito y Responsabilidad Unica
Provee funciones matematicas puras, acotamientos y calculos geometricos vectoriales en la cuadricula logica 2D sin realizar asignaciones dinamicas.

## Dependencias
### Para calculos_espaciales.h
- `#include <cmath>`
- `#include <algorithm>`
- `#include "herramientas/tipos_espaciales.h"`

### Para calculos_espaciales.cpp
- `#include "herramientas/calculos_espaciales.h"`

## Definicion Exacta de Cabecera (calculos_espaciales.h)

```cpp
#ifndef CALCULOS_ESPACIALES_H
#define CALCULOS_ESPACIALES_H

#include <cmath>
#include <algorithm>
#include "herramientas/tipos_espaciales.h"

namespace CalculosEspaciales {
    constexpr double constantePi = 3.14159265358979323846;
    constexpr double constanteDosPi = 6.28318530717958647692;
    constexpr double constanteMedioPi = 1.57079632679489661923;
    constexpr double toleranciaDistanciaCero = 0.000001;

    inline double mantenerValorDentroDelRango(double valorActual, double valorMinimoPermitido, double valorMaximoPermitido) {
        return std::clamp(valorActual, valorMinimoPermitido, valorMaximoPermitido);
    }

    inline double obtenerValorSinSigno(double valor) {
        return std::fabs(valor);
    }

    inline double calcularDistanciaAlCuadrado(double origenX, double origenY, double destinoX, double destinoY) {
        double diferenciaX = destinoX - origenX;
        double diferenciaY = destinoY - origenY;
        return (diferenciaX * diferenciaX) + (diferenciaY * diferenciaY);
    }

    inline double calcularDistanciaEnUnEje(double coordenadaA, double coordenadaB) {
        return std::fabs(coordenadaA - coordenadaB);
    }

    CoordenadaCasilla convertirPosicionACasilla(double posicionX, double posicionY);
    double calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY, double destinoX, double destinoY);
    bool estaPuntoDentroDelConoDeVision(double origenX, double origenY, DireccionMirada direccionMirada, double objetivoX, double objetivoY, double aperturaDelConoEnRadianes);
    bool estaPuntoDentroDelRectangulo(double puntoX, double puntoY, const RectanguloVision& rectangulo);
}

#endif
```

## Algoritmo Paso a Paso (calculos_espaciales.cpp)

### convertirPosicionACasilla(double posicionX, double posicionY)
1. Obtener la componente entera inferior de `posicionX` mediante `std::floor(posicionX)`.
2. Convertir el resultado a `int` y asignarlo a `columnaCasilla`.
3. Obtener la componente entera inferior de `posicionY` mediante `std::floor(posicionY)`.
4. Convertir el resultado a `int` y asignarlo a `filaCasilla`.
5. Retornar `CoordenadaCasilla{ columnaCasilla, filaCasilla }`.

### calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY, double destinoX, double destinoY)
1. Calcular `diferenciaX = destinoX - origenX`.
2. Calcular `diferenciaY = destinoY - origenY`.
3. Retornar `std::atan2(diferenciaY, diferenciaX)`.

### estaPuntoDentroDelConoDeVision(double origenX, double origenY, DireccionMirada direccionMirada, double objetivoX, double objetivoY, double aperturaDelConoEnRadianes)
1. Calcular el vector desplazamiento: `vectorHaciaObjetivoX = objetivoX - origenX` y `vectorHaciaObjetivoY = objetivoY - origenY`.
2. Calcular la distancia al cuadrado: `distanciaAlCuadrado = (vectorHaciaObjetivoX * vectorHaciaObjetivoX) + (vectorHaciaObjetivoY * vectorHaciaObjetivoY)`.
3. Si `distanciaAlCuadrado < toleranciaDistanciaCero`, retornar `true` (el objetivo coincide con el origen del observador).
4. Calcular la magnitud real: `distanciaReal = std::sqrt(distanciaAlCuadrado)`.
5. Normalizar el vector desplazamiento dividiendo cada componente por `distanciaReal`.
6. Obtener el vector unitario de la direccion observada segun `direccionMirada`:
   - Para `DireccionMirada::Este`: `vectorDireccionX = 1.0`, `vectorDireccionY = 0.0`.
   - Para `DireccionMirada::Sur`: `vectorDireccionX = 0.0`, `vectorDireccionY = 1.0`.
   - Para `DireccionMirada::Oeste`: `vectorDireccionX = -1.0`, `vectorDireccionY = 0.0`.
   - Para `DireccionMirada::Norte`: `vectorDireccionX = 0.0`, `vectorDireccionY = -1.0`.
7. Calcular el producto punto: `productoPunto = (vectorHaciaObjetivoX * vectorDireccionX) + (vectorHaciaObjetivoY * vectorDireccionY)`.
8. Calcular el umbral angular minimo: `cosenoUmbralMinimo = std::cos(aperturaDelConoEnRadianes * 0.5)`.
9. Retornar `productoPunto >= cosenoUmbralMinimo`.

### estaPuntoDentroDelRectangulo(double puntoX, double puntoY, const RectanguloVision& rectangulo)
1. Evaluar si `puntoX >= rectangulo.limiteIzquierdoX` y `puntoX <= rectangulo.limiteDerechoX`.
2. Evaluar si `puntoY >= rectangulo.limiteSuperiorY` y `puntoY <= rectangulo.limiteInferiorY`.
3. Retornar `true` si ambas evaluaciones son verdaderas; en caso contrario retornar `false`.
