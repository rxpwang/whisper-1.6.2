#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "worker.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    int argc; char ** argv; // will receive args

    MainWindow(int argc, char **argv) : argc(argc), argv(argv) {
        auto *layout = new QVBoxLayout(this);

        label = new QLabel("whisper streaming", this);
        auto *button = new QPushButton("Start", this);

        layout->addWidget(label);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, &MainWindow::startWorker);
    }

private slots:
    void startWorker() {
        auto *worker = new Worker(argc, argv);
        auto *workerThread = new QThread;

        Worker::instance = worker; // Save the only instance

        worker->moveToThread(workerThread);

        connect(workerThread, &QThread::started, worker, &Worker::doWork);
        connect(worker, &Worker::updateText, this, &MainWindow::updateLabel);
        connect(worker, &Worker::finished, workerThread, &QThread::quit);
        connect(worker, &Worker::finished, worker, &Worker::deleteLater);
        connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

        connect(worker, &Worker::signal_confirm_tokens, this, &MainWindow::onConfirmedTokens);

        workerThread->start();
    }

    void updateLabel(const QString &text) {
        label->setText(text); // Update UI
    }

    void onConfirmedTokens(const std::vector<std::tuple<double, double, std::string>> &tokens) {
        QString text = "";
        for (const auto &entry : tokens) {
            double start_time, end_time;
            std::string transcript;
            std::tie(start_time, end_time, transcript) = entry;
            text += QString::fromStdString(transcript) + " ";
        }
        label->setText(text);
    }

private:
    QLabel *label;
};