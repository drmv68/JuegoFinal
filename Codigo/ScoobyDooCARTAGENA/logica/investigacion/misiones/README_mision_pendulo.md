# ESPECIFICACION TECNICA: mision_pendulo.h / mision_pendulo.cpp

## Archivo de Cabecera: `mision_pendulo.h`

### Guardas de inclusion
```cpp
#ifndef MISION_PENDULO_H
#define MISION_PENDULO_H
```

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision.h"
#include "personajes/objeto_del_escenario.h"
```

### Declaracion de Clase `MisionPendulo`
```cpp
class MisionPendulo : public MisionBase {
private:
    const ObjetoDelEscenario* lamparaObservada;
    double radioDeInteraccionEnCasillas;

public:
    MisionPendulo(int codigo, int paso, const QString& titulo, const ObjetoDelEscenario* lampara);

    void prepararMision(AzarConSemilla& azar, const Mapa& mapa) override;
    bool evaluarCumplimientoEnCuadro(const SituacionActualDelMundo& situacion) override;

    void asignarLamparaObservada(const ObjetoDelEscenario* lampara);
};

#endif
```

---

## Archivo de Implementacion: `mision_pendulo.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision_pendulo.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `MisionBase(codigo, paso, titulo)`.
- `lamparaObservada = lampara;`
- `radioDeInteraccionEnCasillas = 2.0;`

#### 2. `asignarLamparaObservada`
- `lamparaObservada = lampara;`

#### 3. `prepararMision`
- No requiere generacion espacial; la lampara esta fija en la pared segun el mapa de detalles.

#### 4. `evaluarCumplimientoEnCuadro`
- Si `lamparaObservada == nullptr` retorna `false`.
- Valida pulsacion reciente:
  - Si `!situacion.teclado.interactuarRecienPulsado` retorna `false`.
- Valida cercania fisica de Scooby a la posicion de la lampara:
  - `double distCuad = CalculosEspaciales::calcularDistanciaAlCuadrado(situacion.scooby.obtenerPosicionX(), situacion.scooby.obtenerPosicionY(), lamparaObservada->obtenerPosicionX(), lamparaObservada->obtenerPosicionY());`
  - Si `distCuad > (radioDeInteraccionEnCasillas * radioDeInteraccionEnCasillas)` retorna `false`.
- Valida sincronia con la fisica del pendulo:
  - Si `lamparaObservada->estaLamparaEnAnguloMaximo()`:
    - `finalizarYMarcarComoCompletada();`
    - Retorna `true;`
- Retorna `false;`
