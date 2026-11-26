#include "herramientas/azar_con_semilla.h"
#include <chrono>
#include <algorithm>

AzarConSemilla::AzarConSemilla() noexcept {
    inicializarConSemillaDelSistema();
}

AzarConSemilla::AzarConSemilla(uint32_t semillaInicial) noexcept {
    inicializarConSemillaFija(semillaInicial);
}

uint32_t AzarConSemilla::avanzarPasoXorshift() noexcept {
    uint32_t registroTemporal = estadoInternoXorshift;
    registroTemporal ^= (registroTemporal << 13);
    registroTemporal ^= (registroTemporal >> 17);
    registroTemporal ^= (registroTemporal << 5);
    estadoInternoXorshift = registroTemporal;
    return estadoInternoXorshift;
}

void AzarConSemilla::inicializarConSemillaDelSistema() noexcept {
    long long tiempoNanosegundos =
        std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint32_t semillaGenerada =
        static_cast<uint32_t>(tiempoNanosegundos ^ (tiempoNanosegundos >> 32));
    inicializarConSemillaFija(semillaGenerada);
}

void AzarConSemilla::inicializarConSemillaFija(uint32_t semillaManual) noexcept {
    if (semillaManual == 0u) {
        semillaManual = semillaPorDefectoSiEsCero;
    }
    numeroDeSemillaActiva = semillaManual;
    estadoInternoXorshift = semillaManual;
}

uint32_t AzarConSemilla::obtenerNumeroDeSemillaActiva() const noexcept {
    return numeroDeSemillaActiva;
}

int AzarConSemilla::generarEnteroEnRango(int minimoIncluido, int maximoIncluido) noexcept {
    assert(minimoIncluido <= maximoIncluido);
    if (minimoIncluido == maximoIncluido) {
        return minimoIncluido;
    }
    uint32_t amplitudRango = static_cast<uint32_t>((maximoIncluido - minimoIncluido) + 1);
    uint32_t valorCrudo    = avanzarPasoXorshift();
    return minimoIncluido + static_cast<int>(valorCrudo % amplitudRango);
}

double AzarConSemilla::generarDecimalEnRango(double minimoIncluido,
                                             double maximoIncluido) noexcept {
    assert(minimoIncluido <= maximoIncluido);
    if (minimoIncluido == maximoIncluido) {
        return minimoIncluido;
    }
    double valorNormalizado =
        static_cast<double>(avanzarPasoXorshift()) / divisorMaximoEnteroSinSigno;
    return minimoIncluido + (valorNormalizado * (maximoIncluido - minimoIncluido));
}

bool AzarConSemilla::evaluarProbabilidadPorcentual(double porcentajeDeExito) noexcept {
    if (porcentajeDeExito <= porcentajeMinimoValido) {
        return false;
    }
    if (porcentajeDeExito >= porcentajeMaximoValido) {
        return true;
    }
    return generarDecimalEnRango(0.0, 100.0) <= porcentajeDeExito;
}

void AzarConSemilla::barajarListaDeCasillas(
        std::vector<CoordenadaCasilla>& listaDeCasillas) noexcept {
    int tamanoLista = static_cast<int>(listaDeCasillas.size());
    if (tamanoLista <= 1) {
        return;
    }
    for (int indice = tamanoLista - 1; indice >= 1; --indice) {
        int indiceAleatorio = generarEnteroEnRango(0, indice);
        std::swap(listaDeCasillas[indice], listaDeCasillas[indiceAleatorio]);
    }
}
