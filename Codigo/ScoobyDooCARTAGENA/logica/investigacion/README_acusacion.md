# ESPECIFICACION TECNICA: acusacion.h / acusacion.cpp

## Archivo de Cabecera: `acusacion.h`

### Guardas de inclusion
```cpp
#ifndef ACUSACION_H
#define ACUSACION_H
```

### Inclusiones requeridas
```cpp
#include "juego/controlador_del_juego.h"
#include "investigacion/caso.h"
#include "juego/recursos_de_la_partida.h"
#include "juego/reloj_dia_noche.h"
#include "inteligencia/avisos_entre_agentes.h"
```

### Declaraciones Adelantadas
```cpp
enum class EtapaDelJuego;
class Caso;
class RecursosDeLaPartida;
class RelojDiaNoche;
struct BuzonCompartidoDeAvisos;
```

### Enumeracion de Resultado
```cpp
enum class ResultadoDeAcusacion {
    AciertoVillanoExpuesto,
    ErrorInocenteTachado,
    ErrorSinOportunidadesDerrota
};
```

### Declaracion de Clase `Acusacion`
```cpp
class Acusacion {
private:
    int indiceDelSospechosoSeleccionado;

public:
    Acusacion();

    bool puedeFormularAcusacionAhora(EtapaDelJuego etapaDeLaPartida, const RecursosDeLaPartida& recursos) const;
    ResultadoDeAcusacion procesarAcusacionContraSospechoso(int indiceDelSospechoso, Caso& caso, RecursosDeLaPartida& recursos, RelojDiaNoche& reloj, BuzonCompartidoDeAvisos& buzonDeAvisos);

    int obtenerIndiceDelSospechosoSeleccionado() const;
};

#endif
```

---

## Archivo de Implementacion: `acusacion.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/acusacion.h"
```

### Implementacion de Metodos

#### 1. Constructor
- `indiceDelSospechosoSeleccionado = -1;`

#### 2. `puedeFormularAcusacionAhora`
- Retorna `(etapaDeLaPartida == EtapaDelJuego::ExploracionNivelDos && recursos.obtenerOportunidadesDeAcusacionRestantes() > 0);`

#### 3. `procesarAcusacionContraSospechoso`
- `indiceDelSospechosoSeleccionado = indiceDelSospechoso;`
- `const FichaDeSospechoso& sospechoso = caso.obtenerSospechosos()[indiceDelSospechoso];`
- `const FichaDeSospechoso& culpable = caso.obtenerCulpable();`
- Si `sospechoso.nombre == culpable.nombre`:
  - Acierto:
    - `reloj.aplicarBonoDeTiempo(30.0);`
    - `recursos.agregarGemas(1);`
    - Retorna `ResultadoDeAcusacion::AciertoVillanoExpuesto;`
- Si no coincide (Error):
  - `recursos.descontarOportunidadDeAcusacion();`
  - Penalizacion de tiempo: descuenta 20% del tiempo restante (minimo 20.0 s):
    - `double tiempoActual = reloj.obtenerSegundosRestantes();`
    - `double descuento = tiempoActual * 0.20;`
    - `if (descuento < 20.0) descuento = 20.0;`
    - `reloj.aplicarDescuentoDeTiempo(descuento);`
  - Activa alerta global de 15 segundos en el buzon de secuaces por el escandalo:
    - `buzonDeAvisos.activarAlertaGlobal(15.0);`
  - Si `recursos.obtenerOportunidadesDeAcusacionRestantes() <= 0`:
    - Retorna `ResultadoDeAcusacion::ErrorSinOportunidadesDerrota;`
  - Retorna `ResultadoDeAcusacion::ErrorInocenteTachado;`
