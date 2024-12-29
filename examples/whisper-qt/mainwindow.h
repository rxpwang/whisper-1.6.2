
//----------------- waveform -----------------

/* using openGL. macOS supports OpenGL up to 2.1 (deprecated, migrated to Metal rendering)
    however we still use OpenGL for compatiblity
    check: 
    glxinfo|grep "OpenGL version"
*/

#include <QOpenGLWidget>
// #include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

class WaveformWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
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

        // Normalize data to [-1, 1]
        float range = m_maxValue - m_minValue;
        m_normalizedData.clear();
        for (float val : m_waveformData) {
            m_normalizedData.push_back((val - m_minValue) / range * 2.0f - 1.0f);
        }

        // print the first 10 items of the normalized data
        // for (int i = 0; i < 10; i++) {
        //     std::cout << m_normalizedData[i] << " ";
        // }

        update(); // Trigger a repaint
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions(); // fxl:will crash inside?

        std::cout << "OpenGL Version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::cout << "GLSL Version:" << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Compile shaders
        m_program = new QOpenGLShaderProgram(this);
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
            #version 120
            attribute vec2 position;
            uniform mat4 projection;
            void main() {
                gl_Position = projection * vec4(position, 0.0, 1.0);
            }
        )");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
            #version 120
            void main() {
                gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
            }
        )");
        m_program->link();

        // Prepare vertex buffer
        m_vbo.create();
        m_vbo.bind();
    }

    void resizeGL(int w, int h) override {
        // Create orthographic projection matrix
        QMatrix4x4 projection;
        projection.ortho(0.0f, float(w), -1.0f, 1.0f, -1.0f, 1.0f);

        // Set the projection matrix in the shader
        m_program->bind();
        m_program->setUniformValue("projection", projection);
        m_program->release();
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        if (m_waveformData.empty())
            return;

        // Prepare vertex data
        std::vector<float> vertices;
        for (size_t i = 0; i < m_waveformData.size(); ++i) {
            float x = float(i) / (m_waveformData.size() - 1) * width();
            float y = (m_waveformData[i] - m_minValue) / (m_maxValue - m_minValue) * 2.0f - 1.0f;
            vertices.push_back(x);
            vertices.push_back(y);
        }

        // Upload vertex data to VBO
        m_vbo.bind();
        m_vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(float)));

        // Render
        m_program->bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

        glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(vertices.size() / 2));

        glDisableVertexAttribArray(0);
        m_program->release();
        m_vbo.release();
    }

private:
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer m_vbo { QOpenGLBuffer::VertexBuffer };
    std::vector<float> m_waveformData;
    std::vector<float> m_normalizedData;
    float m_minValue = -1.0f;
    float m_maxValue = 1.0f;
};

//----------------- MainWindow -----------------

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
// #include "waveform.h"
#include "worker.h"


class MainWindow : public QWidget {
    Q_OBJECT

public:
    int argc; char ** argv; // will receive args

    MainWindow(int argc, char **argv) : argc(argc), argv(argv) {
        auto *layout = new QVBoxLayout(this);

        waveformWidget = new WaveformWidget(this);
        waveformWidget->setMinimumHeight(50); // Set the minimum height to 50 pixels
        waveformWidget->setMaximumHeight(100); // Allow resizing up to 100 pixels        
        layout->addWidget(waveformWidget);
        // waveformWidget->show();

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
