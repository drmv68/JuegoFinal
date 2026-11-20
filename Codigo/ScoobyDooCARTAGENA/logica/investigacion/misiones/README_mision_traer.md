# ESPECIFICACION TECNICA: mision_traer.h / mision_traer.cpp

## Archivo de Cabecera: `mision_traer.h`

### Guardas de inclusion
```cpp
#ifndef MISION_TRAER_H
#define MISION_TRAER_H
```

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision.h"
```

### Declaracion de Clase `MisionTraer`
```cpp
class MisionTraer : public MisionBase {
private:
    CategoriaDeObjeto categoriaDelObjetoRequerido;
    int cantidadRequerida;
    int cantidadRecolectadaHastaAhora;
    CoordenadaCasilla casillaDeEntrega;
    bool generaAlertaAlRecoger;

public:
    MisionTraer(int codigo, int paso, const QString& titulo, CategoriaDeObjeto categoria, int cantidad, CoordenadaCasilla entrega, bool alertaAlRecoger);

    void prepararMision(AzarConSemilla& azar, const Mapa& mapa) override;
    bool evaluarCumplimientoEnCuadro(const SituacionActualDelMundo& situacion) override;

    void registrarObjetoRecolectado();
    int obtenerCantidadRecolectadaHastaAhora() const;
    int obtenerCantidadRequerida() const;
    CoordenadaCasilla obtenerCasillaDeEntrega() const;
};

#endif
```

---

## Archivo de Implementacion: `mision_traer.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision_traer.h"
#include "personajes/scooby.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `MisionBase(codigo, paso, titulo)`.
- `categoriaDelObjetoRequerido = categoria;`
- `cantidadRequerida = cantidad;`
- `cantidadRecolectadaHastaAhora = 0;`
- `casillaDeEntrega = entrega;`
- `generaAlertaAlRecoger = alertaAlRecoger;`

#### 2. `prepararMision`
- Baraja lista de arbustos o casillas transitables del mapa usando `azar.barajarListaDeCasillas`.
- Para los primeros `cantidadRequerida` puntos:
  - Crea un `EfectoDeMisionEnElMundo`:
    - `tipo = TipoDeEfectoDeMision::CrearObjetoEnElMapa;`
    - `casilla = casillaSorteada;`
    - `categoriaObjeto = categoriaDelObjetoRequerido;`
    - Agrega a `listaDeEfectosAlIniciar`.

#### 3. `registrarObjetoRecolectado`
- `cantidadRecolectadaHastaAhora++;`

#### 4. `evaluarCumplimientoEnCuadro`
- Si `cantidadRecolectadaHastaAhora < cantidadRequerida` retorna `false`.
- Valida si Scooby esta en la `casillaDeEntrega` y pulso interactuar:
  - `double distEjeX = CalculosEspaciales::calcularDistanciaEnUnEje(situacion.scooby.obtenerPosicionX(), casillaDeEntrega.columna + 0.5);`
  - `double distEjeY = CalculosEspaciales::calcularDistanciaEnUnEje(situacion.scooby.obtenerPosicionY(), casillaDeEntrega.fila + 0.5);`
  - Si `distEjeX <= 1.2 && distEjeY <= 1.2 && situacion.teclado.interactuarRecienPulsado`:
    - `finalizarYMarcarComoCompletada();`
    - Retorna `true;`
- Retorna `false;`
