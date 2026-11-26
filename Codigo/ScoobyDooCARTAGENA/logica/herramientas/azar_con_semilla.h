#ifndef AZAR_CON_SEMILLA_H
#define AZAR_CON_SEMILLA_H

#include <cstdint>
#include <vector>
#include <cassert>
#include "herramientas/tipos_espaciales.h"

class AzarConSemilla final {
public:
    static constexpr uint32_t semillaPorDefectoSiEsCero  = 2463534242u;
    static constexpr double   divisorMaximoEnteroSinSigno = 4294967295.0;
    static constexpr double   porcentajeMinimoValido      = 0.0;
    static constexpr double   porcentajeMaximoValido      = 100.0;

    AzarConSemilla() noexcept;
    explicit AzarConSemilla(uint32_t semillaInicial) noexcept;
    ~AzarConSemilla() = default;

    AzarConSemilla(const AzarConSemilla&) = default;
    AzarConSemilla& operator=(const AzarConSemilla&) = default;

    void     inicializarConSemillaDelSistema() noexcept;
    void     inicializarConSemillaFija(uint32_t semillaManual) noexcept;
    uint32_t obtenerNumeroDeSemillaActiva() const noexcept;

    int    generarEnteroEnRango(int minimoIncluido, int maximoIncluido) noexcept;
    double generarDecimalEnRango(double minimoIncluido, double maximoIncluido) noexcept;
    bool   evaluarProbabilidadPorcentual(double porcentajeDeExito) noexcept;
    void   barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas) noexcept;

private:
    uint32_t estadoInternoXorshift = semillaPorDefectoSiEsCero;
    uint32_t numeroDeSemillaActiva = semillaPorDefectoSiEsCero;

    uint32_t avanzarPasoXorshift() noexcept;
};

#endif
