# ESPECIFICACION TECNICA: memoria_del_agente.h / memoria_del_agente.cpp

## Archivo y Ubicacion
`logica/inteligencia/memoria_del_agente.h`
`logica/inteligencia/memoria_del_agente.cpp`

## Proposito y Responsabilidad Unica
Clase de encapsulamiento estricto que resguarda los recuerdos, puntos de patrulla, cache de ruta A* e historicos de sospecha de un agente autonomo, protegiendo sus invariantes contra corrupcion externa.

## Dependencias
### Para memoria_del_agente.h
- `#include <vector>`
- `#include "herramientas/tipos_espaciales.h"`

### Para memoria_del_agente.cpp
- `#include "inteligencia/memoria_del_agente.h"`

## Definicion Exacta de Cabecera (memoria_del_agente.h)

```cpp
#ifndef MEMORIA_DEL_AGENTE_H
#define MEMORIA_DEL_AGENTE_H

#include <vector>
#include "herramientas/tipos_espaciales.h"

class MemoriaDelAgente final {
public:
    static constexpr int limiteEsconditesEnMismoArbustoParaCaptura = 3;
    static constexpr int indiceInicialPuntosPatrulla = 0;
    static constexpr double tiempoDesvanecimientoSospechaPorDefectoEnSegundos = 15.0;

    MemoriaDelAgente();
    ~MemoriaDelAgente() = default;

    MemoriaDelAgente(const MemoriaDelAgente&) = default;
    MemoriaDelAgente& operator=(const MemoriaDelAgente&) = default;

    void asignarPuntosDeRutaFija(const std::vector<CoordenadaCasilla>& puntos);
    void avanzarAlSiguientePuntoDePatrulla();
    CoordenadaCasilla obtenerPuntoDePatrullaObjetivoActual() const;
    bool tieneRutaFijaDefinida() const;

    void establecerUsaPatrullaLibre(bool usaLibre);
    bool obtenerUsaPatrullaLibre() const;
    void establecerDestinoAleatorioActual(CoordenadaCasilla destino);
    CoordenadaCasilla obtenerDestinoAleatorioActual() const;

    void registrarAvistamientoDeScooby(CoordenadaCasilla casilla);
    void olvidarUltimaPosicionDeScooby();
    bool tieneRecuerdoDeScooby() const;
    CoordenadaCasilla obtenerUltimaCasillaDondeVioAScooby() const;

    void registrarEsconditeEnArbusto(CoordenadaCasilla casillaArbusto);
    bool superoLimiteDeEsconditesEnArbusto(CoordenadaCasilla casillaArbusto) const;
    void limpiarHistoricoDeArbustos();

    void incrementarContadorDeAvistamientosEnZona();
    int obtenerContadorDeAvistamientosEnZona() const;
    void reiniciarContadorDeAvistamientosEnZona();
    void actualizarTemporizadorDeSospecha(double segundosTranscurridos);

    void establecerPistaPrincipalEntregada(bool entregada);
    bool obtenerYaEntregoPistaPrincipal() const;

    void activarEnfriamientoDeServicio(double segundos);
    void descontarEnfriamientoDeServicio(double segundosTranscurridos);
    bool estaServicioDisponible() const;

    void establecerCacheRutaAEstrella(const std::vector<CoordenadaCasilla>& ruta);
    bool tienePasosEnCacheRutaAEstrella() const;
    CoordenadaCasilla extraerSiguientePasoDeCacheRutaAEstrella();
    void limpiarCacheRutaAEstrella();

private:
    std::vector<CoordenadaCasilla> listaDePuntosDeRutaFija;
    int indiceDelPuntoObjetivoActual;
    bool usaPatrullaLibreAleatoria;
    CoordenadaCasilla destinoAleatorioActual;

    bool recuerdaUltimaPosicionDeScooby;
    CoordenadaCasilla ultimaCasillaDondeVioAScooby;
    CoordenadaCasilla ultimoArbustoDondeSeEscondio;
    int vecesEscondidoEnElMismoArbusto;

    int contadorDeAvistamientosEnLaZonaActual;
    double segundosParaOlvidarLaSospecha;

    bool yaEntregoLaPistaPrincipal;
    double segundosDeEnfriamientoDelServicio;

    std::vector<CoordenadaCasilla> cacheRutaAEstrella;
};

#endif
```

## Algoritmo Paso a Paso (memoria_del_agente.cpp)

### Constructor
1. Asignar `indiceDelPuntoObjetivoActual = indiceInicialPuntosPatrulla; usaPatrullaLibreAleatoria = false;`.
2. Asignar `destinoAleatorioActual = CoordenadaCasilla{0, 0};`.
3. Asignar `recuerdaUltimaPosicionDeScooby = false; ultimaCasillaDondeVioAScooby = CoordenadaCasilla{0, 0};`.
4. Asignar `ultimoArbustoDondeSeEscondio = CoordenadaCasilla{-1, -1}; vecesEscondidoEnElMismoArbusto = 0;`.
5. Asignar `contadorDeAvistamientosEnLaZonaActual = 0; segundosParaOlvidarLaSospecha = 0.0;`.
6. Asignar `yaEntregoLaPistaPrincipal = false; segundosDeEnfriamientoDelServicio = 0.0;`.

### registrarEsconditeEnArbusto(CoordenadaCasilla casillaArbusto)
1. Si `ultimoArbustoDondeSeEscondio == casillaArbusto`:
   - `vecesEscondidoEnElMismoArbusto++;`
2. Si no:
   - `ultimoArbustoDondeSeEscondio = casillaArbusto;`
   - `vecesEscondidoEnElMismoArbusto = 1;`

### superoLimiteDeEsconditesEnArbusto(CoordenadaCasilla casillaArbusto) const
1. Retornar `(ultimoArbustoDondeSeEscondio == casillaArbusto) && (vecesEscondidoEnElMismoArbusto >= limiteEsconditesEnMismoArbustoParaCaptura)`.

### avanzarAlSiguientePuntoDePatrulla()
1. Si `listaDePuntosDeRutaFija.empty()`, terminar.
2. `indiceDelPuntoObjetivoActual = (indiceDelPuntoObjetivoActual + 1) % static_cast<int>(listaDePuntosDeRutaFija.size());`

### Cache de Ruta A*
1. `establecerCacheRutaAEstrella`: asigna el vector recibido a `cacheRutaAEstrella`.
2. `tienePasosEnCacheRutaAEstrella`: retorna `!cacheRutaAEstrella.empty()`.
3. `extraerSiguientePasoDeCacheRutaAEstrella`: toma el primer elemento, lo remueve con `erase(begin())` y lo retorna.
