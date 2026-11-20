# ESPECIFICACION TECNICA: recursos_de_la_partida.h / recursos_de_la_partida.cpp

## Archivo de Cabecera: `recursos_de_la_partida.h`

### Guardas de inclusion
```cpp
#ifndef RECURSOS_DE_LA_PARTIDA_H
#define RECURSOS_DE_LA_PARTIDA_H
```

### Constantes constexpr
```cpp
namespace ConstantesRecursos {
    constexpr double PROVISIONES_MAXIMAS_INICIALES = 100.0;
    constexpr double PORCENTAJE_RESERVA_CRITICA = 0.20; // 20%
    constexpr double PROVISIONES_POR_CARGA_ALMACEN = 20.0;
    constexpr double PROVISIONES_POR_CAMBIO_FRED = 25.0;
}
```

### Declaracion de Clase `RecursosDeLaPartida`
```cpp
class RecursosDeLaPartida {
private:
    double provisionesActuales;
    double provisionesMaximas;
    double provisionesPerdidasPorSegundo;
    double multiplicadorDeRoboNocturno;
    int gemasDisponibles;
    int oportunidadesDeAcusacionRestantes;

public:
    RecursosDeLaPartida();

    void inicializarRecursos(double provisionesIniciales, double drenajePorSegundo, double roboNocturno, int oportunidadesAcusacion);
    void actualizarConsumoDeProvisiones(double segundosTranscurridos, bool villanoRobandoDeNoche);

    void sumarProvisiones(double cantidad);
    void agregarGemas(int cantidad);
    bool consumirGemas(int cantidad);
    void descontarOportunidadDeAcusacion();

    double obtenerProvisionesActuales() const;
    double obtenerProvisionesMaximas() const;
    int obtenerGemasDisponibles() const;
    int obtenerOportunidadesDeAcusacionRestantes() const;

    bool estanProvisionesAgotadas() const;
    bool estaEnReservaCritica() const;
};

#endif
```

---

## Archivo de Implementacion: `recursos_de_la_partida.cpp`

### Inclusiones requeridas
```cpp
#include "juego/recursos_de_la_partida.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa variables en valores por defecto seguros.

#### 2. `inicializarRecursos`
- `provisionesMaximas = ConstantesRecursos::PROVISIONES_MAXIMAS_INICIALES;`
- `provisionesActuales = provisionesIniciales;`
- `provisionesPerdidasPorSegundo = drenajePorSegundo;`
- `multiplicadorDeRoboNocturno = roboNocturno;`
- `gemasDisponibles = 0;`
- `oportunidadesDeAcusacionRestantes = oportunidadesAcusacion;`

#### 3. `actualizarConsumoDeProvisiones`
- `double tasaDrenaje = provisionesPerdidasPorSegundo;`
- Si `villanoRobandoDeNoche`: `tasaDrenaje *= multiplicadorDeRoboNocturno;`
- `provisionesActuales -= tasaDrenaje * segundosTranscurridos;`
- `provisionesActuales = CalculosEspaciales::mantenerValorDentroDelRango(provisionesActuales, 0.0, provisionesMaximas);`

#### 4. `sumarProvisiones`
- `provisionesActuales += cantidad;`
- `provisionesActuales = CalculosEspaciales::mantenerValorDentroDelRango(provisionesActuales, 0.0, provisionesMaximas);`

#### 5. `agregarGemas` y `consumirGemas`
- `agregarGemas`: `gemasDisponibles += cantidad;`
- `consumirGemas`: si `gemasDisponibles >= cantidad`, resta y retorna `true`; sino retorna `false`.

#### 6. `descontarOportunidadDeAcusacion`
- `if (oportunidadesDeAcusacionRestantes > 0) oportunidadesDeAcusacionRestantes--;`

#### 7. `estanProvisionesAgotadas` y `estaEnReservaCritica`
- `estanProvisionesAgotadas`: retorna `provisionesActuales <= 0.0;`
- `estaEnReservaCritica`: retorna `provisionesActuales <= (provisionesMaximas * ConstantesRecursos::PORCENTAJE_RESERVA_CRITICA);`
