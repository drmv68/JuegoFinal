# ESPECIFICACION TECNICA: elemento_del_mapa.h / elemento_del_mapa.cpp

## Archivo de Cabecera: `elemento_del_mapa.h`

### Guardas de inclusion
```cpp
#ifndef ELEMENTO_DEL_MAPA_H
#define ELEMENTO_DEL_MAPA_H
```

### Inclusiones requeridas
```cpp
#include "mapa/mapa.h"
```

### Enumeracion
```cpp
enum class DireccionMirada {
    Norte,
    Sur,
    Este,
    Oeste
};
```

### Declaracion de Clase Madre Abstracta `ElementoDelMapaBase`
```cpp
class ElementoDelMapaBase {
private:
    static int ultimoNumeroDeSerieAsignado;

protected:
    int numeroDeSerie;
    double posicionX;
    double posicionY;
    double anchoDeColision;
    double altoDeColision;
    DireccionMirada direccionMirada;
    bool estaActivo;
    bool esVisible;

public:
    ElementoDelMapaBase(double posX, double posY, double ancho, double alto);
    virtual ~ElementoDelMapaBase() = default;

    virtual void avanzarUnCuadro(double segundosTranscurridos) = 0;

    int obtenerNumeroDeSerie() const;
    double obtenerPosicionX() const;
    double obtenerPosicionY() const;
    double obtenerAnchoDeColision() const;
    double obtenerAltoDeColision() const;
    DireccionMirada obtenerDireccionMirada() const;
    bool obtenerEstaActivo() const;
    bool obtenerEsVisible() const;

    void establecerPosicion(double nuevaX, double nuevaY);
    void establecerDireccionMirada(DireccionMirada nuevaDireccion);
    void establecerActivo(bool activo);
    void establecerVisible(bool visible);

    CoordenadaCasilla obtenerCasillaActual() const;
    bool colisionaCon(const ElementoDelMapaBase& otroElemento) const;
    double calcularOrdenDeDibujo() const;
};

#endif
```

---

## Archivo de Implementacion: `elemento_del_mapa.cpp`

### Inclusiones requeridas
```cpp
#include "personajes/elemento_del_mapa.h"
#include "herramientas/calculos_espaciales.h"
#include <cmath>
```

### Inicializacion de Variable Estatica
```cpp
int ElementoDelMapaBase::ultimoNumeroDeSerieAsignado = 0;
```

### Implementacion de Metodos

#### 1. Constructor
- **Firma:** `ElementoDelMapaBase::ElementoDelMapaBase(double posX, double posY, double ancho, double alto)`
- **Logica:**
  - `numeroDeSerie = ++ultimoNumeroDeSerieAsignado;`
  - `posicionX = posX; posicionY = posY;`
  - `anchoDeColision = ancho; altoDeColision = alto;`
  - `direccionMirada = DireccionMirada::Sur;`
  - `estaActivo = true; esVisible = true;`

#### 2. Getters y Setters
- Implementacion estandar de acceso a atributos protegidos.

#### 3. `obtenerCasillaActual`
- **Firma:** `CoordenadaCasilla ElementoDelMapaBase::obtenerCasillaActual() const`
- **Logica:**
  - Retorna `CalculosEspaciales::convertirPosicionACasilla(posicionX, posicionY);`

#### 4. `colisionaCon`
- **Firma:** `bool ElementoDelMapaBase::colisionaCon(const ElementoDelMapaBase& otroElemento) const`
- **Logica:**
  - Si `!estaActivo || !otroElemento.estaActivo` retorna `false`.
  - Comparacion AABB centrada:
    - `double mitadAnchoA = anchoDeColision / 2.0;`
    - `double mitadAltoA = altoDeColision / 2.0;`
    - `double mitadAnchoB = otroElemento.anchoDeColision / 2.0;`
    - `double mitadAltoB = otroElemento.altoDeColision / 2.0;`
    - `bool solapaX = (posicionX + mitadAnchoA >= otroElemento.posicionX - mitadAnchoB) && (posicionX - mitadAnchoA <= otroElemento.posicionX + mitadAnchoB);`
    - `bool solapaY = (posicionY + mitadAltoA >= otroElemento.posicionY - mitadAltoB) && (posicionY - mitadAltoA <= otroElemento.posicionY + mitadAltoB);`
    - Retorna `solapaX && solapaY;`

#### 5. `calcularOrdenDeDibujo`
- **Firma:** `double ElementoDelMapaBase::calcularOrdenDeDibujo() const`
- **Logica:**
  - Retorna `posicionX + posicionY;`
