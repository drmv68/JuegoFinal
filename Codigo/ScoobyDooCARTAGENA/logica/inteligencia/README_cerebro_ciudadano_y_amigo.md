# ESPECIFICACION TECNICA: cerebro_ciudadano_y_amigo.h / cerebro_ciudadano_y_amigo.cpp

## Archivo de Cabecera: `cerebro_ciudadano_y_amigo.h`

### Guardas de inclusion
```cpp
#ifndef CEREBRO_CIUDADANO_Y_AMIGO_H
#define CEREBRO_CIUDADANO_Y_AMIGO_H
```

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro.h"
```

### Constantes constexpr
```cpp
namespace ConstantesCerebroAmigo {
    constexpr double RADIO_DETECCION_SALUDO_EN_CASILLAS = 3.5;
    constexpr double DISTANCIA_ATENCION_CONVERSACION = 2.5;
}
```

### Declaracion de Clase `CerebroCiudadanoYAmigo`
```cpp
class CerebroCiudadanoYAmigo : public CerebroBase {
private:
    double radioDeDeteccionDeSaludoEnCasillas;
    bool puedeCaminarPorRutaCorta;

public:
    CerebroCiudadanoYAmigo(double desfaseInicial, bool caminar);

protected:
    EstimuloPercibido percibirEntorno(const PersonajeNoJugable& cuerpo, const Scooby& scooby, const Mapa& mapa, const BuzonCompartidoDeAvisos& buzon) override;
    void razonarSiguienteDecision(const EstimuloPercibido& estimulo, PersonajeNoJugable& cuerpo, const Mapa& mapa, BuzonCompartidoDeAvisos& buzon, AzarConSemilla& azar, double segundosTranscurridos) override;
    void ejecutarMovimientoOAccion(PersonajeNoJugable& cuerpo, const Mapa& mapa, AzarConSemilla& azar, double segundosTranscurridos) override;
    void actualizarMemoriaYAprendizaje(const EstimuloPercibido& estimulo, const PersonajeNoJugable& cuerpo) override;
};

#endif
```

---

## Archivo de Implementacion: `cerebro_ciudadano_y_amigo.cpp`

### Inclusiones requeridas
```cpp
#include "inteligencia/cerebro_ciudadano_y_amigo.h"
#include "personajes/personaje_no_jugable.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `CerebroBase(desfaseInicial, ConstantesCerebroAmigo::RADIO_DETECCION_SALUDO_EN_CASILLAS, 1.5, 1.5)`.
- `radioDeDeteccionDeSaludoEnCasillas = ConstantesCerebroAmigo::RADIO_DETECCION_SALUDO_EN_CASILLAS;`
- `puedeCaminarPorRutaCorta = caminar;`

#### 2. `percibirEntorno`
- Mide distancia euclidea al cuadrado hacia Scooby con `CalculosEspaciales::calcularDistanciaAlCuadrado`.
- No traza conos de vision ni consulta el buzon de alertas.
- `double radioCuadrado = radioDeDeteccionDeSaludoEnCasillas * radioDeDeteccionDeSaludoEnCasillas;`
- Retorna `EstimuloPercibido{ distCuad <= radioCuadrado, false, distCuad, scooby.obtenerCasillaActual(), false, {0,0} };`

#### 3. `razonarSiguienteDecision`
- Si `estimulo.distanciaAlCuadradoHaciaScooby <= (ConstantesCerebroAmigo::DISTANCIA_ATENCION_CONVERSACION * ConstantesCerebroAmigo::DISTANCIA_ATENCION_CONVERSACION)`:
  - `estadoMentalActual = EstadoMentalDelAgente::PausaDeVigilancia;`
- Si no:
  - `estadoMentalActual = EstadoMentalDelAgente::PatrullandoSector;`

#### 4. `ejecutarMovimientoOAccion`
- Si `estadoMentalActual == EstadoMentalDelAgente::PausaDeVigilancia`:
  - `double ang = CalculosEspaciales::calcularAnguloEntrePuntosEnRadianes(cuerpo.obtenerPosicionX(), cuerpo.obtenerPosicionY(), cuerpo.obtenerDestinoActual().columna + 0.5, cuerpo.obtenerDestinoActual().fila + 0.5);`
  - Gira `direccionMirada` hacia la posicion de Scooby.
- Si `puedeCaminarPorRutaCorta && estadoMentalActual == EstadoMentalDelAgente::PatrullandoSector`:
  - Si tiene puntos en `memoriaPropia.listaDePuntosDeRutaFija`: camina hacia el punto objetivo actual con `cuerpo.moverHacia`. Al llegar a menos de 0.2 casillas, avanza al siguiente punto de forma circular.

#### 5. `actualizarMemoriaYAprendizaje`
- Si `memoriaPropia.segundosDeEnfriamientoDelServicio > 0.0`:
  - `memoriaPropia.segundosDeEnfriamientoDelServicio -= 0.15;`
