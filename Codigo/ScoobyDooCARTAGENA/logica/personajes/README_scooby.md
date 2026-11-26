# ESPECIFICACION TECNICA: scooby.h / scooby.cpp

## Archivo y Ubicacion
`logica/personajes/scooby.h`
`logica/personajes/scooby.cpp`

## Proposito y Responsabilidad Unica
Entidad controlada por el jugador. Administra estamina de sprint, penalizaciones de velocidad por contacto, camuflaje temporal en arbustos, proteccion de Daphne y desplazamiento por ejes independientes contra paredes.

## Dependencias
### Para scooby.h
- `#include "personajes/elemento_del_mapa.h"`
- `#include "juego/teclado.h"`
- `#include <vector>`

### Declaraciones adelantadas en scooby.h
```cpp
class Mapa;
```

### Para scooby.cpp
- `#include "personajes/scooby.h"`
- `#include "mapa/mapa.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <cmath>`

## Definicion Exacta de Cabecera (scooby.h)

```cpp
#ifndef SCOOBY_H
#define SCOOBY_H

#include "personajes/elemento_del_mapa.h"
#include "juego/teclado.h"
#include <vector>

class Mapa;

enum class ConsecuenciaDelToque : uint8_t {
    SoloRalentizado,
    ActivarAlertaGlobal,
    Captura
};

class Scooby final : public ElementoDelMapaBase {
public:
    static constexpr double velocidadBaseEnCasillasPorSegundo = 3.3;
    static constexpr double alientoMaximoEnSegundos = 3.0;
    static constexpr double multiplicadorVelocidadSprint = 1.6;
    static constexpr double factorRalentizacionPorContacto = 0.67;
    static constexpr double duracionSprintInfinitoPorFrutaEnSegundos = 20.0;
    static constexpr double duracionRalentizacionPorContactoEnSegundos = 3.0;
    static constexpr double segundosParaOlvidarToquesAcumulados = 20.0;
    static constexpr double segundosDeInmunidadTrasToque = 1.5;
    static constexpr double segundosMaximosDeCamuflajeEnArbusto = 4.5;
    static constexpr double opacidadCamuflado = 0.45;
    static constexpr double opacidadCompletamenteVisible = 1.0;
    static constexpr double tasaRecuperacionAlientoPorSegundo = 0.5;
    static constexpr double velocidadDeslizamientoEmpuje = 6.0;
    static constexpr double factorAmortiguacionDeslizamiento = 0.85;
    static constexpr int cantidadMaximaDeProvisionesCargadas = 2;
    static constexpr double anchoColisionScooby = 0.70;
    static constexpr double altoColisionScooby = 0.70;

    Scooby(double posicionInicialX, double posicionInicialY);
    ~Scooby() override = default;

    Scooby(const Scooby&) = delete;
    Scooby& operator=(const Scooby&) = delete;

    void avanzarUnCuadro(double segundosTranscurridos) override;
    void moverSegunTeclado(const EstadoDeTeclado& teclado, double segundosTranscurridos, const Mapa& mapa);
    void actualizarTemporizadoresDeEstado(double segundosTranscurridos);

    ConsecuenciaDelToque recibirToqueDeSecuaz(bool partidaEnPanicoClimax);
    void aplicarEmpujeDeFantasma(double origenFantasmaX, double origenFantasmaY);
    void activarEsconditeDeDaphne(double segundosDeProteccion);
    void activarSprintInfinitoPorFruta();
    void ralentizarPorToque(double duracionSegundos);

    double obtenerOpacidadVisual() const;
    bool estaVisibleParaLosEnemigos() const;
    double obtenerAlientoActual() const;
    bool obtenerEstaEnSprint() const;
    int obtenerContadorDeToquesDeSecuaz() const;
    int obtenerProvisionesCargadas() const;

    bool cargarProvision();
    int descargarProvisionesCompletas();

    const std::vector<int>& obtenerEvidenciasEnLaMano() const;
    void agregarEvidencia(int codigoEvidencia);
    void entregarEvidencias();

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
};

#endif
```

## Algoritmo Paso a Paso (scooby.cpp)

