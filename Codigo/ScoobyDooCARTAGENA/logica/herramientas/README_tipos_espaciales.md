# ESPECIFICACION TECNICA: tipos_espaciales.h

## Archivo y Ubicacion
`logica/herramientas/tipos_espaciales.h` (Archivo exclusivo de cabecera).

## Proposito y Responsabilidad Unica
Define los tipos de datos primitivos del espacio bidimensional y orientaciones del mapa. Elimina dependencias circulares entre mapa, entidades y herramientas matematicas.

## Dependencias
- `#include <cstdint>`

## Definicion Exacta de Cabecera (tipos_espaciales.h)

```cpp
#ifndef TIPOS_ESPACIALES_H
#define TIPOS_ESPACIALES_H

#include <cstdint>

struct CoordenadaCasilla {
    int columna;
    int fila;

    bool operator==(const CoordenadaCasilla& otraCasilla) const {
        return (columna == otraCasilla.columna && fila == otraCasilla.fila);
    }

    bool operator!=(const CoordenadaCasilla& otraCasilla) const {
        return !(*this == otraCasilla);
    }
};

enum class DireccionMirada : uint8_t {
    Norte,
    Sur,
    Este,
    Oeste
};

struct RectanguloVision {
    double limiteIzquierdoX;
    double limiteSuperiorY;
    double limiteDerechoX;
    double limiteInferiorY;
};

struct CoordenadaFisicaTresEjes {
    double posicionX;
    double posicionY;
    double alturaZ;
};

#endif
```
