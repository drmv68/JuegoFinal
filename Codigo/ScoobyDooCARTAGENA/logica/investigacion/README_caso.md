# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: CASO DE INVESTIGACIÓN

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `Caso` encapsula la narrativa, el progreso investigativo, las pistas recolectadas, los sospechosos y la evaluación de acusaciones de un misterio en Cartagena. Contiene una lista secuencial de misiones polimórficas (`std::vector<std::unique_ptr<MisionBase>>`) y gestiona directamente la acusación formal sin necesidad de una clase `Acusacion` separada, protegiendo los invariantes del caso.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "investigacion/misiones/mision.h"
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cstdint>

class MundoDeCartagena;
```

## 3. ENUMERACIONES Y ESTRUCTURAS DE RETORNO
```cpp
enum class ResultadoDeAcusacion : uint8_t {
    CulpableIdentificadoCorrectamente = 0,
    InocenteAcusadoPenalizacion = 1,
    EvidenciaInsuficienteParaAcusar = 2,
    SospechosoYaDescartado = 3
};

struct Sospechoso {
    int identificador;
    std::string nombreCompleto;
    std::string coartadaInicial;
    bool esElVerdaderoCulpable;
    bool haSidoTachado;
};
```

## 4. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class Caso {
public:
    static constexpr size_t CANTIDAD_MAXIMA_SOSPECHOSOS = 5;

    Caso(
        int numeroDeCaso,
        std::string tituloDelMisterio,
        std::string descripcionNarrativa,
        int cantidadPistasRequeridasParaAcusar
    );

    ~Caso() = default;

    Caso(const Caso&) = delete;
    Caso& operator=(const Caso&) = delete;
    Caso(Caso&&) noexcept = default;
    Caso& operator=(Caso&&) noexcept = default;

    void agregarMision(std::unique_ptr<MisionBase> nuevaMision);
    void agregarSospechoso(int identificador, std::string nombre, std::string coartada, bool esCulpable);

    void iniciar(MundoDeCartagena& mundo);
    void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos);

    ResultadoDeAcusacion evaluarAcusacionContraSospechoso(int identificadorSospechoso);

    void registrarPistaObtenida(const std::string& descripcionPista);
    void descartarSospechosoPorInocencia(int identificadorSospechoso);

    int obtenerNumeroDeCaso() const;
    const std::string& obtenerTituloDelMisterio() const;
    const std::string& obtenerDescripcionNarrativa() const;
    int obtenerCantidadPistasRecolectadas() const;
    int obtenerCantidadPistasRequeridas() const;
    bool tieneEvidenciaSuficienteParaAcusar() const;
    bool estaResueltoExitosamente() const;

    const std::vector<std::string>& obtenerPistasRecolectadas() const;
    const std::vector<Sospechoso>& obtenerSospechosos() const;
    const MisionBase* obtenerMisionActual() const;

private:
    void avanzarASiguienteMision(MundoDeCartagena& mundo);

    int numeroDeCaso;
    std::string tituloDelMisterio;
    std::string descripcionNarrativa;
    int cantidadPistasRequeridasParaAcusar;

    std::vector<std::unique_ptr<MisionBase>> misionesDelCaso;
    size_t indiceDeMisionActiva;

    std::vector<Sospechoso> listaDeSospechosos;
    std::vector<std::string> listaDePistasRecolectadas;

    bool casoResuelto;
};
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `misionesDelCaso` se ejecuta en estricto orden secuencial: una misión no se inicia hasta que la anterior haya alcanzado `EstadoDeMision::CompletadaConExito`.
2. Para poder emitir una acusación mediante `evaluarAcusacionContraSospechoso`, `listaDePistasRecolectadas.size()` debe ser mayor o igual a `cantidadPistasRequeridasParaAcusar`. De lo contrario, se retorna `ResultadoDeAcusacion::EvidenciaInsuficienteParaAcusar`.
3. No se permite acusar a un sospechoso que ya haya sido descartado (`haSidoTachado == true`).
4. Si el sospechoso acusado tiene `esElVerdaderoCulpable == true`, `casoResuelto` pasa a verdadero y se retorna `ResultadoDeAcusacion::CulpableIdentificadoCorrectamente`.
5. Si no es culpable, se marca `haSidoTachado = true` y se retorna `ResultadoDeAcusacion::InocenteAcusadoPenalizacion`.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `iniciar`
1. Restablece `indiceDeMisionActiva = 0`.
2. Restablece `casoResuelto = false`.
3. Si `!misionesDelCaso.empty()`, invoca `misionesDelCaso[0]->iniciar(mundo)`.

### `actualizar`
1. Si `indiceDeMisionActiva >= misionesDelCaso.size()`, retorna.
2. Invoca `misionesDelCaso[indiceDeMisionActiva]->actualizar(mundo, segundosTranscurridos)`.
3. Si `misionesDelCaso[indiceDeMisionActiva]->estaCompletada()`:
   - Invoca `avanzarASiguienteMision(mundo)`.

### `avanzarASiguienteMision`
1. Incrementa `indiceDeMisionActiva++`.
2. Si `indiceDeMisionActiva < misionesDelCaso.size()`:
   - Invoca `misionesDelCaso[indiceDeMisionActiva]->iniciar(mundo)`.
