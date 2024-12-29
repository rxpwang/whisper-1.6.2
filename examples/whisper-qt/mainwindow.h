
//----------------- waveform -----------------

/* using openGL. macOS supports OpenGL up to 2.1 (deprecated, migrated to Metal rendering)
    however we still use OpenGL for compatiblity
    check: 
    glxinfo|grep "OpenGL version"
*/

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
// #include <QOpenGLShaderProgram>
// #include <QOpenGLBuffer>

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

class WaveformWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget *parent = nullptr)
        : QOpenGLWidget(parent) {
        setStyleSheet("background-color: lightblue;");
    }

    void setWaveformData(const std::vector<float> &data)
    {
        m_waveformData = data;
        update(); // Trigger a repaint
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions(); // fxl:will crash inside?

        std::cout << "OpenGL Version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::cout << "GLSL Version:" << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        if (m_waveformData.empty())
            return;

        // Set up OpenGL rendering state
        glColor3f(0.0f, 1.0f, 0.0f); // Green color for the waveform
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < m_waveformData.size(); ++i) {
            float x = static_cast<float>(i) / (m_waveformData.size() - 1) * 2.0f - 1.0f; // Normalize to [-1, 1]
            glVertex2f(x, m_waveformData[i]);
        }
        glEnd();
    }

private:
    std::vector<float> m_waveformData;
};

//----------------- MainWindow -----------------

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMainWindow>

// #include "waveform.h"
#include "worker.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    int argc; char ** argv; // will receive args

    MainWindow(int argc, char **argv) : argc(argc), argv(argv) {
        // Create the central widget
        auto *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Create and set a layout for the central widget
        auto *layout = new QVBoxLayout(centralWidget);

        // Add the waveform widget
        waveformWidget = new WaveformWidget(this);
        waveformWidget->setMinimumHeight(50); // Minimum height of 50 pixels
        waveformWidget->setMaximumHeight(100); // Maximum height of 100 pixels
        layout->addWidget(waveformWidget);

        // Add the label
        label = new QLabel("whisper streaming", this);
        label->setWordWrap(true);
        label->setFixedWidth(500);
        label->setMinimumHeight(50); // Minimum height of 50 pixels
        label->setMaximumHeight(100); // Maximum height of 100 pixels
        layout->addWidget(label);

        // auto *waveformWidget = new WaveformWidget(this);
        // setCentralWidget(waveformWidget);
        // resize(800, 400);

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
        connect(worker, &Worker::signal_new_audio_chunck, this, &MainWindow::onNewAudioChunk);

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

    void onNewAudioChunk(double start, double end, const std::vector<float> &data) {
        // Do something with the new audio chunk
        // std::cout << "New audio chunk received: " << start << " " << end << "size" << data.size() << std::endl;
        waveformWidget->setWaveformData(data);  // will redraw
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
    WaveformWidget *waveformWidget;

    std::vector<std::tuple<double, double, std::string>> all_confirmed_tokens;
    std::vector<std::tuple<double, double, std::string>> unconfirmed_tokens;
    // fxl: audio_data. the UI thread trim it as needed
    std::vector<float> audio_data;  
    double start_time, end_time; 
};
