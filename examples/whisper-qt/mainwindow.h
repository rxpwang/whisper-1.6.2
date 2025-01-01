
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
#include <QPainter>

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
#define MY_WHISPER_SAMPLE_RATE      16000
#define DRAW_MOST_RECENT_SEC        30
#define MAX_WAVEFORM_SAMPLES        (MY_WHISPER_SAMPLE_RATE * DRAW_MOST_RECENT_SEC)
        // m_waveformData = data;
        // WHISPER_SAMPLE_RATE default 16K
        const int redraw_every_ms = 100; 
        const int redraw_every_samples = (redraw_every_ms * MY_WHISPER_SAMPLE_RATE) / 1000;
        m_waveformData.insert(m_waveformData.end(), data.begin(), data.end());
        n_undrawn_samples += data.size();
        current_wave_end += (data.size() / float(MY_WHISPER_SAMPLE_RATE));
        if (n_undrawn_samples > redraw_every_samples) {
            if (m_waveformData.size() > MAX_WAVEFORM_SAMPLES){
                current_wave_start += ((m_waveformData.size() - MAX_WAVEFORM_SAMPLES) / float(MY_WHISPER_SAMPLE_RATE));
                m_waveformData.erase(m_waveformData.begin(), m_waveformData.begin() + m_waveformData.size() - MAX_WAVEFORM_SAMPLES);
            }
            update(); // Trigger a repaint
            n_undrawn_samples = 0; 
        }
    }

    void UpdateAudioBufferInfo(double start, double end) {
        audio_buffer_start = start;
        audio_buffer_end = end / 1000;
        update();
    }

    void WhisperflowRestarting(bool need_restarting) {
        if (need_restarting) {
            current_wave_start = 0;
            current_wave_end = 0;
            audio_buffer_start = 0;
            audio_buffer_end = 0;
            m_waveformData.clear();
            update();
        }
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
            // drawing the audio buffer part with different color
            int audio_buffer_start_sample = ((audio_buffer_start - current_wave_start) * MY_WHISPER_SAMPLE_RATE);
            int audio_buffer_end_sample = ((audio_buffer_end - current_wave_start) * MY_WHISPER_SAMPLE_RATE);
            if (i >= audio_buffer_start_sample && i <= audio_buffer_end_sample) {
                glColor3f(1.0f, 0.84f, 0.0f); // Red color for the audio buffer
            } else {
                glColor3f(0.0f, 1.0f, 0.0f); // Green color for the waveform
            }
            glVertex2f(x, m_waveformData[i]);
        }
        glEnd();

        // Render start and end indices with QPainter
        QPainter painter(this);
        painter.setPen(Qt::white);  // Text color
        painter.setFont(QFont("Arial", 12));  // Font and size

        // Draw the start index (left side)
        QString startText = QString::number(current_wave_start);  // Starting index
        //painter.drawText(10, height() / 2, startText);
        painter.drawText(3, height() / 4, startText);

        // Draw the end index (right side)
        QString endText = QString::number(current_wave_end);  // Ending index
        // painter.drawText(width() - 30, height() / 2, endText);
        painter.drawText(width() - 30, height() / 4, endText);

        QString audioBufferStartText = QString::number(audio_buffer_start);
        QString audioBufferEndText = QString::number(audio_buffer_end);
        // calculate the audio buffer start and end text position
        float startpos = 3 + ((audio_buffer_start-current_wave_start) / (current_wave_end-current_wave_start)) * width();
        float endpos = ((audio_buffer_end-current_wave_start) / (current_wave_end-current_wave_start)) * width() - 30;
        painter.drawText(startpos, height() * 5 / 6, audioBufferStartText);
        painter.drawText(endpos, height() * 5 / 6, audioBufferEndText);
        painter.end();
    }

private:
    std::vector<float> m_waveformData;
    int n_undrawn_samples = 0; // # of audio samples not yet drawn
    float current_wave_start = 0; // the start index of the current waveform
    float current_wave_end = 0; // the end index of the current waveform
    float audio_buffer_start = 0; // the start index of the audio buffer
    float audio_buffer_end = 0; // the end index of the audio buffer
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

        label->setMinimumHeight(50); // Set the minimum height to 50 pixels
        //label->setMaximumHeight(100); // Allow resizing up to 100 pixels
        label->setMaximumHeight(QWIDGETSIZE_MAX); // Allow resizing up to the maximum height
        
        // auto *button = new QPushButton("Start", this);

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
        connect(worker, &Worker::signal_audio_buffer_info, this, &MainWindow::onAudioBufferInfo);
        connect(worker, &Worker::signal_whisperflow_restarting, this, &MainWindow::onWhisperflowRestarting);

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
#if 0        
        std::cout << "New audio chunk received: " << start << " " << end << "size" << data.size() << std::endl;
        std::cout << "First 10 items of data: ";
        for (size_t i = 0; i < std::min(data.size(), static_cast<size_t>(10)); ++i)
            std::cout << data[i] << " ";
        std::cout << std::endl;
#endif
        waveformWidget->setWaveformData(data);  // will redraw
    }

    void onAudioBufferInfo(double start, double end) {
        std::cout << "Audio buffer info: " << start << " " << end << std::endl;
        waveformWidget->UpdateAudioBufferInfo(start, end);
    }

    void onWhisperflowRestarting(bool need_restarting) {
        std::cout << "Whisperflow restarting: " << need_restarting << std::endl;
        waveformWidget->WhisperflowRestarting(need_restarting);
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
