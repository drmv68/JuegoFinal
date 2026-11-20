# GUIA DE LA RAMA INTERFAZ

Guia para Juan Esteban Solarte. Todo el codigo de la carpeta `interfaz/`.
La interfaz **solo lee** el estado de la logica para pintarlo y le entrega
las teclas presionadas. Nunca modifica reglas ni calcula fisicas.

---

## Reglas generales

- Mismas convenciones de nombres que la logica: español completo,
  cero comentarios, cero numeros sueltos.
- `interfaz/` usa Qt Widgets y Qt Multimedia.
- La logica expone señales y consultas `const`; la interfaz las conecta.
- Cero `new` dentro del bucle de dibujo: reutilizar vectores y pixmaps.

---

## Orden de construccion (ramas)

1. `interfaz-base` → proyeccion_isometrica, animador_de_sprites,
   ventana_principal
2. `interfaz-escena` → vista_del_juego, capa_de_luz,
   temblor_y_transiciones
3. `interfaz-paneles` → panel_de_datos, libreta_del_investigador,
   cuadro_de_dialogo
4. `interfaz-menus` → menu_principal, selector_de_nivel_y_dificultad,
   galeria_de_villanos, pantalla_de_fin
5. `interfaz-sonido` → gestor_de_sonido

---

## Proyeccion isometrica

- Rombo estandar: 64 px ancho x 32 px alto (proporcion 2:1).
- `pixelX = (X - Y) * ancho / 2`
- `pixelY = (X + Y) * alto / 2 - alturaZ * alto`
- Metodos `const` e `inline`, cero trigonometria en tiempo real.

## Animador de sprites

- Precarga y recorta hojas una sola vez al arrancar.
- Claves de texto: `scooby_caminar_sur`, `secuaz_patrulla_norte`, etc.
- En cada cuadro: `indice = (tiempoAcumulado / segundosPorCuadro) %
  totalCuadros`
- Sprint: `segundosPorCuadro` se divide a la mitad.

## Ventana principal

- `QMainWindow` con `QStackedWidget` para cambiar entre pantallas.
- Captura teclas con `keyPressEvent` / `keyReleaseEvent`, ignora
  `isAutoRepeat()`.
- Tab: pausa + abre libreta. Esc: pausa/reanuda.
- `closeEvent`: guarda perfil.

## Vista del juego

- Capas de dibujo en orden:
  1. Suelo plano (solo casillas visibles, ~250 de 3600)
  2. Velo gris (`QColor(70, 75, 85, 145)`) sobre casillas bloqueadas +
     avisos de canones y trampa
  3. Profundidad: recolectar elementos, ordenar por `calcularOrdenDeDibujo()`,
     pintar de atras hacia adelante
  4. Atmosfera: capa de luz + temblor
- Camara fija en Nivel 1, seguidora en Nivel 2.
- Acuerdo de camara: 9 casillas a cada lado, 7 arriba y abajo.

## Capa de luz

- `factorDeOscuridad` 0.0 (dia) a 1.0 (noche cerrada).
- Linterna: circulo con `CompositionMode_DestinationOut`.
- Niebla del puerto: reduce radio 30%.
- Reutiliza un solo `QPixmap` del tamaño de la ventana.

## Temblor y transiciones

- Micro desplazamientos aleatorios durante 0.35 s.
- Parpadeo dia/noche: oscurece 0.4 s.
- Solo toca pixeles, jamas coordenadas logicas.

## Panel de datos

- Tiempo, provisiones, gemas, aliento, oportunidades de acusacion.
- Solo actualiza etiquetas cuando cambia el valor entero.
- Barra roja parpadeante bajo 20% de provisiones.
- Sol/luna junto al reloj. Semilla como `JUEGO #48291`.

## Libreta del investigador

- `QStackedWidget` con 2 paginas: pistas y fichas.
- Sello `DESCARTADO` (rojo) y `RESUELTO` (verde).
- Boton de acusacion: confirmacion antes de llamar al controlador.

## Cuadro de dialogo

- Texto progresivo: 1 letra cada 25 ms.
- Acertijo: 3 botones + opcion de gastar gema.
- `QTimer` se detiene al cerrar.

## Menus

- `MenuPrincipal`: nombre del usuario, rango, boton jugar/galeria/salir.
- `SelectorDeNivelYDificultad`: muestra parametros reales de cada
  dificultad, semilla manual opcional, tarjeta `CASO #n`.
- `GaleriaDeVillanos`: retratos + fragmentos de la carta.
- `PantallaDeFin`: victoria con desenmascaramiento o derrota con motivo
  exacto.

## Gestor de sonido

- `QMediaPlayer` para musica de fondo, `QSoundEffect` para efectos.
- Efectos precargados: `alerta_guardia`, `paso_hojas`,
  `engranajes_trampa`, `redoble_acusacion`, `explosion_canon`,
  `recoger_objeto`, `pulso_sensor`.
- Cambio de pista al entrar en climax.
- Solo sonidos con licencia libre. Creditos en README.

---

## Señales del controlador que la interfaz debe conectar

| Señal | Receptor |
|-------|----------|
| `alCambiarEtapa` | `VentanaPrincipal::cambiarPantallaActiva` |
| `alActualizarRecursos` | `PanelDeDatos::actualizarDatosEnPantalla` |
| `alOcurrirEventoVisual` | `VentanaPrincipal::alRecibirEventoVisualDesdeLogica` → sonido + efecto |
| `alCambiarMomentoDelDia` | `CapaDeLuz`, `PanelDeDatos` |
| `alIniciarConversacion` | `CuadroDeDialogo::mostrarConversacionSimple` |
| `alIniciarAcertijo` | `CuadroDeDialogo::mostrarAcertijoConTresOpciones` |
| `alAnotarEnLibreta` | `LibretaDelInvestigador::agregarAnotacionDePista` |
| `alTacharSospechoso` | `LibretaDelInvestigador::marcarSospechosoComoDescartado` |
| `alTerminarPartida` | `PantallaDeFin::mostrarResultado...` |
