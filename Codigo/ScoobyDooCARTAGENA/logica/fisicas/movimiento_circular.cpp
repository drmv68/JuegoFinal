#include "fisicas/movimiento_circular.h"
#include "herramientas/calculos_espaciales.h"
#include <cmath>
#include <cassert>

MovimientoCircular::MovimientoCircular()
    : centroDeGiroX(0.0)
    , centroDeGiroY(0.0)
    , radioActualEnCasillas(0.0)
    , radioObjetivoEnCasillas(0.0)
    , velocidadAngularEnRadianesPorSegundo(0.0)
    , faseInicialEnRadianes(0.0)
    , sentidoDeGiro(1)
    , segundosParaElProximoCambio(0.0)
    , segundosRestantesDePausa(0.0)
    , estaEnPausaDeCambio(false)
{}

void MovimientoCircular::configurarParametrosCirculares(double centroX, double centroY,
                                                        double radioInicial,
                                                        double velocidadAngular) {
    assert(radioInicial >= radioMinimoPermitidoEnCasillas);
    assert(velocidadAngular > 0.0);

    centroDeGiroX                       = centroX;
    centroDeGiroY                       = centroY;
    radioActualEnCasillas               = radioInicial;
    radioObjetivoEnCasillas             = radioInicial;
    velocidadAngularEnRadianesPorSegundo = velocidadAngular;
    faseInicialEnRadianes               = 0.0;
    sentidoDeGiro                       = 1;
    segundosParaElProximoCambio         = tiempoMinimoEntreCambiosEnSegundos;
    segundosRestantesDePausa            = 0.0;
    estaEnPausaDeCambio                 = false;

    reiniciarSimulacion();
}

void MovimientoCircular::reiniciarSimulacion() {
    tiempoAcumuladoEnSegundos = 0.0;
    estaSimulacionActiva      = true;
}

CoordenadaFisicaTresEjes MovimientoCircular::avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) {
    if (estaEnPausaDeCambio) {
        segundosRestantesDePausa -= segundosDesdeElCuadroAnterior;

        double fraccionPausaTranscurrida = 1.0 - (segundosRestantesDePausa / duracionPausaDeCambioEnSegundos);
        fraccionPausaTranscurrida = CalculosEspaciales::mantenerValorDentroDelRango(fraccionPausaTranscurrida, 0.0, 1.0);
        radioActualEnCasillas = radioActualEnCasillas + (fraccionPausaTranscurrida * (radioObjetivoEnCasillas - radioActualEnCasillas));

        if (segundosRestantesDePausa <= 0.0) {
            estaEnPausaDeCambio   = false;
            segundosRestantesDePausa = 0.0;
            radioActualEnCasillas = radioObjetivoEnCasillas;
        }

        double anguloActual = faseInicialEnRadianes;
        return CoordenadaFisicaTresEjes{
            centroDeGiroX + (radioActualEnCasillas * std::cos(anguloActual)),
            centroDeGiroY + (radioActualEnCasillas * std::sin(anguloActual)),
            0.0
        };
    }

    tiempoAcumuladoEnSegundos   += segundosDesdeElCuadroAnterior;
    segundosParaElProximoCambio -= segundosDesdeElCuadroAnterior;

    double anguloActual = faseInicialEnRadianes + (static_cast<double>(sentidoDeGiro)
                        * velocidadAngularEnRadianesPorSegundo
                        * tiempoAcumuladoEnSegundos);

    return CoordenadaFisicaTresEjes{
        centroDeGiroX + (radioActualEnCasillas * std::cos(anguloActual)),
        centroDeGiroY + (radioActualEnCasillas * std::sin(anguloActual)),
        0.0
    };
}

void MovimientoCircular::variarUnSoloParametroDePatrulla(AzarConSemilla& azar) {
    double anguloActual   = faseInicialEnRadianes + (static_cast<double>(sentidoDeGiro)
                           * velocidadAngularEnRadianesPorSegundo
                           * tiempoAcumuladoEnSegundos);
    faseInicialEnRadianes    = anguloActual;
    tiempoAcumuladoEnSegundos = 0.0;

    estaEnPausaDeCambio      = true;
    segundosRestantesDePausa = duracionPausaDeCambioEnSegundos;
    segundosParaElProximoCambio = azar.generarDecimalEnRango(tiempoMinimoEntreCambiosEnSegundos,
                                                              tiempoMaximoEntreCambiosEnSegundos);

    int opcionSorteada = azar.generarEnteroEnRango(0, 2);
    if (opcionSorteada == 0) {
        double variacion = azar.generarDecimalEnRango(-incrementoRadioEnCasillas, incrementoRadioEnCasillas);
        radioObjetivoEnCasillas = CalculosEspaciales::mantenerValorDentroDelRango(
            radioActualEnCasillas + variacion,
            radioMinimoPermitidoEnCasillas,
            radioMaximoPermitidoEnCasillas);
    } else if (opcionSorteada == 1) {
        sentidoDeGiro = -sentidoDeGiro;
    } else {
        double multiplicador = azar.generarDecimalEnRango(
            1.0 - factorVariacionVelocidadAngular,
            1.0 + factorVariacionVelocidadAngular);
        velocidadAngularEnRadianesPorSegundo = CalculosEspaciales::mantenerValorDentroDelRango(
            velocidadAngularEnRadianesPorSegundo * multiplicador,
            velocidadAngularMinimaPermitida,
            velocidadAngularMaximaPermitida);
    }
}

double MovimientoCircular::obtenerRadioActualEnCasillas() const {
    return radioActualEnCasillas;
}

double MovimientoCircular::obtenerVelocidadAngularEnRadianesPorSegundo() const {
    return velocidadAngularEnRadianesPorSegundo;
}

bool MovimientoCircular::obtenerEstaEnPausaDeCambio() const {
    return estaEnPausaDeCambio;
}