### Constructor
1. Invocar constructor base: `ElementoDelMapaBase(posicionInicialX, posicionInicialY, anchoColisionScooby, altoColisionScooby)`.
2. Asignar `alientoActual = alientoMaximoEnSegundos; estaEnSprint = false;`.
3. Asignar `segundosDeSprintInfinitoRestantes = 0.0; segundosDeRalentizacionRestantes = 0.0;`.
4. Asignar `contadorDeToquesDeSecuaz = 0; segundosDesdeElUltimoToque = 0.0; segundosDeInmunidadRestantes = 0.0;`.
5. Asignar `estaOcultoEnArbusto = false; segundosEscondidoEnElArbustoActual = 0.0; segundosDeEsconditeDeDaphneRestantes = 0.0;`.
6. Asignar `velocidadDeDeslizamientoX = 0.0; velocidadDeDeslizamientoY = 0.0; provisionesCargadas = 0;`.

### avanzarUnCuadro(double segundosTranscurridos)
1. Aplicar velocidad residual de deslizamiento:
   - `posicionX += velocidadDeDeslizamientoX * segundosTranscurridos;`
   - `posicionY += velocidadDeDeslizamientoY * segundosTranscurridos;`
   - Amortiguar: `velocidadDeDeslizamientoX *= factorAmortiguacionDeslizamiento; velocidadDeDeslizamientoY *= factorAmortiguacionDeslizamiento;`.
2. Invocar `actualizarTemporizadoresDeEstado(segundosTranscurridos);`.

### actualizarTemporizadoresDeEstado(double segundosTranscurridos)
1. Si `segundosDeSprintInfinitoRestantes > 0.0`:
   - `segundosDeSprintInfinitoRestantes -= segundosTranscurridos;`
   - Si `segundosDeSprintInfinitoRestantes < 0.0`, asignar `segundosDeSprintInfinitoRestantes = 0.0;`.
2. Si `segundosDeRalentizacionRestantes > 0.0`:
   - `segundosDeRalentizacionRestantes -= segundosTranscurridos;`
   - Si `segundosDeRalentizacionRestantes < 0.0`, asignar `segundosDeRalentizacionRestantes = 0.0;`.
3. Si `segundosDeInmunidadRestantes > 0.0`:
   - `segundosDeInmunidadRestantes -= segundosTranscurridos;`
   - Si `segundosDeInmunidadRestantes < 0.0`, asignar `segundosDeInmunidadRestantes = 0.0;`.
4. Si `segundosDeEsconditeDeDaphneRestantes > 0.0`:
   - `segundosDeEsconditeDeDaphneRestantes -= segundosTranscurridos;`
   - Si `segundosDeEsconditeDeDaphneRestantes < 0.0`, asignar `segundosDeEsconditeDeDaphneRestantes = 0.0;`.
5. Sumar `segundosDesdeElUltimoToque += segundosTranscurridos;`.
6. Si `segundosDesdeElUltimoToque >= segundosParaOlvidarToquesAcumulados`, asignar `contadorDeToquesDeSecuaz = 0;`.
7. Si `estaOcultoEnArbusto`:
   - Sumar `segundosEscondidoEnElArbustoActual += segundosTranscurridos;`.
   - Si `segundosEscondidoEnElArbustoActual >= segundosMaximosDeCamuflajeEnArbusto`:
     - `estaOcultoEnArbusto = false;`

### moverSegunTeclado(const EstadoDeTeclado& teclado, double segundosTranscurridos, const Mapa& mapa)
1. Determinar direccion segun entradas:
   - `vectorX = 0.0; vectorY = 0.0;`
   - Si `teclado.arriba`, `vectorY -= 1.0;`
   - Si `teclado.abajo`, `vectorY += 1.0;`
   - Si `teclado.izquierda`, `vectorX -= 1.0;`
   - Si `teclado.derecha`, `vectorX += 1.0;`
2. Si `vectorX == 0.0 && vectorY == 0.0`, terminar ejecucion.
3. Actualizar `direccionMirada`:
   - Si `std::fabs(vectorX) >= std::fabs(vectorY)`: `direccionMirada = (vectorX > 0.0) ? DireccionMirada::Este : DireccionMirada::Oeste;`
   - En caso contrario: `direccionMirada = (vectorY > 0.0) ? DireccionMirada::Sur : DireccionMirada::Norte;`
