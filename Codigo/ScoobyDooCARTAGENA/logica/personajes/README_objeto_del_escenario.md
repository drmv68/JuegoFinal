# ESPECIFICACION TECNICA: objeto_del_escenario.h / objeto_del_escenario.cpp

## Archivo de Cabecera: `objeto_del_escenario.h`

### Guardas de inclusion
```cpp
#ifndef OBJETO_DEL_ESCENARIO_H
#define OBJETO_DEL_ESCENARIO_H
```

### Inclusiones requeridas
```cpp
#include "personajes/elemento_del_mapa.h"
#include "fisicas/movimiento.h"
#include "personajes/scooby.h"
```

### Enumeracion
```cpp
enum class CategoriaDeObjeto {
    PistaDeLugar,
    EvidenciaDeSospechoso,
    MarcaEnSuelo,
    ProvisionDeSuministro,
    FrutaSprintInfinito,
    GemaDeIntercambio,
    RelojTiempoExtra,
    LamparaOscilante,
    BalaDeCanon
};
```

### Constantes constexpr
```cpp
namespace ConstantesObjeto {
    constexpr double SEGUNDOS_DE_VIDA_DEL_RELOJ_EXTRA = 6.5;
    constexpr double SEGUNDOS_DE_VIDA_DE_LA_FRUTA = 12.0;
    constexpr double SEGUNDOS_DE_VIDA_DE_LA_PROVISION = 20.0;
    constexpr double DISTANCIA_PARA_REVELAR_MARCA_EN_CASILLAS = 2.0;
    constexpr double ANCHO_COLISION_OBJETO = 0.60;
    constexpr double ALTO_COLISION_OBJETO = 0.60;
}
```

### Declaracion de Clase `ObjetoDelEscenario`
```cpp
class ObjetoDelEscenario : public ElementoDelMapaBase {
private:
    CategoriaDeObjeto categoria;
    bool fueRecolectado;
    double segundosDeVidaRestante;
    MovimientoBase* movimientoMatematico;
    double puntoDeImpactoX;
    double puntoDeImpactoY;
    int codigoDeEvidencia;
    int codigoDeLaMisionAsociada;

public:
    ObjetoDelEscenario(CategoriaDeObjeto cat, double posX, double posY);
    ~ObjetoDelEscenario() override;

    void avanzarUnCuadro(double segundosTranscurridos) override;

    CategoriaDeObjeto obtenerCategoria() const;
    bool obtenerFueRecolectado() const;
    void establecerFueRecolectado(bool recolectado);

    void asignarMovimientoMatematico(MovimientoBase* movimiento);
    MovimientoBase* obtenerMovimientoMatematico() const;

    void establecerPuntoDeImpacto(double impactoX, double impactoY);
    CoordenadaFisicaTresEjes obtenerPuntoDeImpactoPrevisto() const;
    double obtenerProgresoDeVueloNormalizado() const;

    int obtenerCodigoDeEvidencia() const;
    void establecerCodigoDeEvidencia(int codigo);

    int obtenerCodigoDeLaMisionAsociada() const;
    void establecerCodigoDeLaMisionAsociada(int codigoMision);

    bool esRecolectablePorScooby() const;
    bool estaLamparaEnAnguloMaximo() const;
    void revelarSiScoobyEstaCerca(const Scooby& scooby);
};

#endif
```

---

## Archivo de Implementacion: `objeto_del_escenario.cpp`

### Inclusiones requeridas
```cpp
#include "personajes/objeto_del_escenario.h"
#include "herramientas/calculos_espaciales.h"
#include "fisicas/tiro_parabolico.h"
#include "fisicas/pendulo_amortiguado.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `ElementoDelMapaBase(posX, posY, ConstantesObjeto::ANCHO_COLISION_OBJETO, ConstantesObjeto::ALTO_COLISION_OBJETO)`.
- `categoria = cat; fueRecolectado = false; movimientoMatematico = nullptr;`
- `codigoDeEvidencia = -1; codigoDeLaMisionAsociada = -1;`
- `puntoDeImpactoX = posX; puntoDeImpactoY = posY;`
- Asignacion de tiempo de vida:
  - Caso `RelojTiempoExtra`: `segundosDeVidaRestante = ConstantesObjeto::SEGUNDOS_DE_VIDA_DEL_RELOJ_EXTRA;`
  - Caso `FrutaSprintInfinito`: `segundosDeVidaRestante = ConstantesObjeto::SEGUNDOS_DE_VIDA_DE_LA_FRUTA;`
  - Caso `ProvisionDeSuministro`: `segundosDeVidaRestante = ConstantesObjeto::SEGUNDOS_DE_VIDA_DE_LA_PROVISION;`
  - Caso `MarcaEnSuelo`: `segundosDeVidaRestante = -1.0; esVisible = false;` (nace oculta).
  - Otros casos: `segundosDeVidaRestante = -1.0;` (permanente).

#### 2. Destructor
- Si `movimientoMatematico != nullptr`, ejecuta `delete movimientoMatematico; movimientoMatematico = nullptr;`

#### 3. `avanzarUnCuadro`
- Si `movimientoMatematico != nullptr`:
  - `CoordenadaFisicaTresEjes coord = movimientoMatematico->avanzarYCalcularPosicion(segundosTranscurridos);`
  - `posicionX = coord.posicionX; posicionY = coord.posicionY;`
- Si `segundosDeVidaRestante > 0.0`:
  - `segundosDeVidaRestante -= segundosTranscurridos;`
  - Si `segundosDeVidaRestante <= 0.0`:
    - `estaActivo = false; esVisible = false;`

#### 4. `esRecolectablePorScooby`
- Retorna `estaActivo && !fueRecolectado && (categoria == CategoriaDeObjeto::ProvisionDeSuministro || categoria == CategoriaDeObjeto::FrutaSprintInfinito || categoria == CategoriaDeObjeto::GemaDeIntercambio || categoria == CategoriaDeObjeto::RelojTiempoExtra || categoria == CategoriaDeObjeto::PistaDeLugar || categoria == CategoriaDeObjeto::EvidenciaDeSospechoso);`

#### 5. `estaLamparaEnAnguloMaximo`
- Si `categoria == CategoriaDeObjeto::LamparaOscilante && movimientoMatematico != nullptr`:
  - Retorna `movimientoMatematico->estaEnMomentoClave();`
- Retorna `false;`

#### 6. `revelarSiScoobyEstaCerca`
- Si `categoria == CategoriaDeObjeto::MarcaEnSuelo && !esVisible`:
  - `double distCuadrada = CalculosEspaciales::calcularDistanciaAlCuadrado(posicionX, posicionY, scooby.obtenerPosicionX(), scooby.obtenerPosicionY());`
  - `double radioCuadrado = ConstantesObjeto::DISTANCIA_PARA_REVELAR_MARCA_EN_CASILLAS * ConstantesObjeto::DISTANCIA_PARA_REVELAR_MARCA_EN_CASILLAS;`
  - Si `distCuadrada <= radioCuadrado`, `esVisible = true;`
