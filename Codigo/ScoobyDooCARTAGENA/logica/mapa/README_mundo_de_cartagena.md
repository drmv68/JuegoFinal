# ESPECIFICACION TECNICA: mundo_de_cartagena.h / mundo_de_cartagena.cpp

## Estado Actual de Implementacion (Pendiente de Dependencias)
Este modulo se deja **pendiente intencionalmente** durante la construccion de `logica/mapa/` y se conserva este README intacto hasta finalizar las carpetas `personajes/`, `inteligencia/`, `investigacion/` y `juego/`.

**Por que se pospone:**
- `mundo_de_cartagena.h` depende directamente de cabeceras aun vacias (`inteligencia/avisos_entre_agentes.h`, `juego/teclado.h` e `investigacion/misiones/mision.h`).
- `mundo_de_cartagena.cpp` instancia, actualiza y destruye entidades concretas de `personajes/`, cerebros de `inteligencia/` y parametros de `juego/dificultad.h`.
- Implementarlo antes de construir esas capas impediria compilar y probar el proyecto paso a paso sin errores; al completarse dichas dependencias, se retomara esta especificacion en su propia sub-rama para compilar e integrar todo limpiamente.

## Archivo y Ubicacion
`logica/mapa/mundo_de_cartagena.h`
`logica/mapa/mundo_de_cartagena.cpp`

## Proposito y Responsabilidad Unica
Clase propietaria absoluta del estado espacial de la partida (mapa, jugador, personajes, objetos recolectables, objetos con fisica, trampa y buzon de avisos). Encapsula colisiones, movimiento, generacion de comodines y cañonazos de muralla.

## Dependencias
### Para mundo_de_cartagena.h
- `#include <vector>`
- `#include <QString>`
- `#include "mapa/mapa.h"`
- `#include "inteligencia/avisos_entre_agentes.h"`
- `#include "juego/teclado.h"`
- `#include "herramientas/azar_con_semilla.h"`
- `#include "investigacion/misiones/mision.h"`

### Declaraciones adelantadas en mundo_de_cartagena.h
```cpp
class Scooby;
class PersonajeNoJugable;
class ObjetoRecolectable;
class ObjetoConFisica;
class Trampa;
struct ParametrosDeDificultad;
```

### Para mundo_de_cartagena.cpp
- `#include "mapa/mundo_de_cartagena.h"`
- `#include "personajes/scooby.h"`
- `#include "personajes/personaje_no_jugable.h"`
- `#include "personajes/objeto_recolectable.h"`
- `#include "personajes/objeto_con_fisica.h"`
- `#include "personajes/trampa.h"`
- `#include "fisicas/tiro_parabolico.h"`
- `#include "fisicas/pendulo_amortiguado.h"`
- `#include "fisicas/movimiento_circular.h"`
- `#include "inteligencia/cerebro_ciudadano_y_amigo.h"`
- `#include "inteligencia/cerebro_secuaz.h"`
- `#include "inteligencia/cerebro_villano.h"`
- `#include "juego/dificultad.h"`

## Definicion Exacta de Cabecera (mundo_de_cartagena.h)

