# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: CONTROLADOR DEL JUEGO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `ControladorDelJuego` hereda de `QObject` y actúa como el orquestador central de la capa lógica. Su responsabilidad es coordinar el bucle temporal maestro mediante un único `QTimer` de 16 ms (~60 FPS), gestionar la máquina de estados de alto nivel (`EtapaDelJuego`), traducir la entrada del usuario (`GestorDeTeclado`), coordinar el `MundoDeCartagena`, avanzar el `Caso` de investigación activo, gestionar el `PerfilDeUsuario` y emitir señales Qt a la capa de presentación (GUI/Audio).

No realiza rendering ni manipula widgets; mantiene desacoplada la simulación y garantiza la estabilidad del sistema capturando excepciones de inicialización o carga de archivos.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "juego/etapa_del_juego.h"
#include "juego/teclado.h"
#include "juego/dificultad.h"
#include "juego/recursos_de_la_partida.h"
#include "juego/reloj_dia_noche.h"
#include "guardado/perfil.h"
#include "guardado/gestor_de_guardado.h"
#include "mapa/mundo_de_cartagena.h"
#include "investigacion/caso.h"
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class ControladorDelJuego : public QObject {
    Q_OBJECT

public:
    explicit ControladorDelJuego(QObject* padre = nullptr);
    ~ControladorDelJuego() override;

    ControladorDelJuego(const ControladorDelJuego&) = delete;
    ControladorDelJuego& operator=(const ControladorDelJuego&) = delete;

    void iniciarSistemaCompleto();
    void cargarONuevoPerfil(const QString& nombreDetective);

    void iniciarNuevaPartida(int nivelDeMapa, NivelDeDificultad dificultad);
    void pausarJuego();
    void reanudarJuego();
    void reiniciarNivelActual();

    void procesarPresionDeTecla(int codigoDeTeclaQt);
    void procesarLiberacionDeTecla(int codigoDeTeclaQt);

    ResultadoDeAcusacion emitirAcusacionContraSospechoso(int identificadorSospechoso);

    EtapaDelJuego obtenerEtapaActual() const;
    const MundoDeCartagena* obtenerMundo() const;
    const Caso* obtenerCasoActivo() const;
    const PerfilDeUsuario& obtenerPerfil() const;
    const RecursosDeLaPartida& obtenerRecursos() const;
    const RelojDiaNoche& obtenerReloj() const;

signals:
    void alActualizarFotogramaLogico();
    void alCambiarEtapaDelJuego(EtapaDelJuego nuevaEtapa);
    void alModificarRecursos();
    void alCompletarMision(int identificadorMision, const QString& titulo);
    void alResolverCasoExitosamente(int numeroDeCaso);
    void alPerderPartidaPorCaptura();
    void alOcurrirErrorCritico(const QString& mensajeError);
    void alDispararEventoVisual(TipoDeEventoVisual tipoEvento, float posicionX, float posicionY);

private slots:
    void ejecutarPasoDeSimulacion();

private:
    void cambiarEtapa(EtapaDelJuego nuevaEtapa);
    void actualizarLogicaDeJuego(float segundosTranscurridos);
    void verificarCondicionesDeVictoriaODerrota();
    void guardarProgresoAutomatico();

    QTimer temporizadorMaestro;
    EtapaDelJuego etapaActual;
    NivelDeDificultad dificultadActual;
    int nivelDeMapaActual;

    GestorDeTeclado gestorDeTeclado;
    RecursosDeLaPartida recursos;
    RelojDiaNoche reloj;
    PerfilDeUsuario perfilActivo;
    GestorDeGuardado gestorDeGuardado;

    std::unique_ptr<MundoDeCartagena> mundo;
    std::unique_ptr<Caso> casoActivo;
};
```

## 4. CONSTANTES DE TIEMPO Y CONFIGURACIÓN
```cpp
namespace ConstantesControlador {
    inline constexpr int INTERVALO_TEMPORIZADOR_MS = 16;
    inline constexpr float DELTA_TIEMPO_SEGUNDOS = 0.016666667f;
}
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. **Un solo temporizador maestro:** La simulación se ejecuta estrictamente a razón de un tick de ~16 ms conectado a `ejecutarPasoDeSimulacion()`. Ningún otro componente de lógica crea temporizadores paralelos.
2. **Propiedad clara:** El `ControladorDelJuego` es el dueño absoluto de `MundoDeCartagena` y `Caso` mediante `std::unique_ptr`. Al cambiar de nivel o reiniciar, los objetos previos son destruidos ordenadamente antes de instanciar los nuevos.
3. **Manejo defensivo de excepciones:** Todo proceso de lectura de archivos (`CargadorDeNiveles::cargarNivel`, `GestorDeGuardado::cargarPerfil`) se ejecuta dentro de bloques `try / catch`. Si ocurre un fallo, no se produce aborto del proceso; se emite `alOcurrirErrorCritico` y se transiciona a un estado seguro (`EtapaDelJuego::MenuPrincipal`).
4. **Desacoplamiento GUI:** No existen includes de `QWidget`, `QPainter` ni llamadas gráficas en este archivo. Toda comunicación hacia la vista se realiza mediante las señales de Qt declaradas.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `ControladorDelJuego::ControladorDelJuego`
1. Conecta `temporizadorMaestro.timeout` con el slot `ejecutarPasoDeSimulacion`.
2. Establece `etapaActual = EtapaDelJuego::Cargando`.
3. Configura `temporizadorMaestro.setInterval(ConstantesControlador::INTERVALO_TEMPORIZADOR_MS)`.

