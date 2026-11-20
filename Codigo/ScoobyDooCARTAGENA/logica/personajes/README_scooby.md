# ESPECIFICACION TECNICA: scooby.h / scooby.cpp

## Archivo de Cabecera: `scooby.h`

### Guardas de inclusion
```cpp
#ifndef SCOOBY_H
#define SCOOBY_H
```

### Inclusiones requeridas
```cpp
#include "personajes/elemento_del_mapa.h"
#include "juego/teclado.h"
#include <vector>
```

### Enumeracion de Consecuencia
```cpp
enum class ConsecuenciaDelToque {
    SoloRalentizado,
    ActivarAlertaGlobal,
    Captura
};
```

### Constantes constexpr
```cpp
namespace ConstantesScooby {
    constexpr double VELOCIDAD_BASE_EN_CASILLAS_POR_SEGUNDO = 3.3;
    constexpr double ALIENTO_MAXIMO_EN_SEGUNDOS = 3.0;
    constexpr double MULTIPLICADOR_DE_SPRINT = 1.6;
    constexpr double SEGUNDOS_DE_SPRINT_INFINITO = 20.0;
    constexpr double FACTOR_DE_RALENTIZACION = 0.67;
    constexpr double SEGUNDOS_PARA_OLVIDAR_TOQUES = 20.0;
    constexpr double SEGUNDOS_DE_INMUNIDAD_TRAS_TOQUE = 1.5;
    constexpr double SEGUNDOS_MAXIMOS_DE_CAMUFLAJE_EN_ARBUSTO = 4.5;
    constexpr double OPACIDAD_OCULTO = 0.45;
    constexpr double OPACIDAD_NORMAL = 1.0;
    constexpr int CANTIDAD_MAXIMA_DE_PROVISIONES_CARGADAS = 2;
    constexpr double TASA_RECUPERACION_ALIENTO_CAMINANDO = 0.5;
    constexpr double ALIENTO_MINIMO_PARA_VOLVER_A_CORRER = 1.0;
    constexpr double ANCHO_COLISION_SCOOBY = 0.70;
    constexpr double ALTO_COLISION_SCOOBY = 0.70;
}
```

### Declaracion de Clase `Scooby`
```cpp
class Scooby : public ElementoDelMapaBase {
private:
    double alientoActual;
    bool estaEnSprint;
    double segundosDeSprintInfinitoRestantes;
    double segundosDeRalentizacionRestantes;
    int contadorDeToquesDeSecuaz;
    double segundosDesdeElUltimoToque;
    double segundosDeInmunidadRestantes;
    bool estaOcultoEnArbusto;
    double segundosEscondidoEnElArbustoActual;
    double segundosDeEsconditeDeDaphneRestantes;
    double velocidadDeDeslizamientoX;
    double velocidadDeDeslizamientoY;
    int provisionesCargadas;
    std::vector<int> evidenciasEnLaMano;

public:
    Scooby(double posX, double posY);

    void avanzarUnCuadro(double segundosTranscurridos) override;
    void moverSegunTeclado(const EstadoDeTeclado& teclado, double segundosTranscurridos, const Mapa& mapa);
    void actualizarTemporizadoresDeEstado(double segundosTranscurridos);
    ConsecuenciaDelToque recibirToqueDeSecuaz(bool partidaEnPanico);
    void aplicarEmpujeDeFantasma(double origenX, double origenY);
    void activarEsconditeDeDaphne(double segundos);
    void activarSprintInfinitoPorFruta(double segundos);
    void ralentizar(double duracionSegundos);

    double obtenerOpacidadVisual() const;
    bool estaVisibleParaLosEnemigos() const;

    double obtenerAlientoActual() const;
    bool obtenerEstaEnSprint() const;
    int obtenerContadorDeToquesDeSecuaz() const;
    int obtenerProvisionesCargadas() const;
    bool cargarProvision();
    int descargarProvisiones();

    const std::vector<int>& obtenerEvidenciasEnLaMano() const;
    void agregarEvidencia(int codigoEvidencia);
    void entregarEvidencias();
};

#endif
```

---

## Archivo de Implementacion: `scooby.cpp`

