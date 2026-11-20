# ESPECIFICACION TECNICA: cerebro_secuaz.h / cerebro_secuaz.cpp

## Archivo de Cabecera: `cerebro_secuaz.h`

### Guardas de inclusion
```cpp
#ifndef CEREBRO_SECUAZ_H
#define CEREBRO_SECUAZ_H
```

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro.h"
```

### Constantes constexpr
```cpp
namespace ConstantesCerebroSecuaz {
    constexpr double APERTURA_CONO_VISION_RADIANES = 1.3962634; // ~80 grados
    constexpr double DURACION_PAUSA_ESQUINA_SEGUNDOS = 1.2;
    constexpr double DISTANCIA_DETECCION_ARBUSTO_CERCANO = 2.0;
}
```

### Declaracion de Clase `CerebroSecuaz`
```cpp
class CerebroSecuaz : public CerebroBase {
private:
    double segundosParaConfirmarLaSospecha;
    int casillasAdyacentesRevisadas;
    bool movimientoLoControlaLaFisica;

    void avanzarHaciaCasillaEsquivandoObstaculos(PersonajeNoJugable& cuerpo, CoordenadaCasilla destino, const Mapa& mapa, double segundosTranscurridos);

public:
    CerebroSecuaz(double desfaseInicial, double segSospecha, double rangoVision, bool porFisica);

protected:
    EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) override;
    void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) override;
    void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) override;
    void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) override;
};

#endif
```

---

## Archivo de Implementacion: `cerebro_secuaz.cpp`

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro_secuaz.h"
#include "personajes/personaje_no_jugable.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `CerebroBase(desfaseInicial, rangoVision, 2.4, 3.0)`.
- `segundosParaConfirmarLaSospecha = segSospecha;`
- `casillasAdyacentesRevisadas = 0;`
- `movimientoLoControlaLaFisica = porFisica;`

#### 2. `percibirEntorno`
- Embudo de percepcion en 3 filtros:
  1. Filtro 1 (Distancia al cuadrado):
     - `double distCuad = CalculosEspaciales::calcularDistanciaAlCuadrado(cuerpo.obtenerPosicionX(), cuerpo.obtenerPosicionY(), scooby.obtenerPosicionX(), scooby.obtenerPosicionY());`
     - Si `distCuad > (rangoDeVisionEnCasillas * rangoDeVisionEnCasillas)`: no ve a Scooby.
  2. Filtro 2 (Cono de vision):
     - Evalua `CalculosEspaciales::estaPuntoDentroDelConoDeVision(cuerpo.obtenerPosicionX(), cuerpo.obtenerPosicionY(), cuerpo.obtenerDireccionMirada(), scooby.obtenerPosicionX(), scooby.obtenerPosicionY(), ConstantesCerebroSecuaz::APERTURA_CONO_VISION_RADIANES)`.
     - Si esta fuera del cono: no ve a Scooby.
  3. Filtro 3 (Linea de vista y camuflaje):
     - Si `!scooby.estaVisibleParaLosEnemigos()`:
       - Solo lo detecta si `memoriaPropia.vecesEscondidoEnElMismoArbusto >= 3` o si lo vio entrar a menos de `ConstantesCerebroSecuaz::DISTANCIA_DETECCION_ARBUSTO_CERCANO` casillas.
     - Valida `mapa.tieneLineaDeVista(cuerpo.obtenerCasillaActual(), scooby.obtenerCasillaActual())`.
- Buzon: consulta `buzon.buscarAvisoCercanoParaElAgente(cuerpo.obtenerCasillaActual(), aviso)`.
- Retorna `EstimuloPercibido`.

#### 3. `razonarSiguienteDecision`
- Si `movimientoLoControlaLaFisica` (Centinela): solo actualiza icono a exclamacion si `estimulo.veAScooby` y publica aviso al buzon; no persigue.
- Si `estimulo.veAScooby`:
  - Si `estadoMentalActual != EstadoMentalDelAgente::PersiguiendoObjetivo && estadoMentalActual != EstadoMentalDelAgente::SospechandoConDuda`:
    - `estadoMentalActual = EstadoMentalDelAgente::SospechandoConDuda;`
    - `segundosRestantesEnElEstadoActual = segundosParaConfirmarLaSospecha;`
    - `cuerpo.establecerIconoSobreLaCabeza(IconoSobreLaCabeza::SignoDeInterrogacion);`
  - Si `estadoMentalActual == EstadoMentalDelAgente::SospechandoConDuda && segundosRestantesEnElEstadoActual <= 0.0`:
    - `estadoMentalActual = EstadoMentalDelAgente::PersiguiendoObjetivo;`
    - `cuerpo.establecerIconoSobreLaCabeza(IconoSobreLaCabeza::SignoDeExclamacion);`
    - `buzon.publicarNuevoAviso(cuerpo.obtenerNumeroDeSerie(), estimulo.casillaDondeLoVio, false);`
- Si no ve a Scooby:
  - Si estaba persiguiendo: pasa a `EstadoMentalDelAgente::BuscandoEnUltimaPosicion;`
  - Si estaba buscando y llego a la casilla: tras 1.5 s de rastreo vuelve a `PatrullandoSector;`
  - Si recibe aviso de alerta y esta patrullando: acude a la casilla del aviso.

#### 4. `ejecutarMovimientoOAccion`
- Si `movimientoLoControlaLaFisica` retorna (el centinela se mueve por ecuacion parametrica).
- Segun `estadoMentalActual`:
  - `PatrullandoSector`: velocidad al 80% (2.4 cas/s). Recorre `listaDePuntosDeRutaFija` o sortea casilla libre si usa patrulla libre. Pausa de 1.2 s en cada punto de giro mirando a ambos lados.
  - `PersiguiendoObjetivo`: velocidad al 100% (3.0 cas/s). Llama a `avanzarHaciaCasillaEsquivandoObstaculos` con la casilla actual de Scooby.
  - `BuscandoEnUltimaPosicion`: camina hacia `memoriaPropia.ultimaCasillaDondeVioAScooby`.

#### 5. `avanzarHaciaCasillaEsquivandoObstaculos`
- Obtiene siguiente casilla mediante `mapa.calcularSiguientePasoHacia(cuerpo.obtenerCasillaActual(), destino, true)`.
- Llama a `cuerpo.moverHacia(siguiente.columna + 0.5, siguiente.fila + 0.5, mapa, segundosTranscurridos);`