```cpp
#ifndef MUNDO_DE_CARTAGENA_H
#define MUNDO_DE_CARTAGENA_H

#include <vector>
#include <QString>
#include "mapa/mapa.h"
#include "inteligencia/avisos_entre_agentes.h"
#include "juego/teclado.h"
#include "herramientas/azar_con_semilla.h"
#include "investigacion/misiones/mision.h"

class Scooby;
class PersonajeNoJugable;
class ObjetoRecolectable;
class ObjetoConFisica;
class Trampa;
struct ParametrosDeDificultad;

enum class ResultadoColisionConEnemigo {
    SinContacto,
    ScoobyRalentizado,
    AlertaGlobalDisparada,
    ScoobyCapturado
};

class MundoDeCartagena final {
public:
    static constexpr double distanciaMinimaComodinEnCasillas = 4.0;
    static constexpr double distanciaMaximaComodinEnCasillas = 8.0;
    static constexpr double intervaloMinimoComodinEnSegundos = 40.0;
    static constexpr double intervaloMaximoComodinEnSegundos = 120.0;
    static constexpr double duracionVidaRelojEnSegundos = 6.5;
    static constexpr double duracionVidaFrutaEnSegundos = 12.0;
    static constexpr double duracionVidaProvisionEnSegundos = 20.0;
    static constexpr double velocidadDisparoCanonMuralla = 18.0;
    static constexpr double anguloElevacionCanonRadianes = 0.785398;
    static constexpr double gravedadSimuladaCanones = 9.81;

    MundoDeCartagena();
    ~MundoDeCartagena();

    MundoDeCartagena(const MundoDeCartagena&) = delete;
    MundoDeCartagena& operator=(const MundoDeCartagena&) = delete;

    void inicializarMundoVacio();
    void liberarRecursosEspaciales();

    void avanzarSimulacionDelMundo(double segundosTranscurridos, const EstadoDeTeclado& teclado, bool esDeNoche, bool estaEnPanicoClimax, AzarConSemilla& azar, const ParametrosDeDificultad& parametros);

    ResultadoColisionConEnemigo verificarColisionesConEnemigos(bool estaEnPanicoClimax);
    int resolverRecoleccionDeObjetos();
    bool verificarInteraccionConAlmacen(double radioMaximoEnCasillas);

    void aplicarEfectoDeMision(const EfectoDeMisionEnElMundo& efecto);
    void generarComodinCercanoAScooby(AzarConSemilla& azar);
    void gestionarDisparoDeMuralla(double segundosTranscurridos, AzarConSemilla& azar, const ParametrosDeDificultad& parametros, bool modoClimaxInterior);

    RectanguloVision armarRectanguloDeVisionDeScooby() const;

    const Mapa& obtenerMapa() const;
    Mapa& modificarMapa();

    const Scooby* obtenerScooby() const;
    Scooby* modificarScooby();

    const Trampa* obtenerTrampa() const;
    Trampa* modificarTrampa();

    const BuzonCompartidoDeAvisos& obtenerBuzonDeAvisos() const;
    BuzonCompartidoDeAvisos& modificarBuzonDeAvisos();

    const std::vector<PersonajeNoJugable*>& obtenerPersonajes() const;
    const std::vector<ObjetoRecolectable*>& obtenerObjetosRecolectables() const;
    const std::vector<ObjetoConFisica*>& obtenerObjetosConFisica() const;

    void agregarPersonaje(PersonajeNoJugable* personajeDueno);
    void agregarObjetoRecolectable(ObjetoRecolectable* objetoDueno);
    void agregarObjetoConFisica(ObjetoConFisica* objetoDueno);
    void establecerTrampa(Trampa* trampaDueno);
    void establecerScooby(Scooby* scoobyDueno);

    const ObjetoConFisica* obtenerLamparaDeLaTaberna() const;
    const PersonajeNoJugable* obtenerVillano() const;

private:
    Mapa mapa;
    BuzonCompartidoDeAvisos buzonDeAvisos;
    Scooby* jugadorScooby;
    Trampa* trampaDelEscenario;
    std::vector<PersonajeNoJugable*> listaDePersonajes;
    std::vector<ObjetoRecolectable*> listaDeObjetosRecolectables;
    std::vector<ObjetoConFisica*> listaDeObjetosConFisica;

    double segundosParaSiguienteComodin;
    double segundosParaSiguienteDisparoCanon;
    CoordenadaCasilla casillaAlmacen;
    std::vector<CoordenadaCasilla> casillasCanonesMuralla;
};

#endif
```

## Algoritmo Paso a Paso (mundo_de_cartagena.cpp)

### Constructor
1. Inicializar punteros a `nullptr`: `jugadorScooby = nullptr; trampaDelEscenario = nullptr;`.
2. Asignar `segundosParaSiguienteComodin = intervaloMinimoComodinEnSegundos; segundosParaSiguienteDisparoCanon = 10.0;`.

### Destructor
1. Invocar `liberarRecursosEspaciales()`.

### liberarRecursosEspaciales()
1. Si `jugadorScooby != nullptr`: ejecutar `delete jugadorScooby; jugadorScooby = nullptr;`.
2. Si `trampaDelEscenario != nullptr`: ejecutar `delete trampaDelEscenario; trampaDelEscenario = nullptr;`.
3. Para cada puntero `personaje` en `listaDePersonajes`: ejecutar `delete personaje;`.
4. `listaDePersonajes.clear();`.
5. Para cada puntero `objeto` en `listaDeObjetosRecolectables`: ejecutar `delete objeto;`.
6. `listaDeObjetosRecolectables.clear();`.
7. Para cada puntero `objetoFisico` en `listaDeObjetosConFisica`: ejecutar `delete objetoFisico;`.
8. `listaDeObjetosConFisica.clear();`.

