# ESPECIFICACION TECNICA: trampa.h / trampa.cpp

## Archivo de Cabecera: `trampa.h`

### Guardas de inclusion
```cpp
#ifndef TRAMPA_H
#define TRAMPA_H
```

### Inclusiones requeridas
```cpp
#include "personajes/elemento_del_mapa.h"
#include "personajes/scooby.h"
#include "personajes/personaje_no_jugable.h"
```

### Enumeracion
```cpp
enum class EstadoDeLaTrampa {
    OcultaSinArmar,
    ArmadaInvisible,
    DisparadaConCaptura
};
```

### Constantes constexpr
```cpp
namespace ConstantesTrampa {
    constexpr double MITAD_DEL_LADO_DE_LA_ZONA_EN_CASILLAS = 2.5;
    constexpr double SEGUNDOS_NECESARIOS_PARA_ARMAR = 3.5;
    constexpr double DISTANCIA_MAXIMA_PARA_INICIAR_ARMADO = 1.5;
    constexpr double ANCHO_COLISION_TRAMPA = 5.0;
    constexpr double ALTO_COLISION_TRAMPA = 5.0;
}
```

### Declaracion de Clase `Trampa`
```cpp
class Trampa : public ElementoDelMapaBase {
private:
    EstadoDeLaTrampa estadoActual;
    double segundosAcumuladosParaArmar;
    bool villanoCayoEnLaTrampa;
    bool scoobyCayoEnLaTrampa;

public:
    Trampa(double posX, double posY);

    void avanzarUnCuadro(double segundosTranscurridos) override;

    EstadoDeLaTrampa obtenerEstadoActual() const;
    double obtenerSegundosAcumuladosParaArmar() const;
    double obtenerProgresoDeArmadoNormalizado() const;
    bool obtenerVillanoCayoEnLaTrampa() const;
    bool obtenerScoobyCayoEnLaTrampa() const;

    bool armarSiScoobyMantieneLaInteraccion(const Scooby& scooby, bool interaccionSostenida, double segundosTranscurridos);
    void revisarSiScoobyOElVillanoPisanLaZona(const Scooby& scooby, const PersonajeNoJugable& villano);
    void forzarDisparoTrampa();
};

#endif
```

---

## Archivo de Implementacion: `trampa.cpp`

### Inclusiones requeridas
```cpp
#include "personajes/trampa.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `ElementoDelMapaBase(posX, posY, ConstantesTrampa::ANCHO_COLISION_TRAMPA, ConstantesTrampa::ALTO_COLISION_TRAMPA)`.
- `estadoActual = EstadoDeLaTrampa::OcultaSinArmar;`
- `segundosAcumuladosParaArmar = 0.0;`
- `villanoCayoEnLaTrampa = false; scoobyCayoEnLaTrampa = false;`
- `esVisible = true; estaActivo = true;`

#### 2. `avanzarUnCuadro`
- Si `estadoActual == EstadoDeLaTrampa::ArmadaInvisible`:
  - `esVisible = false;` (la trampa permanece invisible mientras esta armada esperando).

#### 3. `armarSiScoobyMantieneLaInteraccion`
- Si `estadoActual != EstadoDeLaTrampa::OcultaSinArmar` retorna `false`.
- `double distEjeX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, scooby.obtenerPosicionX());`
- `double distEjeY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, scooby.obtenerPosicionY());`
- Si `distEjeX <= ConstantesTrampa::DISTANCIA_MAXIMA_PARA_INICIAR_ARMADO && distEjeY <= ConstantesTrampa::DISTANCIA_MAXIMA_PARA_INICIAR_ARMADO && interaccionSostenida`:
  - `segundosAcumuladosParaArmar += segundosTranscurridos;`
  - Si `segundosAcumuladosParaArmar >= ConstantesTrampa::SEGUNDOS_NECESARIOS_PARA_ARMAR`:
    - `estadoActual = EstadoDeLaTrampa::ArmadaInvisible;`
    - `esVisible = false;`
    - Retorna `true;`
- Si no mantiene la interaccion:
  - `segundosAcumuladosParaArmar = 0.0;`
- Retorna `false;`

#### 4. `revisarSiScoobyOElVillanoPisanLaZona`
- Si `estadoActual != EstadoDeLaTrampa::ArmadaInvisible` retorna.
- Comprobacion Villano:
  - Si `villano.obtenerEstaActivo()`:
    - `double vDistX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, villano.obtenerPosicionX());`
    - `double vDistY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, villano.obtenerPosicionY());`
    - Si `vDistX <= ConstantesTrampa::MITAD_DEL_LADO_DE_LA_ZONA_EN_CASILLAS && vDistY <= ConstantesTrampa::MITAD_DEL_LADO_DE_LA_ZONA_EN_CASILLAS`:
      - `villanoCayoEnLaTrampa = true;`
      - `estadoActual = EstadoDeLaTrampa::DisparadaConCaptura;`
      - `esVisible = true;`
      - Retorna;
- Comprobacion Scooby:
  - `double sDistX = CalculosEspaciales::calcularDistanciaEnUnEje(posicionX, scooby.obtenerPosicionX());`
  - `double sDistY = CalculosEspaciales::calcularDistanciaEnUnEje(posicionY, scooby.obtenerPosicionY());`
  - Si `sDistX <= ConstantesTrampa::MITAD_DEL_LADO_DE_LA_ZONA_EN_CASILLAS && sDistY <= ConstantesTrampa::MITAD_DEL_LADO_DE_LA_ZONA_EN_CASILLAS`:
    - `scoobyCayoEnLaTrampa = true;`
    - `estadoActual = EstadoDeLaTrampa::DisparadaConCaptura;`
    - `esVisible = true;`

#### 5. `obtenerProgresoDeArmadoNormalizado`
- Retorna `CalculosEspaciales::mantenerValorDentroDelRango(segundosAcumuladosParaArmar / ConstantesTrampa::SEGUNDOS_NECESARIOS_PARA_ARMAR, 0.0, 1.0);`
