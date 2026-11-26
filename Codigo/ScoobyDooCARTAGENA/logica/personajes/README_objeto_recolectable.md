# ESPECIFICACION TECNICA: objeto_recolectable.h / objeto_recolectable.cpp

## Archivo y Ubicacion
`logica/personajes/objeto_recolectable.h`
`logica/personajes/objeto_recolectable.cpp`

## Proposito y Responsabilidad Unica
Entidad pasiva recolectable o marca estatica en el suelo (pistas, evidencias de identidad, sacos de suministros, fruta de sprint, gemas de trueque, relojes de tiempo extra y marcas de tierra removida).

## Dependencias
### Para objeto_recolectable.h
- `#include "personajes/elemento_del_mapa.h"`
- `#include <cstdint>`

### Declaraciones adelantadas en objeto_recolectable.h
```cpp
class Scooby;
```

### Para objeto_recolectable.cpp
- `#include "personajes/objeto_recolectable.h"`
- `#include "personajes/scooby.h"`
- `#include "herramientas/calculos_espaciales.h"`

## Definicion Exacta de Cabecera (objeto_recolectable.h)

```cpp
#ifndef OBJETO_RECOLECTABLE_H
#define OBJETO_RECOLECTABLE_H

#include "personajes/elemento_del_mapa.h"
#include <cstdint>

class Scooby;

enum class CategoriaDeObjetoRecolectable : uint8_t {
    PistaDeLugar,
    EvidenciaDeSospechoso,
    MarcaEnSuelo,
    ProvisionDeSuministro,
    FrutaSprintInfinito,
    GemaDeIntercambio,
    RelojTiempoExtra
};

class ObjetoRecolectable final : public ElementoDelMapaBase {
public:
    static constexpr double duracionDeVidaPermanenteEnSegundos = -1.0;
    static constexpr int codigoIdentificadorNoAsignado = -1;
    static constexpr double distanciaParaRevelarMarcaEnCasillas = 2.0;
    static constexpr double anchoColisionObjeto = 0.60;
    static constexpr double altoColisionObjeto = 0.60;

    ObjetoRecolectable(CategoriaDeObjetoRecolectable categoriaObjeto, double posicionInicialX, double posicionInicialY, double tiempoVidaSegundos = duracionDeVidaPermanenteEnSegundos, int codigoAsociado = codigoIdentificadorNoAsignado);
    ~ObjetoRecolectable() override = default;

    ObjetoRecolectable(const ObjetoRecolectable&) = delete;
    ObjetoRecolectable& operator=(const ObjetoRecolectable&) = delete;

    void avanzarUnCuadro(double segundosTranscurridos) override;

    CategoriaDeObjetoRecolectable obtenerCategoria() const;
    bool obtenerFueRecolectado() const;
    void establecerFueRecolectado(bool estadoRecolectado);

    int obtenerCodigoAsociado() const;
    void establecerCodigoAsociado(int codigo);

    bool esRecolectablePorScooby() const;
    void revelarSiScoobyEstaCerca(const Scooby& scooby);

private:
    CategoriaDeObjetoRecolectable categoria;
    bool fueRecolectado;
    double segundosDeVidaRestantes;
    int codigoIdentificadorAsociado;
};

#endif
```

## Algoritmo Paso a Paso (objeto_recolectable.cpp)

### Constructor
1. Invocar `ElementoDelMapaBase(posicionInicialX, posicionInicialY, anchoColisionObjeto, altoColisionObjeto)`.
2. Asignar `categoria = categoriaObjeto; fueRecolectado = false;`.
3. Asignar `segundosDeVidaRestantes = tiempoVidaSegundos;`.
4. Asignar `codigoIdentificadorAsociado = codigoAsociado;`.
5. Si `categoria == CategoriaDeObjetoRecolectable::MarcaEnSuelo`:
   - Asignar `esVisible = false;` (nace oculta en la tierra).

### avanzarUnCuadro(double segundosTranscurridos)
1. Si `segundosDeVidaRestantes > 0.0`:
   - `segundosDeVidaRestantes -= segundosTranscurridos;`
   - Si `segundosDeVidaRestantes <= 0.0`:
     - `segundosDeVidaRestantes = 0.0;`
     - `estaActivo = false; esVisible = false;`

### esRecolectablePorScooby() const
1. Retornar `estaActivo && !fueRecolectado && (categoria != CategoriaDeObjetoRecolectable::MarcaEnSuelo)`.

### revelarSiScoobyEstaCerca(const Scooby& scooby)
1. Si `categoria == CategoriaDeObjetoRecolectable::MarcaEnSuelo && !esVisible`:
   - `distanciaAlCuadrado = CalculosEspaciales::calcularDistanciaAlCuadrado(posicionX, posicionY, scooby.obtenerPosicionX(), scooby.obtenerPosicionY());`
   - `umbralAlCuadrado = distanciaParaRevelarMarcaEnCasillas * distanciaParaRevelarMarcaEnCasillas;`
   - Si `distanciaAlCuadrado <= umbralAlCuadrado`:
     - `esVisible = true;`
