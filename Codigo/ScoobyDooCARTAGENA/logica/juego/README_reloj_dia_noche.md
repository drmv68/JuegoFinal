# ESPECIFICACION TECNICA: reloj_dia_noche.h / reloj_dia_noche.cpp

## Archivo de Cabecera: `reloj_dia_noche.h`

### Guardas de inclusion
```cpp
#ifndef RELOJ_DIA_NOCHE_H
#define RELOJ_DIA_NOCHE_H
```

### Enumeracion de Momento
```cpp
enum class MomentoDelDia {
    Dia,
    Noche
};
```

### Constantes constexpr
```cpp
namespace ConstantesReloj {
    constexpr double DURACION_TRANSICION_LUZ_SEGUNDOS = 5.0;
}
```

### Declaracion de Clase `RelojDiaNoche`
```cpp
class RelojDiaNoche {
private:
    double segundosRestantesTotales;
    double duracionDelCicloDeDia;
    double duracionDelCicloDeNoche;
    double tiempoEnElCicloActual;
    MomentoDelDia momentoActual;
    double factorDeOscuridad;
    bool cicloDiaNocheHabilitado;

public:
    RelojDiaNoche();

    void reiniciar(double totalSegundos, double segundosDeDia, double segundosDeNoche, bool habilitarCiclo);
    bool avanzarTiempo(double segundosTranscurridos);

    void aplicarBonoDeTiempo(double segundosExtra);
    void aplicarDescuentoDeTiempo(double segundosMenos);

    bool esDeNoche() const;
    double obtenerFactorDeOscuridad() const;
    double obtenerSegundosRestantes() const;
    MomentoDelDia obtenerMomentoActual() const;
    bool seAcaboElTiempo() const;
};

#endif
```

---

## Archivo de Implementacion: `reloj_dia_noche.cpp`

### Inclusiones requeridas
```cpp
#include "juego/reloj_dia_noche.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa variables en 0.0, `momentoActual = MomentoDelDia::Dia`, `factorDeOscuridad = 0.0`, `cicloDiaNocheHabilitado = false`.

#### 2. `reiniciar`
- `segundosRestantesTotales = totalSegundos;`
- `duracionDelCicloDeDia = segundosDeDia;`
- `duracionDelCicloDeNoche = segundosDeNoche;`
- `tiempoEnElCicloActual = 0.0;`
- `momentoActual = MomentoDelDia::Dia;`
- `factorDeOscuridad = 0.0;`
- `cicloDiaNocheHabilitado = habilitarCiclo;`

#### 3. `avanzarTiempo`
- `segundosRestantesTotales -= segundosTranscurridos;`
- `if (segundosRestantesTotales < 0.0) segundosRestantesTotales = 0.0;`
- Si `!cicloDiaNocheHabilitado`:
  - `factorDeOscuridad = 0.0; momentoActual = MomentoDelDia::Dia;`
  - Retorna `false;`
- `tiempoEnElCicloActual += segundosTranscurridos;`
- `bool huboCambioDeMomento = false;`
- Evaluacion de cambio de ciclo:
  - Si `momentoActual == MomentoDelDia::Dia`:
    - Si `tiempoEnElCicloActual >= duracionDelCicloDeDia`:
      - `momentoActual = MomentoDelDia::Noche;`
      - `tiempoEnElCicloActual = 0.0;`
      - `huboCambioDeMomento = true;`
  - Si `momentoActual == MomentoDelDia::Noche`:
    - Si `tiempoEnElCicloActual >= duracionDelCicloDeNoche`:
      - `momentoActual = MomentoDelDia::Dia;`
      - `tiempoEnElCicloActual = 0.0;`
      - `huboCambioDeMomento = true;`
- Interpolacion lineal de `factorDeOscuridad` en los ultimos 5.0 s de transicion:
  - En Dia hacia Noche (ultimos 5 s de dia):
    - `tiempoParaNoche = duracionDelCicloDeDia - tiempoEnElCicloActual;`
    - Si `tiempoParaNoche <= ConstantesReloj::DURACION_TRANSICION_LUZ_SEGUNDOS`:
      - `factorDeOscuridad = 1.0 - (tiempoParaNoche / ConstantesReloj::DURACION_TRANSICION_LUZ_SEGUNDOS);`
    - Sino `factorDeOscuridad = 0.0;`
  - En Noche hacia Dia (ultimos 5 s de noche):
    - `tiempoParaDia = duracionDelCicloDeNoche - tiempoEnElCicloActual;`
    - Si `tiempoParaDia <= ConstantesReloj::DURACION_TRANSICION_LUZ_SEGUNDOS`:
      - `factorDeOscuridad = tiempoParaDia / ConstantesReloj::DURACION_TRANSICION_LUZ_SEGUNDOS;`
    - Sino `factorDeOscuridad = 1.0;`
- `factorDeOscuridad = CalculosEspaciales::mantenerValorDentroDelRango(factorDeOscuridad, 0.0, 1.0);`
- Retorna `huboCambioDeMomento;`

#### 4. `aplicarBonoDeTiempo` y `aplicarDescuentoDeTiempo`
- `aplicarBonoDeTiempo`: `segundosRestantesTotales += segundosExtra;`
- `aplicarDescuentoDeTiempo`: `segundosRestantesTotales -= segundosMenos; if (segundosRestantesTotales < 0.0) segundosRestantesTotales = 0.0;`

#### 5. `esDeNoche`, `seAcaboElTiempo`
- `esDeNoche`: retorna `momentoActual == MomentoDelDia::Noche;`
- `seAcaboElTiempo`: retorna `segundosRestantesTotales <= 0.0;`
