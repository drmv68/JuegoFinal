# ESPECIFICACION TECNICA: objeto_con_fisica.h / objeto_con_fisica.cpp

## Archivo y Ubicacion
`logica/personajes/objeto_con_fisica.h`
`logica/personajes/objeto_con_fisica.cpp`

## Proposito y Responsabilidad Unica
Entidad del mapa cuyo movimiento o animacion espacial esta gobernado por un modelo analitico derivado de `MovimientoBase` (lampara oscilante de taberna con `PenduloAmortiguado` o bala balistica con `TiroParabolico`). Es dueña exclusiva de su puntero de fisica.

## Dependencias
### Para objeto_con_fisica.h
- `#include "personajes/elemento_del_mapa.h"`
- `#include "fisicas/movimiento.h"`
- `#include <cstdint>`

### Para objeto_con_fisica.cpp
- `#include "personajes/objeto_con_fisica.h"`
- `#include <cassert>`

## Definicion Exacta de Cabecera (objeto_con_fisica.h)

```cpp
#ifndef OBJETO_CON_FISICA_H
#define OBJETO_CON_FISICA_H

#include "personajes/elemento_del_mapa.h"
#include "fisicas/movimiento.h"
#include <cstdint>

enum class TipoDeFisicaAsociada : uint8_t {
    LamparaOscilanteTaberna,
    BalaDeCanonMuralla
};

class ObjetoConFisica final : public ElementoDelMapaBase {
public:
    static constexpr double anchoColisionObjetoConFisica = 0.50;
    static constexpr double altoColisionObjetoConFisica = 0.50;

    ObjetoConFisica(TipoDeFisicaAsociada tipoFisica, double posicionInicialX, double posicionInicialY, MovimientoBase* modeloFisicoDueno);
    ~ObjetoConFisica() override;

    ObjetoConFisica(const ObjetoConFisica&) = delete;
    ObjetoConFisica& operator=(const ObjetoConFisica&) = delete;

    void avanzarUnCuadro(double segundosTranscurridos) override;

    TipoDeFisicaAsociada obtenerTipoDeFisica() const;
    const MovimientoBase* obtenerModeloFisicoObservador() const;
    MovimientoBase* modificarModeloFisico();

    double obtenerAlturaZ() const;
    bool estaEnMomentoClaveDeFisica() const;

private:
    TipoDeFisicaAsociada tipoFisica;
    MovimientoBase* modeloFisico;
    double alturaZ;
};

#endif
```

## Algoritmo Paso a Paso (objeto_con_fisica.cpp)

### Constructor
1. `assert(modeloFisicoDueno != nullptr);`
2. Invocar `ElementoDelMapaBase(posicionInicialX, posicionInicialY, anchoColisionObjetoConFisica, altoColisionObjetoConFisica)`.
3. Asignar `tipoFisica = tipoFisica; modeloFisico = modeloFisicoDueno; alturaZ = 0.0;`.

### Destructor
1. Si `modeloFisico != nullptr`:
   - `delete modeloFisico;`
   - `modeloFisico = nullptr;`

### avanzarUnCuadro(double segundosTranscurridos)
1. Si `modeloFisico != nullptr && modeloFisico->obtenerEstaSimulacionActiva()`:
   - `CoordenadaFisicaTresEjes coordenadas = modeloFisico->avanzarYCalcularPosicion(segundosTranscurridos);`
   - `posicionX = coordenadas.posicionX;`
   - `posicionY = coordenadas.posicionY;`
   - `alturaZ = coordenadas.alturaZ;`
   - Si `tipoFisica == TipoDeFisicaAsociada::BalaDeCanonMuralla && modeloFisico->estaEnMomentoClave()`:
     - `estaActivo = false;` (impacto contra el suelo terminado).

### estaEnMomentoClaveDeFisica() const
1. Si `modeloFisico != nullptr`:
   - Retornar `modeloFisico->estaEnMomentoClave();`
2. Retornar `false`.