### `iniciarNuevaPartida`
1. Detiene `temporizadorMaestro`.
2. Asigna `nivelDeMapaActual = nivelDeMapa` y `dificultadActual = dificultad`.
3. Configura `recursos.reiniciarConVidas(ConfiguracionDeDificultad::obtenerVidasIniciales(dificultad))`.
4. Obtiene parámetros de dificultad y configura las duraciones de `reloj`.
5. En un bloque `try / catch`:
   - Crea `mundo = std::make_unique<MundoDeCartagena>()`.
   - Invoca `mundo->cargarNivel(nivelDeMapaActual, dificultadActual)`.
   - Instancia y configura `casoActivo` para el nivel correspondiente.
   - Invoca `casoActivo->iniciar(*mundo)`.
   - Cambia la etapa a `EtapaDelJuego::JugandoPartidaNormal`.
   - Inicia `temporizadorMaestro.start()`.
6. En el bloque `catch`:
   - Emite `alOcurrirErrorCritico("Error al cargar los recursos del mapa o nivel.")`.
   - Cambia la etapa a `EtapaDelJuego::MenuPrincipal`.

### `ejecutarPasoDeSimulacion`
1. Si `etapaActual != EtapaDelJuego::JugandoPartidaNormal` y `etapaActual != EtapaDelJuego::ConfrontacionFinalConVillano`:
   - Retorna (el juego se encuentra pausado o en menú).
2. Si `gestorDeTeclado.obtenerEstadoActual().pausarPulsado`:
   - Invoca `pausarJuego()`.
   - `gestorDeTeclado.limpiarPulsacionesInstantaneas()`.
   - Retorna.
3. Invoca `actualizarLogicaDeJuego(ConstantesControlador::DELTA_TIEMPO_SEGUNDOS)`.
4. `gestorDeTeclado.limpiarPulsacionesInstantaneas()`.
5. Emite `alActualizarFotogramaLogico()`.

### `actualizarLogicaDeJuego`
1. `reloj.actualizar(segundosTranscurridos)`.
2. `mundo->actualizar(segundosTranscurridos, gestorDeTeclado.obtenerEstadoActual())`.
3. `casoActivo->actualizar(*mundo, segundosTranscurridos)`.
4. `perfilActivo.acumularTiempoJugado(segundosTranscurridos)`.
5. Verifica colisiones de Scooby con objetos y enemigos mediante `mundo`.
6. Verifica condiciones de victoria o derrota.

### `emitirAcusacionContraSospechoso`
1. Evalúa la acusación delegando en `casoActivo->evaluarAcusacionContraSospechoso(identificadorSospechoso)`.
2. Si el resultado es `CulpableIdentificadoCorrectamente`:
   - Suma puntaje mayor a `recursos` y `perfilActivo`.
   - Registra caso resuelto en `perfilActivo`.
   - Guarda automáticamente la partida con `gestorDeGuardado.guardarPerfil(perfilActivo)`.
   - Cambia etapa a `EtapaDelJuego::CasoResueltoVictoria`.
   - Emite `alResolverCasoExitosamente(casoActivo->obtenerNumeroDeCaso())`.
3. Si el resultado es `InocenteAcusadoPenalizacion`:
   - Aplica penalización de puntaje en `recursos` y resta una vida.
   - Emite `alModificarRecursos()`.
   - Si `recursos.estaSinVidas()`:
     - Cambia etapa a `EtapaDelJuego::PartidaPerdidaPorAtrapado`.
     - Emite `alPerderPartidaPorCaptura()`.
4. Retorna el resultado de la acusación.
