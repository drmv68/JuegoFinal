#include <QApplication>
#include <QMessageBox>
#include <stdexcept>
#include "ventana_principal.h"

int main(int argc, char *argv[])
{
    QApplication aplicacion(argc, argv);
    aplicacion.setApplicationName("ScoobyCartagena");
    aplicacion.setOrganizationName("ScoobyCartagena");

    try {
        VentanaPrincipal ventana;
        ventana.show();
        return aplicacion.exec();
    } catch (const std::exception& errorCapturado) {
        QMessageBox::critical(nullptr,
            "Error al iniciar ScoobyCartagena",
            QString::fromStdString(errorCapturado.what()));
        return 1;
    }
}
