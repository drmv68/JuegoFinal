#ifndef TIRO_PARABOLICO_H
#define TIRO_PARABOLICO_H

#include "fisicas/movimiento.h"
#include <cmath>

class TiroParabolico final : public MovimientoBase {
public:
    static constexpr double gravedadTerrestreSimulada       = 9.81;
    static constexpr double alturaSueloNivelZero            = 0.0;
    static constexpr double tiempoMinimoDeVueloEnSegundos   = 0.10;
    static constexpr double factorMitadCuadratica           = 0.5;
    static constexpr double factorCuatroFormulaCuadratica   = 4.0;
    static constexpr double factorDosFormulaCuadratica      = 2.0;
    static constexpr double progresoMinimoNormalizado       = 0.0;
    static constexpr double progresoMaximoNormalizado       = 1.0;

    TiroParabolico();
    ~TiroParabolico() override = default;

    TiroParabolico(const TiroParabolico&) = delete;
    TiroParabolico& operator=(const TiroParabolico&) = delete;

    void configurarDisparoDesdeMuralla(double origenX, double origenY,
                                       double alturaCanonZ, double velocidad,
                                       double anguloElevacionRadianes,
                                       double anguloDireccionMapaRadianes,
                                       double gravedad);

    void configurarDisparoHaciaUnPunto(double origenX, double origenY,
                                       double alturaCanonZ,
                                       double destinoX, double destinoY,
                                       double segundosDeVuelo, double gravedad);

    void precalcularTiempoYPuntoDeImpacto();
    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;

    CoordenadaFisicaTresEjes obtenerCoordenadaDeImpactoFinal() const;
    double obtenerProgresoDeVueloNormalizado() const;
    bool   obtenerYaImpactoContraElSuelo() const;
    bool   estaEnMomentoClave() const override;

private:
    double posicionDeDisparoX;
    double posicionDeDisparoY;
    double alturaInicialDelCanonZ;
    double velocidadInicialDeDisparo;
    double anguloDeElevacionEnRadianes;
    double anguloDeDireccionEnMapaEnRadianes;
    double gravedadSimulada;
    double tiempoTotalDeVueloCalculado;
    double puntoDeImpactoX;
    double puntoDeImpactoY;
    bool   yaImpactoContraElSuelo;
};

#endif
