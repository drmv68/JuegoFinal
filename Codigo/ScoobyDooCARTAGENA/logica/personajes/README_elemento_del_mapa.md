# ESPECIFICACION TECNICA: elemento_del_mapa.h / elemento_del_mapa.cpp

## Archivo y Ubicacion
`logica/personajes/elemento_del_mapa.h`
`logica/personajes/elemento_del_mapa.cpp`

## Proposito y Responsabilidad Unica
Clase madre abstracta de cualquier entidad con presencia espacial en el mapa 2D. Asigna identificador unico de serie, gestiona coordenadas continuas y calcula colisiones AABB y orden de dibujo isometrico.

## Dependencias
### Para elemento_del_mapa.h
- `#include "herramientas/tipos_espaciales.h"`

### Para elemento_del_mapa.cpp
- `#include "personajes/elemento_del_mapa.h"`
- `#include "herramientas/calculos_espaciales.h"`

## Definicion Exacta de Cabecera (elemento_del_mapa.h)

```cpp
#ifndef ELEMENTO_DEL_MAPA_H
#define ELEMENTO_DEL_MAPA_H

#include "herramientas/tipos_espaciales.h"

class ElementoDelMapaBase {
public:
    ElementoDelMapaBase(double posicionInicialX, double posicionInicialY, double anchoColision, double altoColision);
    virtual ~ElementoDelMapaBase() = default;

    ElementoDelMapaBase(const ElementoDelMapaBase&) = delete;
    ElementoDelMapaBase& operator=(const ElementoDelMapaBase&) = delete;

    virtual void avanzarUnCuadro(double segundosTranscurridos) = 0;

    int obtenerNumeroDeSerieUnico() const;
    double obtenerPosicionX() const;
    double obtenerPosicionY() const;
    double obtenerAnchoDeColision() const;
    double obtenerAltoDeColision() const;
    DireccionMirada obtenerDireccionMirada() const;
    bool obtenerEstaActivo() const;
    bool obtenerEsVisible() const;

    void establecerPosicion(double nuevaPosicionX, double nuevaPosicionY);
    void establecerDireccionMirada(DireccionMirada nuevaDireccion);
    void establecerActivo(bool estadoActivo);
    void establecerVisible(bool estadoVisible);

    CoordenadaCasilla obtenerCasillaActual() const;
    bool colisionaCon(const ElementoDelMapaBase& otroElemento) const;
    double calcularPrioridadDeDibujoIsometrico() const;

protected:
    int numeroDeSerieUnico;
    double posicionX;
    double posicionY;
    double anchoDeColision;
    double altoDeColision;
    DireccionMirada direccionMirada;
    bool estaActivo;
    bool esVisible;

private:
    static int generadorSecuencialDeNumeroDeSerie;
};

#endif
```

## Algoritmo Paso a Paso (elemento_del_mapa.cpp)

### Inicializacion de Variable Estatica
```cpp
int ElementoDelMapaBase::generadorSecuencialDeNumeroDeSerie = 0;
```

### Constructor
1. Incrementar y asignar `numeroDeSerieUnico = ++generadorSecuencialDeNumeroDeSerie;`.
2. Asignar `posicionX = posicionInicialX; posicionY = posicionInicialY;`.
3. Asignar `anchoDeColision = anchoColision; altoDeColision = altoColision;`.
4. Asignar `direccionMirada = DireccionMirada::Sur;`.
5. Asignar `estaActivo = true; esVisible = true;`.

### obtenerCasillaActual() const
1. Retornar `CalculosEspaciales::convertirPosicionACasilla(posicionX, posicionY)`.

### colisionaCon(const ElementoDelMapaBase& otroElemento) const
1. Si `!estaActivo || !otroElemento.estaActivo`, retornar `false`.
2. Calcular semianchos y semialtos:
   - `mitadAnchoPropio = anchoDeColision * 0.5;`
   - `mitadAltoPropio = altoDeColision * 0.5;`
   - `mitadAnchoOtro = otroElemento.anchoDeColision * 0.5;`
   - `mitadAltoOtro = otroElemento.altoDeColision * 0.5;`
3. Evaluar solapamiento en el eje horizontal:
   - `solapaHorizontal = (posicionX + mitadAnchoPropio >= otroElemento.posicionX - mitadAnchoOtro) && (posicionX - mitadAnchoPropio <= otroElemento.posicionX + mitadAnchoOtro);`
4. Evaluar solapamiento en el eje vertical:
   - `solapaVertical = (posicionY + mitadAltoPropio >= otroElemento.posicionY - mitadAltoOtro) && (posicionY - mitadAltoPropio <= otroElemento.posicionY + mitadAltoOtro);`
5. Retornar `solapaHorizontal && solapaVertical`.

### calcularPrioridadDeDibujoIsometrico() const
1. En la proyeccion isometrica estandar, la profundidad cartesiana de oclusion equivale a la suma de coordenadas:
   - Retornar `posicionX + posicionY`.
