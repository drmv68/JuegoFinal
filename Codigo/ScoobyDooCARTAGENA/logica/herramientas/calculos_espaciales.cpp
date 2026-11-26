#include "herramientas/calculos_espaciales.h"

namespace CalculosEspaciales {

    CoordenadaCasilla convertirPosicionACasilla(double posicionX, double posicionY) noexcept {
        int columnaCasilla = static_cast<int>(std::floor(posicionX));
        int filaCasilla    = static_cast<int>(std::floor(posicionY));
        return CoordenadaCasilla{ columnaCasilla, filaCasilla };
    }

    double calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY,
                                              double destinoX, double destinoY) noexcept {
        double diferenciaX = destinoX - origenX;
        double diferenciaY = destinoY - origenY;
        return std::atan2(diferenciaY, diferenciaX);
    }

    bool estaPuntoDentroDelConoDeVision(double origenX, double origenY,
                                       DireccionMirada direccionMirada,
                                       double objetivoX, double objetivoY,
                                       double aperturaDelConoEnRadianes) noexcept {
        double vectorHaciaObjetivoX = objetivoX - origenX;
        double vectorHaciaObjetivoY = objetivoY - origenY;
        double distanciaAlCuadrado  = (vectorHaciaObjetivoX * vectorHaciaObjetivoX)
                                    + (vectorHaciaObjetivoY * vectorHaciaObjetivoY);

        if (distanciaAlCuadrado < toleranciaDistanciaCero) {
            return true;
        }

        double distanciaReal              = std::sqrt(distanciaAlCuadrado);
        double vectorNormalizadoHaciaX    = vectorHaciaObjetivoX / distanciaReal;
        double vectorNormalizadoHaciaY    = vectorHaciaObjetivoY / distanciaReal;

        double vectorDireccionX = 0.0;
        double vectorDireccionY = 0.0;
        switch (direccionMirada) {
            case DireccionMirada::Este:  vectorDireccionX =  1.0; break;
            case DireccionMirada::Oeste: vectorDireccionX = -1.0; break;
            case DireccionMirada::Sur:   vectorDireccionY =  1.0; break;
            case DireccionMirada::Norte: vectorDireccionY = -1.0; break;
        }

        double productoPunto       = (vectorNormalizadoHaciaX * vectorDireccionX)
                                   + (vectorNormalizadoHaciaY * vectorDireccionY);
        double cosenoUmbralMinimo  = std::cos(aperturaDelConoEnRadianes * 0.5);
        return productoPunto >= cosenoUmbralMinimo;
    }

    bool estaPuntoDentroDelRectangulo(double puntoX, double puntoY,
                                     const RectanguloVision& rectangulo) noexcept {
        return puntoX >= rectangulo.limiteIzquierdoX
            && puntoX <= rectangulo.limiteDerechoX
            && puntoY >= rectangulo.limiteSuperiorY
            && puntoY <= rectangulo.limiteInferiorY;
    }

}
