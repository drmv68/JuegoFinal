# ESPECIFICACION TECNICA: mision_hablar.h / mision_hablar.cpp

## Archivo de Cabecera: `mision_hablar.h`

### Guardas de inclusion
```cpp
#ifndef MISION_HABLAR_H
#define MISION_HABLAR_H
```

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision.h"
#include "personajes/personaje_no_jugable.h"
```

### Declaracion de Clase `MisionHablar`
```cpp
class MisionHablar : public MisionBase {
private:
    TipoRolDelPersonaje rolDelPersonajeObjetivo;
    bool requiereAcertijoConOpciones;
    QString preguntaDelAcertijo;
    QString opcionesDeRespuesta[3];
    int indiceDeLaRespuestaCorrecta;
    int indiceDeLaOpcionDescartadaPorGema;
    std::vector<QString> clavesDePersonajesQueDebenHablarAntes;
    bool respondioCorrectamente;

public:
    MisionHablar(int codigo, int paso, const QString& titulo, TipoRolDelPersonaje rolObjetivo);

    void configurarAcertijo(const QString& pregunta, const QString& op0, const QString& op1, const QString& op2, int indiceCorrecto);
    void descartarOpcionPorGema();
    bool verificarRespuesta(int indiceElegido);

    void prepararMision(AzarConSemilla& azar, const Mapa& mapa) override;
    bool evaluarCumplimientoEnCuadro(const SituacionActualDelMundo& situacion) override;

    TipoRolDelPersonaje obtenerRolDelPersonajeObjetivo() const;
    bool obtenerRequiereAcertijoConOpciones() const;
    QString obtenerPreguntaDelAcertijo() const;
    QString obtenerOpcionDeRespuesta(int indice) const;
    int obtenerIndiceDeLaOpcionDescartadaPorGema() const;
};

#endif
```

---

## Archivo de Implementacion: `mision_hablar.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision_hablar.h"
#include "personajes/scooby.h"
#include "personajes/personaje_no_jugable.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `MisionBase(codigo, paso, titulo)`.
- `rolDelPersonajeObjetivo = rolObjetivo;`
- `requiereAcertijoConOpciones = false;`
- `indiceDeLaRespuestaCorrecta = 0;`
- `indiceDeLaOpcionDescartadaPorGema = -1;`
- `respondioCorrectamente = false;`

#### 2. `configurarAcertijo`
- Asigna pregunta, las 3 opciones de respuesta y el indice correcto.
- `requiereAcertijoConOpciones = true;`

#### 3. `descartarOpcionPorGema`
- Si no se ha descartado aun, busca un indice `i != indiceDeLaRespuestaCorrecta` y asigna `indiceDeLaOpcionDescartadaPorGema = i;`

#### 4. `verificarRespuesta`
- Si `indiceElegido == indiceDeLaRespuestaCorrecta`:
  - `respondioCorrectamente = true;`
  - Retorna `true;`
- Retorna `false;`

#### 5. `prepararMision`
- No requiere generacion aleatoria espacial adicional; el personaje ya existe en el mapa.

#### 6. `evaluarCumplimientoEnCuadro`
- Si `situacion.numeroDeSerieDelPersonajeInteractuado == -1` retorna `false`.
- Busca en `situacion.personajes` el personaje con ese numero de serie.
- Si `personaje->obtenerRol() == rolDelPersonajeObjetivo`:
  - Si `requiereAcertijoConOpciones`: retorna `respondioCorrectamente;`
  - Si no requiere acertijo (solo hablar):
    - `finalizarYMarcarComoCompletada();`
    - Retorna `true;`
- Retorna `false;`
