# ESPECIFICACION TECNICA: teclado.h

## Archivo de Cabecera: `teclado.h` (Solo .h)

### Guardas de inclusion
```cpp
#ifndef TECLADO_H
#define TECLADO_H
```

### Estructura `EstadoDeTeclado`
```cpp
struct EstadoDeTeclado {
    bool arriba;
    bool abajo;
    bool izquierda;
    bool derecha;
    bool correrActivo;
    bool interactuarPresionado;
    bool interactuarRecienPulsado;

    EstadoDeTeclado()
        : arriba(false)
        , abajo(false)
        , izquierda(false)
        , derecha(false)
        , correrActivo(false)
        , interactuarPresionado(false)
        , interactuarRecienPulsado(false) {}

    inline void limpiarPulsacionReciente() {
        interactuarRecienPulsado = false;
    }

    inline bool hayAlgunaDireccionPresionada() const {
        return (arriba || abajo || izquierda || derecha);
    }
};

#endif
```

### Reglas de Diseño
1. Solo cabecera `.h`.
2. La tecla `Tab` no viaja a la logica: la interfaz la captura de manera local para abrir y cerrar la libreta.
3. `interactuarRecienPulsado` es un pulso unico de un cuadro para no disparar interacciones repetidas mientras se sostiene la tecla.
