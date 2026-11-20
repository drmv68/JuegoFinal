# ESPECIFICACION TECNICA: cerebro_villano.h / cerebro_villano.cpp

## Archivo de Cabecera: `cerebro_villano.h`

### Guardas de inclusion
```cpp
#ifndef CEREBRO_VILLANO_H
#define CEREBRO_VILLANO_H
```

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro.h"
```

### Constantes constexpr
```cpp
namespace ConstantesCerebroVillano {
    constexpr double PAUSA_ANTES_DE_LA_IRA_SEGUNDOS = 0.8;
    constexpr double DURACION_BASE_DE_LA_IRA_SEGUNDOS = 4.0;
    constexpr double INTERVALO_ENTRE_RAFAGAS_DE_IRA = 10.0;
    constexpr double MULTIPLICADOR_VELOCIDAD_FURIA = 1.05; // 3.15 cas/s
    constexpr double RANGO_VISION_VILLANO = 8.0;
    constexpr double ENFRIAMIENTO_BLOQUEO_PASILLO_SEGUNDOS = 25.0;
}
```

### Declaracion de Clase `CerebroVillano`
```cpp
class CerebroVillano : public CerebroBase {
private:
    double segundosParaLaProximaRafagaDeIra;
    double duracionDeLaIraActivaEnSegundos;
    double segundosDePausaAntesDeLaIra;
    double segundosDeEnfriamientoDelBloqueoDePasillo;

    void alternarEntrePanicoYFuria(PersonajeNoJugable& cuerpo, double segundosTranscurridos);
    CoordenadaCasilla elegirCasillaDeHuidaLejanaDeScooby(const Scooby& scooby, const Mapa& mapa, AzarConSemilla& azar);

public:
    CerebroVillano(double desfaseInicial, double enfriamientoBloqueo);

protected:
    EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) override;
    void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) override;
    void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) override;
    void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) override;
};

#endif
```

---

## Archivo de Implementacion: `cerebro_villano.cpp`

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro_villano.h"
#include "personajes/personaje_no_jugable.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `CerebroBase(desfaseInicial, ConstantesCerebroVillano::RANGO_VISION_VILLANO, 2.6, 3.15)`.
- `segundosParaLaProximaRafagaDeIra = ConstantesCerebroVillano::INTERVALO_ENTRE_RAFAGAS_DE_IRA;`
- `duracionDeLaIraActivaEnSegundos = 0.0;`
- `segundosDePausaAntesDeLaIra = 0.0;`
- `segundosDeEnfriamientoDelBloqueoDePasillo = enfriamientoBloqueo;`

#### 2. `percibirEntorno`
- Embudo de percepcion con rango amplio (8 casillas).
- Consulta `distanciaAlCuadrado`, cono de vision y `lineaDeVista`.

#### 3. `razonarSiguienteDecision`
- Descuenta `segundosDeEnfriamientoDelBloqueoDePasillo -= segundosTranscurridos;`
- Si la partida esta en fase de clímax:
  - Gestiona ciclo huida/furia mediante `alternarEntrePanicoYFuria(cuerpo, segundosTranscurridos);`
  - Retorna.
- Si no esta en clímax (exploracion nocturna):
  - Si ve a Scooby en zona restringida:
    - Incrementa `memoriaPropia.contadorDeAvistamientosEnLaZonaActual++;`
    - Emite aviso con `ordenVillano = true` al buzon.
    - Si `memoriaPropia.contadorDeAvistamientosEnLaZonaActual >= 3 && segundosDeEnfriamientoDelBloqueoDePasillo <= 0.0`:
      - Activa `tienePasoBloqueado` en un pasillo secundario cercano sin encerrar completamente a Scooby.
      - `segundosDeEnfriamientoDelBloqueoDePasillo = ConstantesCerebroVillano::ENFRIAMIENTO_BLOQUEO_PASILLO_SEGUNDOS;`

#### 4. `alternarEntrePanicoYFuria`
- Si `estadoMentalActual == EstadoMentalDelAgente::HuyendoEnPanico`:
  - `segundosParaLaProximaRafagaDeIra -= segundosTranscurridos;`
  - Si `segundosParaLaProximaRafagaDeIra <= 0.0`:
    - `estadoMentalActual = EstadoMentalDelAgente::PreparandoFuria;`
    - `segundosDePausaAntesDeLaIra = ConstantesCerebroVillano::PAUSA_ANTES_DE_LA_IRA_SEGUNDOS;`
    - `cuerpo.establecerIconoSobreLaCabeza(IconoSobreLaCabeza::AuraDeFuria);`
- Si `estadoMentalActual == EstadoMentalDelAgente::PreparandoFuria`:
  - `segundosDePausaAntesDeLaIra -= segundosTranscurridos;`
  - Si `segundosDePausaAntesDeLaIra <= 0.0`:
    - `estadoMentalActual = EstadoMentalDelAgente::FuriaDeContraataque;`
    - `duracionDeLaIraActivaEnSegundos = ConstantesCerebroVillano::DURACION_BASE_DE_LA_IRA_SEGUNDOS;`
- Si `estadoMentalActual == EstadoMentalDelAgente::FuriaDeContraataque`:
  - `duracionDeLaIraActivaEnSegundos -= segundosTranscurridos;`
  - Si `duracionDeLaIraActivaEnSegundos <= 0.0`:
    - `estadoMentalActual = EstadoMentalDelAgente::HuyendoEnPanico;`
    - `segundosParaLaProximaRafagaDeIra = ConstantesCerebroVillano::INTERVALO_ENTRE_RAFAGAS_DE_IRA;`
    - `cuerpo.establecerIconoSobreLaCabeza(IconoSobreLaCabeza::Ninguno);`

#### 5. `elegirCasillaDeHuidaLejanaDeScooby`
- Evalua candidatos transitables en direccion opuesta a Scooby.
- Retorna la casilla con mayor distancia a Scooby que mantenga ruta transitable hacia ella.

#### 6. `ejecutarMovimientoOAccion`
- Si `estadoMentalActual == EstadoMentalDelAgente::PreparandoFuria`: frena (pausa de furia).
- Si `estadoMentalActual == EstadoMentalDelAgente::FuriaDeContraataque`:
  - Persigue a Scooby a `velocidadDePersecucion` (3.15 cas/s).
- Si `estadoMentalActual == EstadoMentalDelAgente::HuyendoEnPanico`:
  - Huye hacia la casilla seleccionada de huida lejana.
