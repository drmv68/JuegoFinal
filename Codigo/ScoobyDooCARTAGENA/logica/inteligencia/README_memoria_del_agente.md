# ESPECIFICACION TECNICA: memoria_del_agente.h

## Archivo de Cabecera: `memoria_del_agente.h` (Solo .h)

### Guardas de inclusion
```cpp
#ifndef MEMORIA_DEL_AGENTE_H
#define MEMORIA_DEL_AGENTE_H
```

### Inclusiones requeridas
```cpp
#include <vector>
#include "mapa/mapa.h"
```

### Estructura `MemoriaDelAgente`
```cpp
struct MemoriaDelAgente {
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

    MemoriaDelAgente()
        : indiceDelPuntoObjetivoActual(0)
        , usaPatrullaLibreAleatoria(false)
        , destinoAleatorioActual{0, 0}
        , recuerdaUltimaPosicionDeScooby(false)
        , ultimaCasillaDondeVioAScooby{0, 0}
        , ultimoArbustoDondeSeEscondio{-1, -1}
        , vecesEscondidoEnElMismoArbusto(0)
        , contadorDeAvistamientosEnLaZonaActual(0)
        , segundosParaOlvidarLaSospecha(0.0)
        , yaEntregoLaPistaPrincipal(false)
        , segundosDeEnfriamientoDelServicio(0.0) {}
};

#endif
```

### Reglas de Diseño
1. Estructura liviana de datos planos sin memoria dinamica propia durante ejecucion.
2. `vecesEscondidoEnElMismoArbusto`: si llega a 3, el agente ya sabe que Scooby esta en ese arbusto e ignora el camuflaje.