4. Normalizar vector diagonal dividiendo por `std::sqrt((vectorX * vectorX) + (vectorY * vectorY))`.
5. Administrar estamina de sprint:
   - Si `segundosDeSprintInfinitoRestantes > 0.0`:
     - `estaEnSprint = teclado.correrActivo;`
   - Si no:
     - Si `teclado.correrActivo && alientoActual > 0.0`:
       - `estaEnSprint = true;`
       - `alientoActual -= segundosTranscurridos;`
       - Si `alientoActual < 0.0`, asignar `alientoActual = 0.0; estaEnSprint = false;`.
     - Si no esta corriendo:
       - `estaEnSprint = false;`
       - `alientoActual = CalculosEspaciales::mantenerValorDentroDelRango(alientoActual + (tasaRecuperacionAlientoPorSegundo * segundosTranscurridos), 0.0, alientoMaximoEnSegundos);`
6. Determinar velocidad resultante:
   - `velocidadCalculada = velocidadBaseEnCasillasPorSegundo;`
   - Si `estaEnSprint`, `velocidadCalculada *= multiplicadorVelocidadSprint;`
   - Si `segundosDeRalentizacionRestantes > 0.0`, `velocidadCalculada *= factorRalentizacionPorContacto;`
7. Validar movimiento por ejes separados (deslizamiento suave):
   - Probar eje horizontal: `candidataX = posicionX + (vectorX * velocidadCalculada * segundosTranscurridos);`
   - Si `mapa.esPosicionTransitable(candidataX, posicionY, true)`, asignar `posicionX = candidataX;`.
   - Probar eje vertical: `candidataY = posicionY + (vectorY * velocidadCalculada * segundosTranscurridos);`
   - Si `mapa.esPosicionTransitable(posicionX, candidataY, true)`, asignar `posicionY = candidataY;`.
8. Comprobar permanencia en arbusto:
   - `CoordenadaCasilla celda = obtenerCasillaActual();`
   - `CasillaDelMapa casilla = mapa.obtenerCasilla(celda.columna, celda.fila);`
   - Si `casilla.esArbustoParaOcultarse == 1u`:
     - Si `segundosEscondidoEnElArbustoActual < segundosMaximosDeCamuflajeEnArbusto`:
       - `estaOcultoEnArbusto = true;`
   - En caso contrario:
     - `estaOcultoEnArbusto = false; segundosEscondidoEnElArbustoActual = 0.0;`

### recibirToqueDeSecuaz(bool partidaEnPanicoClimax)
1. Si `segundosDeInmunidadRestantes > 0.0`, retornar `ConsecuenciaDelToque::SoloRalentizado`.
2. Asignar `segundosDeInmunidadRestantes = segundosDeInmunidadTrasToque;`.
3. Asignar `segundosDesdeElUltimoToque = 0.0;`.
4. Si `partidaEnPanicoClimax`, retornar `ConsecuenciaDelToque::Captura`.
5. Incrementar `contadorDeToquesDeSecuaz++`.
6. Si `contadorDeToquesDeSecuaz == 1`:
   - `segundosDeRalentizacionRestantes = duracionRalentizacionPorContactoEnSegundos;`
   - Retornar `ConsecuenciaDelToque::SoloRalentizado`.
7. Si `contadorDeToquesDeSecuaz == 2`:
   - Retornar `ConsecuenciaDelToque::ActivarAlertaGlobal`.
8. Retornar `ConsecuenciaDelToque::Captura`.

### aplicarEmpujeDeFantasma(double origenFantasmaX, double origenFantasmaY)
1. `anguloEmpuje = CalculosEspaciales::calcularAnguloEntrePuntosEnRadianes(origenFantasmaX, origenFantasmaY, posicionX, posicionY);`
2. `velocidadDeDeslizamientoX = std::cos(anguloEmpuje) * velocidadDeslizamientoEmpuje;`
3. `velocidadDeDeslizamientoY = std::sin(anguloEmpuje) * velocidadDeslizamientoEmpuje;`

### obtenerOpacidadVisual() const
1. Si `estaOcultoEnArbusto || segundosDeEsconditeDeDaphneRestantes > 0.0`, retornar `opacidadCamuflado`.
2. Retornar `opacidadCompletamenteVisible`.

### estaVisibleParaLosEnemigos() const
1. Retornar `!(estaOcultoEnArbusto || segundosDeEsconditeDeDaphneRestantes > 0.0)`.
