# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: GESTOR DE GUARDADO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `GestorDeGuardado` es responsable de la persistencia atómica y la deserialización robusta del `PerfilDeUsuario` en disco utilizando `QSaveFile` de Qt. Garantiza que ante un fallo de energía, caída del programa o archivo corrupto, nunca se destruya la partida previa. Aplica validación estricta de tipos numéricos mediante conversiones seguras con control de errores (`toInt(&ok)`), cargando los datos en una instancia temporal antes de sobreescribir el perfil en memoria.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "guardado/perfil.h"
#include <QString>
#include <QSaveFile>
#include <QFile>
#include <QTextStream>
#include <cstdint>
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class GestorDeGuardado {
public:
    GestorDeGuardado();
    explicit GestorDeGuardado(QString rutaDeArchivoPersonalizada);
    ~GestorDeGuardado() = default;

    bool guardarPerfil(const PerfilDeUsuario& perfil);
    bool cargarPerfil(PerfilDeUsuario& perfilDestino);
    bool existeArchivoDeGuardado() const;
    void eliminarArchivoDeGuardado();

    const QString& obtenerRutaDeArchivo() const;

private:
    QString rutaDeArchivo;
};
```

## 4. FORMATO DE ARCHIVO Y CONSTANTES
El archivo se guarda en texto plano delimitado por claves y valores para máxima legibilidad e inspección:
```text
NOMBRE=Scooby Detective
PUNTAJE=1250
CASOS=1
TIEMPO=430.5
HISTORIAL=Mision 1 completada;Mision 2 completada;
```

```cpp
namespace ConstantesGestorDeGuardado {
    inline const QString NOMBRE_ARCHIVO_POR_DEFECTO = QStringLiteral("perfil_detective_scooby.dat");
}
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `guardarPerfil` debe utilizar obligatoriamente `QSaveFile`. Si la escritura se interrumpe, `QSaveFile::cancelWriting()` asegura que el archivo original no sufra corrupción.
2. `cargarPerfil` nunca muta directamente `perfilDestino` hasta que todos los campos requeridos hayan sido leídos y validados con éxito en una instancia temporal local `PerfilDeUsuario perfilTemporal`.
3. Todas las lecturas de enteros o números flotantes deben verificar la bandera de conversión (por ejemplo: `bool conversionExitosa = false; int valor = linea.toInt(&conversionExitosa);`). Si la conversión falla, la carga se aborta y se reporta error, protegiendo al motor de estados inconsistentes.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `guardarPerfil`
1. Instancia `QSaveFile archivoSeguro(rutaDeArchivo)`.
2. Si `!archivoSeguro.open(QIODevice::WriteOnly | QIODevice::Text)`, retorna falso.
3. Instancia `QTextStream flujoSalida(&archivoSeguro)`.
4. Escribe cada campo en formato `CLAVE=VALOR\n`.
5. Serializa el historial uniendo las cadenas con delimitador `;`.
6. Invoca `archivoSeguro.commit()`. Retorna verdadero si la operación de commit fue exitosa.

### `cargarPerfil`
1. Instancia `QFile archivo(rutaDeArchivo)`.
2. Si `!archivo.exists()` o `!archivo.open(QIODevice::ReadOnly | QIODevice::Text)`, retorna falso.
3. Instancia `PerfilDeUsuario perfilTemporal`.
4. Lee línea por línea con `QTextStream flujoEntrada(&archivo)`.
5. Separa cada línea por el primer carácter `=` encontrado.
6. Valida y asigna según la clave:
   - `NOMBRE`: invoca `perfilTemporal.establecerNombre(...)`.
   - `PUNTAJE`: convierte con `toInt(&ok)`. Si `!ok`, aborta y retorna falso. Invoca `perfilTemporal.sumarPuntaje(...)`.
   - `CASOS`: convierte con `toInt(&ok)`. Si `!ok`, aborta y retorna falso.
   - `TIEMPO`: convierte con `toFloat(&ok)`. Si `!ok`, aborta y retorna falso. Invoca `perfilTemporal.acumularTiempoJugado(...)`.
   - `HISTORIAL`: divide por `;` y agrega cada elemento a `perfilTemporal.agregarMisionAlHistorial(...)`.
7. Si la lectura de todo el archivo fue exitosa, copia `perfilDestino = perfilTemporal` y retorna verdadero.
