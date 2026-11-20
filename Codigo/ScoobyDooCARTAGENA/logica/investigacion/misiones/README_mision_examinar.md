# ESPECIFICACION TECNICA: mision_examinar.h / mision_examinar.cpp

## Archivo de Cabecera: `mision_examinar.h`

### Guardas de inclusion
```cpp
#ifndef MISION_EXAMINAR_H
#define MISION_EXAMINAR_H
```

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision.h"
```

### Declaracion de Clase `MisionExaminar`
```cpp
class MisionExaminar : public MisionBase {
private:
    CoordenadaCasilla casillaDeLaMarca;
    bool soloVisibleDeNocheConLinterna;
    bool usaSensorDeProximidadSonoro;
    double distanciaActualALaMarca;
    double radioDeInteraccionEnCasillas;

public:
    MisionExaminar(int codigo, int paso, const QString& titulo, bool soloDeNoche, bool usaSensor);

    void establecerCasillaDeLaMarca(CoordenadaCasilla casilla);
    CoordenadaCasilla obtenerCasillaDeLaMarca() const;

    void prepararMision(AzarConSemilla& azar, const Mapa& mapa) override;
    bool evaluarCumplimientoEnCuadro(const SituacionActualDelMundo& situacion) override;

    double obtenerDistanciaActualALaMarca() const;
    bool obtenerUsaSensorDeProximidadSonoro() const;
    bool obtenerSoloVisibleDeNocheConLinterna() const;
};

#endif
```

---

## Archivo de Implementacion: `mision_examinar.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision_examinar.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `MisionBase(codigo, paso, titulo)`.
- `soloVisibleDeNocheConLinterna = soloDeNoche;`
- `usaSensorDeProximidadSonoro = usaSensor;`
- `distanciaActualALaMarca = 999.0;`
- `radioDeInteraccionEnCasillas = 1.5;`

#### 2. `establecerCasillaDeLaMarca`
- `casillaDeLaMarca = casilla;`

#### 3. `prepararMision`
- Si la marca aun no se ha fijado, toma una casilla transitable del sector activo.
- Crea un `EfectoDeMisionEnElMundo`:
  - `tipo = TipoDeEfectoDeMision::MostrarMarcaEnSuelo;`
  - `casilla = casillaDeLaMarca;`
  - Agrega a `listaDeEfectosAlIniciar`.

#### 4. `evaluarCumplimientoEnCuadro`
- Calcula distancia euclidea:
  - `double distCuad = CalculosEspaciales::calcularDistanciaAlCuadrado(situacion.scooby.obtenerPosicionX(), situacion.scooby.obtenerPosicionY(), casillaDeLaMarca.columna + 0.5, casillaDeLaMarca.fila + 0.5);`
  - `distanciaActualALaMarca = std::sqrt(distCuad);`
- Si `soloVisibleDeNocheConLinterna && !situacion.esDeNoche` retorna `false`.
- Si `distanciaActualALaMarca <= radioDeInteraccionEnCasillas && situacion.teclado.interactuarRecienPulsado`:
  - `finalizarYMarcarComoCompletada();`
  - Retorna `true;`
- Retorna `false;`
