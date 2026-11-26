#include "fisicas/tiro_parabolico.h"
#include "herramientas/calculos_espaciales.h"
#include <cassert>

TiroParabolico::TiroParabolico()
    : posicionDeDisparoX(0.0)
    , posicionDeDisparoY(0.0)
    , alturaInicialDelCanonZ(0.0)
    , velocidadInicialDeDisparo(0.0)
    , anguloDeElevacionEnRadianes(0.0)
    , anguloDeDireccionEnMapaEnRadianes(0.0)
    , gravedadSimulada(0.0)
    , tiempoTotalDeVueloCalculado(0.0)
    , puntoDeImpactoX(0.0)
    , puntoDeImpactoY(0.0)
    , yaImpactoContraElSuelo(false)
{}

void TiroParabolico::configurarDisparoDesdeMuralla(double origenX, double origenY,
                                                   double alturaCanonZ, double velocidad,
                                                   double anguloElevacionRadianes,
                                                   double anguloDireccionMapaRadianes,
                                                   double gravedad) {
    assert(velocidad > 0.0);
    assert(gravedad > 0.0);

    posicionDeDisparoX                = origenX;
    posicionDeDisparoY                = origenY;
    alturaInicialDelCanonZ            = alturaCanonZ;
    velocidadInicialDeDisparo         = velocidad;
    anguloDeElevacionEnRadianes       = anguloElevacionRadianes;
    anguloDeDireccionEnMapaEnRadianes = anguloDireccionMapaRadianes;
    gravedadSimulada                  = gravedad;

    precalcularTiempoYPuntoDeImpacto();
    reiniciarSimulacion();
}

void TiroParabolico::configurarDisparoHaciaUnPunto(double origenX, double origenY,
                                                   double alturaCanonZ,
                                                   double destinoX, double destinoY,
                                                   double segundosDeVuelo, double gravedad) {
    assert(segundosDeVuelo >= tiempoMinimoDeVueloEnSegundos);
    assert(gravedad > 0.0);

    posicionDeDisparoX          = origenX;
    posicionDeDisparoY          = origenY;
    alturaInicialDelCanonZ      = alturaCanonZ;
    puntoDeImpactoX             = destinoX;
    puntoDeImpactoY             = destinoY;
    tiempoTotalDeVueloCalculado = segundosDeVuelo;
    gravedadSimulada            = gravedad;

    double diferenciaHorizontalX      = destinoX - origenX;
    double diferenciaVerticalY        = destinoY - origenY;
    double distanciaHorizontal        = std::sqrt((diferenciaHorizontalX * diferenciaHorizontalX) + (diferenciaVerticalY * diferenciaVerticalY));
    anguloDeDireccionEnMapaEnRadianes = std::atan2(diferenciaVerticalY, diferenciaHorizontalX);
    double velocidadHorizontal        = distanciaHorizontal / segundosDeVuelo;
    double velocidadVerticalInicial   = ((factorMitadCuadratica * gravedad * segundosDeVuelo * segundosDeVuelo) - alturaCanonZ) / segundosDeVuelo;

    velocidadInicialDeDisparo   = std::sqrt((velocidadHorizontal * velocidadHorizontal) + (velocidadVerticalInicial * velocidadVerticalInicial));
    anguloDeElevacionEnRadianes = std::atan2(velocidadVerticalInicial, velocidadHorizontal);
    yaImpactoContraElSuelo      = false;

    reiniciarSimulacion();
}

void TiroParabolico::precalcularTiempoYPuntoDeImpacto() {
    assert(gravedadSimulada > 0.0);

    double coeficienteA  = -factorMitadCuadratica * gravedadSimulada;
    double coeficienteB  = velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes);
    double coeficienteC  = alturaInicialDelCanonZ;
    double discriminante = (coeficienteB * coeficienteB) - (factorCuatroFormulaCuadratica * coeficienteA * coeficienteC);
    assert(discriminante >= 0.0);

    tiempoTotalDeVueloCalculado = (-coeficienteB - std::sqrt(discriminante)) / (factorDosFormulaCuadratica * coeficienteA);

    double distanciaHorizontalTotal = velocidadInicialDeDisparo
                                    * std::cos(anguloDeElevacionEnRadianes)
                                    * tiempoTotalDeVueloCalculado;
    puntoDeImpactoX        = posicionDeDisparoX + (distanciaHorizontalTotal * std::cos(anguloDeDireccionEnMapaEnRadianes));
    puntoDeImpactoY        = posicionDeDisparoY + (distanciaHorizontalTotal * std::sin(anguloDeDireccionEnMapaEnRadianes));
    yaImpactoContraElSuelo = false;
}

void TiroParabolico::reiniciarSimulacion() {
    tiempoAcumuladoEnSegundos = 0.0;
    yaImpactoContraElSuelo    = false;
    estaSimulacionActiva      = true;
}

CoordenadaFisicaTresEjes TiroParabolico::avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) {
    if (yaImpactoContraElSuelo) {
        return CoordenadaFisicaTresEjes{ puntoDeImpactoX, puntoDeImpactoY, alturaSueloNivelZero };
    }

    if (estaSimulacionActiva) {
        tiempoAcumuladoEnSegundos += segundosDesdeElCuadroAnterior;
    }

    if (tiempoAcumuladoEnSegundos >= tiempoTotalDeVueloCalculado) {
        tiempoAcumuladoEnSegundos = tiempoTotalDeVueloCalculado;
        yaImpactoContraElSuelo    = true;
        estaSimulacionActiva      = false;
        return CoordenadaFisicaTresEjes{ puntoDeImpactoX, puntoDeImpactoY, alturaSueloNivelZero };
    }

    double tiempoActualEnSegundos = tiempoAcumuladoEnSegundos;
    double distanciaHorizontal    = velocidadInicialDeDisparo * std::cos(anguloDeElevacionEnRadianes) * tiempoActualEnSegundos;
    double posicionActualX        = posicionDeDisparoX + (distanciaHorizontal * std::cos(anguloDeDireccionEnMapaEnRadianes));
    double posicionActualY        = posicionDeDisparoY + (distanciaHorizontal * std::sin(anguloDeDireccionEnMapaEnRadianes));
    double alturaActualZ          = alturaInicialDelCanonZ
                                  + (velocidadInicialDeDisparo * std::sin(anguloDeElevacionEnRadianes) * tiempoActualEnSegundos)
                                  - (factorMitadCuadratica * gravedadSimulada * tiempoActualEnSegundos * tiempoActualEnSegundos);
    if (alturaActualZ < alturaSueloNivelZero) {
        alturaActualZ = alturaSueloNivelZero;
    }
    return CoordenadaFisicaTresEjes{ posicionActualX, posicionActualY, alturaActualZ };
}

CoordenadaFisicaTresEjes TiroParabolico::obtenerCoordenadaDeImpactoFinal() const {
    return CoordenadaFisicaTresEjes{ puntoDeImpactoX, puntoDeImpactoY, alturaSueloNivelZero };
}

double TiroParabolico::obtenerProgresoDeVueloNormalizado() const {
    if (tiempoTotalDeVueloCalculado <= 0.0) {
        return progresoMaximoNormalizado;
    }
    return CalculosEspaciales::mantenerValorDentroDelRango(
        tiempoAcumuladoEnSegundos / tiempoTotalDeVueloCalculado,
        progresoMinimoNormalizado,
        progresoMaximoNormalizado);
}

bool TiroParabolico::obtenerYaImpactoContraElSuelo() const {
    return yaImpactoContraElSuelo;
}

bool TiroParabolico::estaEnMomentoClave() const {
    return yaImpactoContraElSuelo;
}
