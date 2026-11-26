#ifndef CALCULOS_ESPACIALES_H
#define CALCULOS_ESPACIALES_H

#include <cmath>
#include <algorithm>
#include "herramientas/tipos_espaciales.h"

namespace CalculosEspaciales {

    inline constexpr double constantePi      = 3.14159265358979323846;
    inline constexpr double constanteDosPi   = 6.28318530717958647692;
    inline constexpr double constanteMedioPi = 1.57079632679489661923;
    inline constexpr double toleranciaDistanciaCero = 0.000001;

    inline double mantenerValorDentroDelRango(double valorActual,
                                              double valorMinimoPermitido,
                                              double valorMaximoPermitido) noexcept {
        return std::clamp(valorActual, valorMinimoPermitido, valorMaximoPermitido);
    }

    inline double obtenerValorSinSigno(double valor) noexcept {
        return std::fabs(valor);
    }

    inline double calcularDistanciaAlCuadrado(double origenX, double origenY,
                                              double destinoX, double destinoY) noexcept {
        double diferenciaX = destinoX - origenX;
        double diferenciaY = destinoY - origenY;
        return (diferenciaX * diferenciaX) + (diferenciaY * diferenciaY);
    }

    inline double calcularDistanciaEnUnEje(double coordenadaA,
                                           double coordenadaB) noexcept {
        return std::fabs(coordenadaA - coordenadaB);
    }

    CoordenadaCasilla convertirPosicionACasilla(double posicionX, double posicionY) noexcept;
    double calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY,
                                              double destinoX, double destinoY) noexcept;
    bool estaPuntoDentroDelConoDeVision(double origenX, double origenY,
                                       DireccionMirada direccionMirada,
                                       double objetivoX, double objetivoY,
                                       double aperturaDelConoEnRadianes) noexcept;
    bool estaPuntoDentroDelRectangulo(double puntoX, double puntoY,
                                     const RectanguloVision& rectangulo) noexcept;

}

#endif