### avanzarSimulacionDelMundo
1. Descontar vigencia en el buzon: `buzonDeAvisos.descontarTiempoDeVigencia(segundosTranscurridos)`.
2. Si `jugadorScooby != nullptr`:
   - `jugadorScooby->moverSegunTeclado(teclado, segundosTranscurridos, mapa)`.
   - `jugadorScooby->avanzarUnCuadro(segundosTranscurridos)`.
3. Obtener rectangulo de vision de Scooby: `RectanguloVision vision = armarRectanguloDeVisionDeScooby()`.
4. Para cada `personaje` en `listaDePersonajes`:
   - Si no esta activo, continuar.
   - `personaje->actualizarPresenciaPorMomentoDelDia(esDeNoche, vision, mapa, azar)`.
   - `personaje->pensarYActuar(*jugadorScooby, mapa, buzonDeAvisos, azar, segundosTranscurridos)`.
   - `personaje->avanzarUnCuadro(segundosTranscurridos)`.
5. Iterar sobre `listaDeObjetosRecolectables` de fin a inicio:
   - `objeto->avanzarUnCuadro(segundosTranscurridos)`.
   - `objeto->revelarSiScoobyEstaCerca(*jugadorScooby)`.
   - Si `!objeto->obtenerEstaActivo()`: aplicar Swap-and-Pop `std::swap(listaDeObjetosRecolectables[i], listaDeObjetosRecolectables.back()); delete listaDeObjetosRecolectables.back(); listaDeObjetosRecolectables.pop_back();`.
6. Iterar sobre `listaDeObjetosConFisica` de fin a inicio:
   - `objetoFisico->avanzarUnCuadro(segundosTranscurridos)`.
   - Si `!objetoFisico->obtenerEstaActivo()`: aplicar Swap-and-Pop, liberar y retirar.
7. Si `trampaDelEscenario != nullptr`:
   - `trampaDelEscenario->avanzarUnCuadro(segundosTranscurridos)`.

### verificarColisionesConEnemigos
1. Si `jugadorScooby == nullptr` retornar `ResultadoColisionConEnemigo::SinContacto`.
2. Para cada `personaje` en `listaDePersonajes`:
   - Si `!personaje->obtenerEstaActivo()` continuar.
   - Si `personaje->obtenerRol() == TipoRolDelPersonaje::SecuazVigilante || personaje->obtenerRol() == TipoRolDelPersonaje::CentinelaAlmacen || personaje->obtenerRol() == TipoRolDelPersonaje::VillanoFantasma`:
     - Si `jugadorScooby->colisionaCon(*personaje)`:
       - Si es Villano en furia o modo pánico: retornar `ResultadoColisionConEnemigo::ScoobyCapturado`.
       - Si es Secuaz:
         - `ConsecuenciaDelToque resultado = jugadorScooby->recibirToqueDeSecuaz(estaEnPanicoClimax)`.
         - Si `resultado == ConsecuenciaDelToque::SoloRalentizado`, retornar `ResultadoColisionConEnemigo::ScoobyRalentizado`.
         - Si `resultado == ConsecuenciaDelToque::ActivarAlertaGlobal`, retornar `ResultadoColisionConEnemigo::AlertaGlobalDisparada`.
         - Si `resultado == ConsecuenciaDelToque::Captura`, retornar `ResultadoColisionConEnemigo::ScoobyCapturado`.
3. Retornar `ResultadoColisionConEnemigo::SinContacto`.

### resolverRecoleccionDeObjetos
1. Retorna el codigo de evidencia o mision si un objeto de tipo evidencia o pista fue recolectado.
2. Itera sobre `listaDeObjetosRecolectables`: si `objeto->esRecolectablePorScooby() && jugadorScooby->colisionaCon(*objeto)`:
   - `objeto->establecerFueRecolectado(true)`.
   - `objeto->establecerActivo(false)`.
   - Retorna su codigo numerico asociado.

### aplicarEfectoDeMision
1. Evalua `efecto.tipo`:
   - `AbrirPuerta`: `mapa.cambiarBloqueoDeCasilla(efecto.casilla.columna, efecto.casilla.fila, false)`.
   - `CerrarPuerta`: `mapa.cambiarBloqueoDeCasilla(efecto.casilla.columna, efecto.casilla.fila, true)`.
   - `CrearObjetoEnElMapa`: instancia nuevo `ObjetoRecolectable` en `efecto.casilla` y lo agrega a `listaDeObjetosRecolectables`.
   - `AlertarZona`: `buzonDeAvisos.activarAlertaGlobal(15.0)`.
   - `MostrarMarcaEnSuelo`: recorre objetos recolectables buscando marcas en esa casilla y las hace visibles.
