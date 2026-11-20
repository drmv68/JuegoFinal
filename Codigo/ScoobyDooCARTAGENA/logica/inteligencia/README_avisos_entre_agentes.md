# ESPECIFICACION TECNICA: avisos_entre_agentes.h

## Archivo de Cabecera: `avisos_entre_agentes.h` (Solo .h)

### Guardas de inclusion
```cpp
#ifndef AVISOS_ENTRE_AGENTES_H
#define AVISOS_ENTRE_AGENTES_H
```

### Inclusiones requeridas
```cpp
#include "mapa/mapa.h"
#include "herramientas/calculos_espaciales.h"
```

### Constantes constexpr
```cpp
namespace ConstantesAvisos {
    constexpr int CAPACIDAD_MAXIMA_AVISOS = 16;
    constexpr double DISTANCIA_MAXIMA_ATENCION_AVISO = 10.0;
}
```

### Estructura `AvisoDeVigilancia`
```cpp
struct AvisoDeVigilancia {
    int numeroDeSerieDelEmisor;
    CoordenadaCasilla casillaReportada;
    double segundosRestantesDeVigencia;
    bool esOrdenDirectaDelVillano;
};
```

### Estructura `BuzonCompartidoDeAvisos`
```cpp
struct BuzonCompartidoDeAvisos {
    AvisoDeVigilancia listaDeAvisosActivos[ConstantesAvisos::CAPACIDAD_MAXIMA_AVISOS];
    int cantidadDeAvisosActivos;
    double segundosDeAlertaGlobalRestantes;

    BuzonCompartidoDeAvisos()
        : cantidadDeAvisosActivos(0), segundosDeAlertaGlobalRestantes(0.0) {}

    inline void publicarNuevoAviso(int emisor, CoordenadaCasilla casilla, bool ordenVillano, double vigenciaSegundos = 8.0) {
        if (cantidadDeAvisosActivos < ConstantesAvisos::CAPACIDAD_MAXIMA_AVISOS) {
            listaDeAvisosActivos[cantidadDeAvisosActivos] = AvisoDeVigilancia{ emisor, casilla, vigenciaSegundos, ordenVillano };
            cantidadDeAvisosActivos++;
        }
    }

    inline void descontarTiempoDeVigencia(double segundosTranscurridos) {
        if (segundosDeAlertaGlobalRestantes > 0.0) {
            segundosDeAlertaGlobalRestantes -= segundosTranscurridos;
            if (segundosDeAlertaGlobalRestantes < 0.0) {
                segundosDeAlertaGlobalRestantes = 0.0;
            }
        }

        int indiceActual = 0;
        while (indiceActual < cantidadDeAvisosActivos) {
            listaDeAvisosActivos[indiceActual].segundosRestantesDeVigencia -= segundosTranscurridos;
            if (listaDeAvisosActivos[indiceActual].segundosRestantesDeVigencia <= 0.0) {
                listaDeAvisosActivos[indiceActual] = listaDeAvisosActivos[cantidadDeAvisosActivos - 1];
                cantidadDeAvisosActivos--;
            } else {
                indiceActual++;
            }
        }
    }

    inline bool buscarAvisoCercanoParaElAgente(CoordenadaCasilla posicionDelAgente, AvisoDeVigilancia& avisoEncontrado) const {
        for (int i = 0; i < cantidadDeAvisosActivos; ++i) {
            const AvisoDeVigilancia& aviso = listaDeAvisosActivos[i];
            if (aviso.esOrdenDirectaDelVillano || segundosDeAlertaGlobalRestantes > 0.0) {
                avisoEncontrado = aviso;
                return true;
            }
            double distCuad = CalculosEspaciales::calcularDistanciaAlCuadrado(posicionDelAgente.columna, posicionDelAgente.fila, aviso.casillaReportada.columna, aviso.casillaReportada.fila);
            if (distCuad <= (ConstantesAvisos::DISTANCIA_MAXIMA_ATENCION_AVISO * ConstantesAvisos::DISTANCIA_MAXIMA_ATENCION_AVISO)) {
                avisoEncontrado = aviso;
                return true;
            }
        }
        return false;
    }

    inline void activarAlertaGlobal(double duracionEnSegundos) {
        segundosDeAlertaGlobalRestantes = duracionEnSegundos;
    }
};

#endif
```
