#include <vector>
#include <tuple>
#include <string>

#include <QApplication>
#include "mainwindow.h"

Worker *Worker::instance = nullptr; // must instantiate the static member....

using VectorTuple = std::vector<std::tuple<double, double, std::string>>;
using VectorFloat = std::vector<float>;

Q_DECLARE_METATYPE(VectorTuple)
Q_DECLARE_METATYPE(VectorFloat)

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Register the type before using it in signals/slots
    qRegisterMetaType<std::vector<std::tuple<double, double, std::string>>>("std::vector<std::tuple<double, double, std::string>>");
    qRegisterMetaType<std::vector<float>>("std::vector<float>");
    
    MainWindow window(argc, argv);
    window.show();

    return app.exec();
}


