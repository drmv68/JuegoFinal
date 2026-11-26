#ifndef TIPOS_ESPACIALES_H
#define TIPOS_ESPACIALES_H

#include <cstdint>

struct CoordenadaCasilla {
    int columna = 0;
    int fila    = 0;

    constexpr bool operator==(const CoordenadaCasilla& otraCasilla) const noexcept {
        return (columna == otraCasilla.columna && fila == otraCasilla.fila);
    }

    constexpr bool operator!=(const CoordenadaCasilla& otraCasilla) const noexcept {
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
    double limiteIzquierdoX = 0.0;
    double limiteSuperiorY  = 0.0;
    double limiteDerechoX   = 0.0;
    double limiteInferiorY  = 0.0;
};

struct CoordenadaFisicaTresEjes {
    double posicionX = 0.0;
    double posicionY = 0.0;
    double alturaZ   = 0.0;
};

#endif
