# ESPECIFICACION TECNICA: movimiento.h

## Archivo de Cabecera: `movimiento.h` (Solo .h)

### Guardas de inclusion
```cpp
#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H
```

### Estructura de Coordenada Tridimensional
```cpp
struct CoordenadaFisicaTresEjes {
    double posicionX;
    double posicionY;
    double alturaZ;
};
```

### Declaracion de Clase Madre Abstracta `MovimientoBase`
```cpp
class MovimientoBase {
protected:
    double tiempoAcumuladoEnSegundos;
    bool estaSimulacionActiva;

public:
    MovimientoBase()
        : tiempoAcumuladoEnSegundos(0.0), estaSimulacionActiva(true) {}

    virtual ~MovimientoBase() = default;

    virtual void reiniciarSimulacion() = 0;
    virtual CoordenadaFisicaTresEjes avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) = 0;

    virtual bool estaEnMomentoClave() const {
        return false;
    }

    double obtenerTiempoAcumuladoEnSegundos() const {
        return tiempoAcumuladoEnSegundos;
    }

    bool obtenerEstaSimulacionActiva() const {
        return estaSimulacionActiva;
    }

    void establecerSimulacionActiva(bool estadoActivo) {
        estaSimulacionActiva = estadoActivo;
    }
};

#endif
```

### Reglas de Diseño
1. Clase puramente polimorfica sin archivo `.cpp`.
2. Destructor virtual obligatorio.
3. El tiempo no se acumula por saltos discretos externos; `avanzarYCalcularPosicion` suma el delta de tiempo al acumulador interno y evalua la ecuacion continua exacta.
