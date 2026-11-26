#ifndef PENDULO_AMORTIGUADO_H
#define PENDULO_AMORTIGUADO_H

#include "fisicas/movimiento.h"
#include <cmath>

class PenduloAmortiguado final : public MovimientoBase {
public:
    static constexpr double constanteDosPi                  = 6.28318530717958647692;
    static constexpr double umbralPorcentualMomentoClave     = 0.90;
    static constexpr double umbralReinicioAmortiguamiento    = 0.40;
    static constexpr double duracionRampaImpulsoEnSegundos   = 1.0;

    PenduloAmortiguado();
    ~PenduloAmortiguado() override = default;

    PenduloAmortiguado(const PenduloAmortiguado&) = delete;
    PenduloAmortiguado& operator=(const PenduloAmortiguado&) = delete;

    void configurarParametrosPendulo(double anclajeX, double anclajeY,
                                     double longitud, double amplitudRadianes,
                                     double frecuenciaHertz, double amortiguamiento);

    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;

    double obtenerAnguloActualEnRadianes() const;
    double obtenerAmplitudActualEnRadianes() const;
    bool   estaEnMomentoClave() const override;

private:
    double puntoAnclajeX;
    double puntoAnclajeY;
    double longitudCuerdaEnCasillas;
    double amplitudMaximaEnRadianes;
    double frecuenciaOscilacionEnHertz;
    double factorDeAmortiguamiento;
    double anguloActualEnRadianes;
    double amplitudBaseActual;
    double segundosDesdeUltimoImpulso;
};

#endif
