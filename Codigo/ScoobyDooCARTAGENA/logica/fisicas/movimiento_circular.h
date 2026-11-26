#ifndef MOVIMIENTO_CIRCULAR_H
#define MOVIMIENTO_CIRCULAR_H

#include "fisicas/movimiento.h"
#include "herramientas/azar_con_semilla.h"

class MovimientoCircular final : public MovimientoBase {
public:
    static constexpr double duracionPausaDeCambioEnSegundos      = 0.8;
    static constexpr double tiempoMinimoEntreCambiosEnSegundos   = 6.0;
    static constexpr double tiempoMaximoEntreCambiosEnSegundos   = 12.0;
    static constexpr double factorVariacionVelocidadAngular      = 0.10;
    static constexpr double incrementoRadioEnCasillas            = 0.40;
    static constexpr double radioMinimoPermitidoEnCasillas       = 1.5;
    static constexpr double radioMaximoPermitidoEnCasillas       = 5.0;
    static constexpr double velocidadAngularMinimaPermitida      = 0.5;
    static constexpr double velocidadAngularMaximaPermitida      = 2.0;

    MovimientoCircular();
    ~MovimientoCircular() override = default;

    MovimientoCircular(const MovimientoCircular&) = delete;
    MovimientoCircular& operator=(const MovimientoCircular&) = delete;

    void configurarParametrosCirculares(double centroX, double centroY,
                                        double radioInicial, double velocidadAngular);

    void reiniciarSimulacion() override;
    CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) override;
    void variarUnSoloParametroDePatrulla(AzarConSemilla& azar);

    double obtenerRadioActualEnCasillas() const;
    double obtenerVelocidadAngularEnRadianesPorSegundo() const;
    bool   obtenerEstaEnPausaDeCambio() const;

private:
    double centroDeGiroX;
    double centroDeGiroY;
    double radioActualEnCasillas;
    double radioObjetivoEnCasillas;
    double velocidadAngularEnRadianesPorSegundo;
    double faseInicialEnRadianes;
    int    sentidoDeGiro;
    double segundosParaElProximoCambio;
    double segundosRestantesDePausa;
    bool   estaEnPausaDeCambio;
};

#endif
