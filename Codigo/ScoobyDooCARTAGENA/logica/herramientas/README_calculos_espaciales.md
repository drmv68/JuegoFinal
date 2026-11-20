# ESPECIFICACION TECNICA: calculos_espaciales.h / calculos_espaciales.cpp

## Archivo de Cabecera: `calculos_espaciales.h`

### Guardas de inclusion
```cpp
#ifndef CALCULOS_ESPACIALES_H
#define CALCULOS_ESPACIALES_H
```

### Inclusiones requeridas
```cpp
#include <cmath>
#include "mapa/mapa.h"
#include "personajes/elemento_del_mapa.h"
```

### Constantes matematicas constexpr
```cpp
namespace CalculosEspaciales {
    constexpr double PI_EXACTO = 3.14159265358979323846;
    constexpr double MEDIO_PI = 1.57079632679489661923;
}
```

### Estructuras
```cpp
struct RectanguloVision {
    double limiteIzquierdoX;
    double limiteSuperiorY;
    double limiteDerechoX;
    double limiteInferiorY;
};
```

### Declaraciones en Namespace `CalculosEspaciales`
```cpp
namespace CalculosEspaciales {
    inline double mantenerValorDentroDelRango(double valorActual, double valorMinimoPermitido, double valorMaximoPermitido) {
        if (valorActual < valorMinimoPermitido) {
            return valorMinimoPermitido;
        }
        if (valorActual > valorMaximoPermitido) {
            return valorMaximoPermitido;
        }
        return valorActual;
    }

    inline double obtenerValorSinSigno(double valor) {
        return (valor < 0.0) ? -valor : valor;
    }

    inline double calcularDistanciaAlCuadrado(double origenX, double origenY, double destinoX, double destinoY) {
        double diferenciaX = destinoX - origenX;
        double diferenciaY = destinoY - origenY;
        return (diferenciaX * diferenciaX) + (diferenciaY * diferenciaY);
    }

    inline double calcularDistanciaEnUnEje(double coordenadaA, double coordenadaB) {
        return obtenerValorSinSigno(coordenadaA - coordenadaB);
    }

    CoordenadaCasilla convertirPosicionACasilla(double posicionX, double posicionY);
    double calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY, double destinoX, double destinoY);
    bool estaPuntoDentroDelConoDeVision(double origenX, double origenY, DireccionMirada direccionMirada, double objetivoX, double objetivoY, double aperturaDelConoEnRadianes);
    bool estaPuntoDentroDelRectangulo(double puntoX, double puntoY, const RectanguloVision& rectangulo);
}
```

---

## Archivo de Implementacion: `calculos_espaciales.cpp`

### Inclusiones requeridas
```cpp
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Funciones

#### 1. `convertirPosicionACasilla`
- **Firma:** `CoordenadaCasilla CalculosEspaciales::convertirPosicionACasilla(double posicionX, double posicionY)`
- **Logica:**
  - `int columnaCalculada = static_cast<int>(std::floor(posicionX));`
  - `int filaCalculada = static_cast<int>(std::floor(posicionY));`
  - Retorna `CoordenadaCasilla{ columnaCalculada, filaCalculada };`

#### 2. `calcularAnguloEntrePuntosEnRadianes`
- **Firma:** `double CalculosEspaciales::calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY, double destinoX, double destinoY)`
- **Logica:**
  - `double diferenciaX = destinoX - origenX;`
  - `double diferenciaY = destinoY - origenY;`
  - Retorna `std::atan2(diferenciaY, diferenciaX);`

#### 3. `estaPuntoDentroDelConoDeVision`
- **Firma:** `bool CalculosEspaciales::estaPuntoDentroDelConoDeVision(double origenX, double origenY, DireccionMirada direccionMirada, double objetivoX, double objetivoY, double aperturaDelConoEnRadianes)`
- **Logica:**
  - Determinar angulo central segun direccion:
    - Caso `DireccionMirada::Este`: `anguloCentral = 0.0;`
    - Caso `DireccionMirada::Sur`: `anguloCentral = MEDIO_PI;`
    - Caso `DireccionMirada::Oeste`: `anguloCentral = PI_EXACTO;`
    - Caso `DireccionMirada::Norte`: `anguloCentral = -MEDIO_PI;`
  - `double anguloHaciaObjetivo = calcularAnguloEntrePuntosEnRadianes(origenX, origenY, objetivoX, objetivoY);`
  - `double diferenciaAngular = std::atan2(std::sin(anguloHaciaObjetivo - anguloCentral), std::cos(anguloHaciaObjetivo - anguloCentral));`
  - `double diferenciaSinSigno = obtenerValorSinSigno(diferenciaAngular);`
  - Retorna `diferenciaSinSigno <= (aperturaDelConoEnRadianes / 2.0);`

#### 4. `estaPuntoDentroDelRectangulo`
- **Firma:** `bool CalculosEspaciales::estaPuntoDentroDelRectangulo(double puntoX, double puntoY, const RectanguloVision& rectangulo)`
- **Logica:**
  - Retorna `(puntoX >= rectangulo.limiteIzquierdoX && puntoX <= rectangulo.limiteDerechoX && puntoY >= rectangulo.limiteSuperiorY && puntoY <= rectangulo.limiteInferiorY);`
