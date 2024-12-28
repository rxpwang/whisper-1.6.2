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
        label->setWordWrap(true);
        label->setFixedWidth(500);

        label->setMinimumHeight(50); // Set the minimum height to 50 pixels
        label->setMaximumHeight(100); // Allow resizing up to 100 pixels

        // auto *button = new QPushButton("Start", this);

        layout->addWidget(label);
        // layout->addWidget(button);

        // connect(button, &QPushButton::clicked, this, &MainWindow::startWorker);
        startWorker(); 
    }

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
        connect(worker, &Worker::signal_unconfirmed_tokens, this, &MainWindow::onUnconfirmedTokens);

        workerThread->start();
    }

private slots:
    void updateLabel(const QString &text) {
        label->setText(text); // Update UI
    }

    void onConfirmedTokens(const std::vector<std::tuple<double, double, std::string>> &tokens) {
        all_confirmed_tokens.insert(all_confirmed_tokens.end(), tokens.begin(), tokens.end());        
        render_text();
    }

    void onUnconfirmedTokens(const std::vector<std::tuple<double, double, std::string>> &tokens) {
        unconfirmed_tokens = tokens;
        render_text();
    }

private:
    void render_text(void) {
        // render the confirmed tokens (only the last N tokens)
        int N = 100; 
        QString text = "<font color='green'><b>"; // HTML format
        unsigned long start_index = std::max(0, static_cast<int>(all_confirmed_tokens.size()) - N);
        for (unsigned long i = start_index; i < all_confirmed_tokens.size(); ++i) {
            double start_time, end_time;
            std::string transcript;
            std::tie(start_time, end_time, transcript) = all_confirmed_tokens[i];
            text += QString::fromStdString(transcript) + " ";
        }
        text += "</b></font>";
        
        // render all the unconfirmed tokens
        text += "<font color='gray'>"; // HTML format
        for (const auto& token : unconfirmed_tokens) {
            double start_time, end_time;
            std::string transcript;
            std::tie(start_time, end_time, transcript) = token;
            text += QString::fromStdString(transcript) + " ";
        }
        text += "</font>";

        std::cout << text.toStdString() << std::endl;

        label->setText(text);
    }

    QLabel *label;
    std::vector<std::tuple<double, double, std::string>> all_confirmed_tokens;
    std::vector<std::tuple<double, double, std::string>> unconfirmed_tokens;
};