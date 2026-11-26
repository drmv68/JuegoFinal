#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include "herramientas/tipos_espaciales.h"

class MovimientoBase {
public:
    MovimientoBase()
        : tiempoAcumuladoEnSegundos(0.0)
        , estaSimulacionActiva(true) {}

    virtual ~MovimientoBase() = default;

    MovimientoBase(const MovimientoBase&) = delete;
    MovimientoBase& operator=(const MovimientoBase&) = delete;

    virtual void reiniciarSimulacion() = 0;
    virtual CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) = 0;

    virtual bool estaEnMomentoClave() const {
        return false;
    }

    double obtenerTiempoAcumuladoEnSegundos() const {
        return tiempoAcumuladoEnSegundos;
    }

    bool obtenerEstaSimulacionActiva() const {
        return estaSimulacionActiva;
    }

    void establecerSimulacionActiva(bool estadoActivo) {
        estaSimulacionActiva = estadoActivo;
    }

protected:
    double tiempoAcumuladoEnSegundos;
    bool   estaSimulacionActiva;
};

#endif
