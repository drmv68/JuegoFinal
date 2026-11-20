# ESPECIFICACION TECNICA: mision.h / mision.cpp

## Archivo de Cabecera: `mision.h`

### Guardas de inclusion
```cpp
#ifndef MISION_H
#define MISION_H
```

### Inclusiones requeridas
```cpp
#include <QString>
#include <vector>
#include "mapa/mapa.h"
#include "personajes/objeto_del_escenario.h"
#include "herramientas/azar_con_semilla.h"
#include "juego/teclado.h"
```

### Declaraciones Adelantadas
```cpp
class Scooby;
class PersonajeNoJugable;
```

### Enumeraciones y Estructuras
```cpp
enum class TipoDeEfectoDeMision {
    AbrirPuerta,
    CerrarPuerta,
    CrearObjetoEnElMapa,
    MostrarMarcaEnSuelo,
    CambiarDialogoDePersonaje,
    EntregarProvisiones,
    EntregarGemas,
    AlertarZona,
    TacharSospechoso,
    EscribirEnLibreta
};

struct EfectoDeMisionEnElMundo {
    TipoDeEfectoDeMision tipo;
    CoordenadaCasilla casilla;
    CategoriaDeObjeto categoriaObjeto;
    int cantidadNumerica;
    QString textoInformativo;
    QString clavePersonaje;
};

struct SituacionActualDelMundo {
    const Scooby& scooby;
    const Mapa& mapa;
    const EstadoDeTeclado& teclado;
    const std::vector<PersonajeNoJugable*>& personajes;
    const std::vector<ObjetoDelEscenario*>& objetos;
    bool esDeNoche;
    int numeroDeSerieDelPersonajeInteractuado;
};

enum class EstadoDeLaMision {
    EnEsperaPorPasoAnterior,
    ActivaEnCurso,
    CompletadaConExito
};
```

### Declaracion de Clase Madre Abstracta `MisionBase`
```cpp
class MisionBase {
protected:
    int codigoDeLaMision;
    int pasoDeUbicacionCorrespondiente;
    QString tituloParaLaLibreta;
    QString textoDeLaPistaParaLaLibreta;
    EstadoDeLaMision estadoActual;
    std::vector<EfectoDeMisionEnElMundo> listaDeEfectosAlIniciar;
    std::vector<EfectoDeMisionEnElMundo> listaDeEfectosAlCompletar;

public:
    MisionBase(int codigo, int paso, const QString& titulo);
    virtual ~MisionBase() = default;

    virtual void prepararMision(AzarConSemilla& azar, const Mapa& mapa) = 0;
    virtual bool evaluarCumplimientoEnCuadro(const SituacionActualDelMundo& situacion) = 0;

    void finalizarYMarcarComoCompletada();

    int obtenerCodigoDeLaMision() const;
    int obtenerPasoDeUbicacionCorrespondiente() const;
    QString obtenerTituloParaLaLibreta() const;
    QString obtenerTextoDeLaPistaParaLaLibreta() const;
    void establecerTextoDeLaPistaParaLaLibreta(const QString& texto);

    EstadoDeLaMision obtenerEstadoActual() const;
    void establecerEstadoActual(EstadoDeLaMision nuevoEstado);

    const std::vector<EfectoDeMisionEnElMundo>& obtenerListaDeEfectosAlIniciar() const;
    const std::vector<EfectoDeMisionEnElMundo>& obtenerListaDeEfectosAlCompletar() const;

    void agregarEfectoAlIniciar(const EfectoDeMisionEnElMundo& efecto);
    void agregarEfectoAlCompletar(const EfectoDeMisionEnElMundo& efecto);
};

#endif
```

---

## Archivo de Implementacion: `mision.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/misiones/mision.h"
```

### Implementacion de Metodos

#### 1. Constructor
- `codigoDeLaMision = codigo;`
- `pasoDeUbicacionCorrespondiente = paso;`
- `tituloParaLaLibreta = titulo;`
- `estadoActual = EstadoDeLaMision::EnEsperaPorPasoAnterior;`

#### 2. `finalizarYMarcarComoCompletada`
- `estadoActual = EstadoDeLaMision::CompletadaConExito;`
