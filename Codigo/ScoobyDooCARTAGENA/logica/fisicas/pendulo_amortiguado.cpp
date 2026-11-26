#include "fisicas/pendulo_amortiguado.h"
#include "herramientas/calculos_espaciales.h"
#include <cassert>

PenduloAmortiguado::PenduloAmortiguado()
    : puntoAnclajeX(0.0)
    , puntoAnclajeY(0.0)
    , longitudCuerdaEnCasillas(0.0)
    , amplitudMaximaEnRadianes(0.0)
    , frecuenciaOscilacionEnHertz(0.0)
    , factorDeAmortiguamiento(0.0)
    , anguloActualEnRadianes(0.0)
    , amplitudBaseActual(0.0)
    , segundosDesdeUltimoImpulso(0.0)
{}

void PenduloAmortiguado::configurarParametrosPendulo(double anclajeX, double anclajeY,
                                                     double longitud, double amplitudRadianes,
                                                     double frecuenciaHertz, double amortiguamiento) {
    assert(longitud > 0.0);
    assert(frecuenciaHertz > 0.0);
    assert(amortiguamiento >= 0.0);

    puntoAnclajeX              = anclajeX;
    puntoAnclajeY              = anclajeY;
    longitudCuerdaEnCasillas   = longitud;
    amplitudMaximaEnRadianes   = amplitudRadianes;
    frecuenciaOscilacionEnHertz = frecuenciaHertz;
    factorDeAmortiguamiento    = amortiguamiento;

    reiniciarSimulacion();
}

void PenduloAmortiguado::reiniciarSimulacion() {
    tiempoAcumuladoEnSegundos  = 0.0;
    segundosDesdeUltimoImpulso = 0.0;
    amplitudBaseActual         = amplitudMaximaEnRadianes;
    anguloActualEnRadianes     = amplitudMaximaEnRadianes;
    estaSimulacionActiva       = true;
}

CoordenadaFisicaTresEjes PenduloAmortiguado::avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) {
    if (!estaSimulacionActiva) {
        return CoordenadaFisicaTresEjes{ puntoAnclajeX, puntoAnclajeY, 0.0 };
    }

    tiempoAcumuladoEnSegundos  += segundosDesdeElCuadroAnterior;
    segundosDesdeUltimoImpulso += segundosDesdeElCuadroAnterior;

    double envolventeExponencial = std::exp(-factorDeAmortiguamiento * segundosDesdeUltimoImpulso);

    if (envolventeExponencial < umbralReinicioAmortiguamiento) {
        segundosDesdeUltimoImpulso = 0.0;
        envolventeExponencial      = 1.0;
    }

    double amplitudInstantanea      = amplitudMaximaEnRadianes * envolventeExponencial;
    double faseTemporal             = constanteDosPi * frecuenciaOscilacionEnHertz * tiempoAcumuladoEnSegundos;
    anguloActualEnRadianes          = amplitudInstantanea * std::cos(faseTemporal);

    double desplazamientoHorizontalX = longitudCuerdaEnCasillas * std::sin(anguloActualEnRadianes);
    double alturaRelativaZ           = longitudCuerdaEnCasillas * (1.0 - std::cos(anguloActualEnRadianes));

    return CoordenadaFisicaTresEjes{ puntoAnclajeX + desplazamientoHorizontalX, puntoAnclajeY, alturaRelativaZ };
}

double PenduloAmortiguado::obtenerAnguloActualEnRadianes() const {
    return anguloActualEnRadianes;
}

double PenduloAmortiguado::obtenerAmplitudActualEnRadianes() const {
    return amplitudMaximaEnRadianes * std::exp(-factorDeAmortiguamiento * segundosDesdeUltimoImpulso);
}

bool PenduloAmortiguado::estaEnMomentoClave() const {
    double amplitudInstantanea = amplitudMaximaEnRadianes * std::exp(-factorDeAmortiguamiento * segundosDesdeUltimoImpulso);
    double anguloSinSigno      = std::fabs(anguloActualEnRadianes);
    return anguloSinSigno >= (amplitudInstantanea * umbralPorcentualMomentoClave);
}