### Inclusiones requeridas
```cpp
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
#include <cmath>
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `ElementoDelMapaBase(posX, posY, ConstantesScooby::ANCHO_COLISION_SCOOBY, ConstantesScooby::ALTO_COLISION_SCOOBY)`.
- `alientoActual = ConstantesScooby::ALIENTO_MAXIMO_EN_SEGUNDOS;`
- Variables de temporizador e indicadores en 0.0 / false / 0.

#### 2. `avanzarUnCuadro`
- Descuenta deslizamiento residual de clímax:
  - `posicionX += velocidadDeDeslizamientoX * segundosTranscurridos;`
  - `posicionY += velocidadDeDeslizamientoY * segundosTranscurridos;`
  - Frena deslizamiento gradualmente (`velocidadDeDeslizamientoX *= 0.85; velocidadDeDeslizamientoY *= 0.85;`).
- Llama a `actualizarTemporizadoresDeEstado(segundosTranscurridos);`

#### 3. `actualizarTemporizadoresDeEstado`
- Descuenta `segundosDeSprintInfinitoRestantes`.
- Descuenta `segundosDeRalentizacionRestantes`.
- Descuenta `segundosDeInmunidadRestantes`.
- Descuenta `segundosDeEsconditeDeDaphneRestantes`.
- Aumenta `segundosDesdeElUltimoToque += segundosTranscurridos;`
- Si `segundosDesdeElUltimoToque >= ConstantesScooby::SEGUNDOS_PARA_OLVIDAR_TOQUES`, `contadorDeToquesDeSecuaz = 0;`
- Si `estaOcultoEnArbusto`:
  - `segundosEscondidoEnElArbustoActual += segundosTranscurridos;`
  - Si `segundosEscondidoEnElArbustoActual >= ConstantesScooby::SEGUNDOS_MAXIMOS_DE_CAMUFLAJE_EN_ARBUSTO`, `estaOcultoEnArbusto = false;`

#### 4. `moverSegunTeclado`
- Determina vector direccion `(dirX, dirY)` segun `teclado.arriba, teclado.abajo, teclado.izquierda, teclado.derecha`.
- Normaliza vector si es diagonal: `magnitud = std::sqrt(dirX * dirX + dirY * dirY)`. Si `magnitud > 0.0`, `dirX /= magnitud; dirY /= magnitud;`
- Actualiza `direccionMirada` segun direccion dominante.
- Evalua sprint:
  - `bool intentaCorrer = teclado.correrActivo;`
  - Si `segundosDeSprintInfinitoRestantes > 0.0`: `estaEnSprint = intentaCorrer;`
  - Si no: corre solo si `intentaCorrer && alientoActual > 0.0`. Si corre, `alientoActual -= segundosTranscurridos;` si no corre, recupera `alientoActual += ConstantesScooby::TASA_RECUPERACION_ALIENTO_CAMINANDO * segundosTranscurridos;`
  - Acota aliento entre `0.0` y `ConstantesScooby::ALIENTO_MAXIMO_EN_SEGUNDOS`.
- Calcula velocidad efectiva:
  - `vel = ConstantesScooby::VELOCIDAD_BASE_EN_CASILLAS_POR_SEGUNDO;`
  - Si `estaEnSprint`: `vel *= ConstantesScooby::MULTIPLICADOR_DE_SPRINT;`
  - Si `segundosDeRalentizacionRestantes > 0.0`: `vel *= ConstantesScooby::FACTOR_DE_RALENTIZACION;`
- Movimiento por ejes (deslizamiento en esquinas):
  - `nuevaX = posicionX + dirX * vel * segundosTranscurridos;`
  - Si `mapa.esPosicionTransitable(nuevaX, posicionY, true)`: `posicionX = nuevaX;`
  - `nuevaY = posicionY + dirY * vel * segundosTranscurridos;`
  - Si `mapa.esPosicionTransitable(posicionX, nuevaY, true)`: `posicionY = nuevaY;`
- Actualiza deteccion de arbusto:
  - `CoordenadaCasilla cas = obtenerCasillaActual();`
  - `bool pisandoArbusto = mapa.obtenerCasilla(cas.columna, cas.fila).esArbustoParaOcultarse;`
  - Si `pisandoArbusto && segundosEscondidoEnElArbustoActual < ConstantesScooby::SEGUNDOS_MAXIMOS_DE_CAMUFLAJE_EN_ARBUSTO`, `estaOcultoEnArbusto = true;`
  - Si no pisa arbusto, `estaOcultoEnArbusto = false; segundosEscondidoEnElArbustoActual = 0.0;`

#### 5. `recibirToqueDeSecuaz`
- Si `segundosDeInmunidadRestantes > 0.0` retorna `ConsecuenciaDelToque::SoloRalentizado`.
- `segundosDeInmunidadRestantes = ConstantesScooby::SEGUNDOS_DE_INMUNIDAD_TRAS_TOQUE;`
- `segundosDesdeElUltimoToque = 0.0;`
- Si `partidaEnPanico`: retorna `ConsecuenciaDelToque::Captura;`
- `contadorDeToquesDeSecuaz++;`
- Si `contadorDeToquesDeSecuaz == 1`:
  - `segundosDeRalentizacionRestantes = 3.0;`
  - Retorna `ConsecuenciaDelToque::SoloRalentizado;`
- Si `contadorDeToquesDeSecuaz == 2`:
  - Retorna `ConsecuenciaDelToque::ActivarAlertaGlobal;`
- Retorna `ConsecuenciaDelToque::Captura;`

#### 6. `aplicarEmpujeDeFantasma`
- Calcula vector desde fantasma `(origenX, origenY)` hacia Scooby:
  - `ang = std::atan2(posicionY - origenY, posicionX - origenX);`
  - `velocidadDeDeslizamientoX = std::cos(ang) * 6.0;`
  - `velocidadDeDeslizamientoY = std::sin(ang) * 6.0;`

#### 7. `obtenerOpacidadVisual`
- Si `estaOcultoEnArbusto || segundosDeEsconditeDeDaphneRestantes > 0.0` retorna `ConstantesScooby::OPACIDAD_OCULTO;`
- Retorna `ConstantesScooby::OPACIDAD_NORMAL;`

#### 8. `estaVisibleParaLosEnemigos`
- Retorna `!(estaOcultoEnArbusto || segundosDeEsconditeDeDaphneRestantes > 0.0);`
