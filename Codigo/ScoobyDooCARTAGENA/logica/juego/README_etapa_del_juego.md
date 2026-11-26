# ESPECIFICACION TECNICA: etapa_del_juego.h

## Archivo y Ubicacion
`logica/juego/etapa_del_juego.h` (Archivo exclusivo de cabecera).

## Proposito y Responsabilidad Unica
Define la maquina de estados finita de la sesion de juego. Desacopla la etapa de la clase `ControladorDelJuego` para prevenir dependencias circulares.

## Dependencias
- `#include <cstdint>`

## Definicion Exacta de Cabecera (etapa_del_juego.h)

```cpp
#ifndef ETAPA_DEL_JUEGO_H
#define ETAPA_DEL_JUEGO_H

#include <cstdint>

enum class EtapaDelJuego : uint8_t {
    MenuPrincipal,
    TutorialNivelUno,
    ExploracionNivelDos,
    ClimaxTrampaArmada,
    Pausa,
    Derrota,
    Victoria
};

enum class TipoDeEventoVisual : uint8_t {
    ObjetoRecogido,
    PistaDescubierta,
    MisionCompletada,
    ScoobyRalentizado,
    AlertaGlobalActivada,
    CanonDisparado,
    CanonImpactoCercano,
    TrampaArmada,
    CambioDeMomentoDelDia,
    AcusacionAcertada,
    AcusacionFallida,
    ScoobyCapturado,
    VillanoAtrapado
};

#endif
```
