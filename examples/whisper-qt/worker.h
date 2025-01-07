#ifndef WORKER_H
#define WORKER_H

#include <iostream>
#include <vector>
#include <QObject>
#include <QString>
#include <QThread>

#include "whisper-qt.h" // our engine

class Worker : public QObject {
    Q_OBJECT // Required for signal-slot mechanism

public:
    int argc; 
    char ** argv; // will receive args
    // Worker() = default;
    Worker(int argc, char **argv) : argc(argc), argv(argv) {}
    static Worker* instance;    // the only instance of Worker

private: 
    // callback internals
    void confirm_tokens(std::vector<std::tuple<double, double, std::string>>& committed) {
        
        /* for (const auto& entry : committed) {
            double start_time, end_time;
            std::string transcript;
            std::tie(start_time, end_time, transcript) = entry;

            std::cout << "Start Time: " << start_time 
                      << ", End Time: " << end_time 
                      << ", Transcript: " << transcript 
                      << std::endl;
        } */
        emit signal_confirm_tokens(committed);
    };

    void unconfirmed_tokens(std::vector<std::tuple<double, double, std::string>>& unconfirmed) {
        /* for (const auto& entry : unconfirmed) {
            double start_time, end_time;
            std::string transcript;
            std::tie(start_time, end_time, transcript) = entry;

            std::cout << "Start Time: " << start_time 
                      << ", End Time: " << end_time 
                      << ", Transcript: " << transcript 
                      << std::endl;
        } */
        emit signal_unconfirmed_tokens(unconfirmed);
    };

    void new_audio_chunk(double start, double end, std::vector<float>& data) {
        // std::cout << "Start Time: " << start << ", End Time: " << end << std::endl;
        emit signal_new_audio_chunck(start, end, data);
    };

    void audio_buffer_info(double start, double end) {
        emit signal_audio_buffer_info(start, end);
    }

    void whisperflow_restarting(bool need_restarting) {
        emit signal_whisperflow_restarting(need_restarting);
    }

    void token_latency_info(std::vector<std::tuple<double, double, std::string, double>>& latency_record) {
        emit signal_token_latency_info(latency_record);
    }

    /////////////////
    // callback wrappers, must be static 
    static void confirm_tokens_callback(std::vector<std::tuple<double, double, std::string>>& tokens) {
        instance->confirm_tokens(tokens);
    }
    static void unconfirmed_tokens_callback(std::vector<std::tuple<double, double, std::string>>& tokens) {
        instance->unconfirmed_tokens(tokens);
    }
    static void new_audio_chunk_callback(double start_ms, double end_ms, std::vector<float>& data) {
        instance->new_audio_chunk(start_ms, end_ms, data);
    }
    static void audio_buffer_info_callback(double start, double end) {
        instance->audio_buffer_info(start, end);
    }
    static void whisperflow_restarting_callback(bool need_restarting) {
        instance->whisperflow_restarting(need_restarting);
    }
    static void token_latency_info_callback(std::vector<std::tuple<double, double, std::string, double>>& latency_record) {
        instance->token_latency_info(latency_record);
    }

public slots:
    // thread main body 
    void doWork() {
        thread_main(this->argc, this->argv, 
            // IO
            confirm_tokens_callback,
            unconfirmed_tokens_callback,
            new_audio_chunk_callback,
            audio_buffer_info_callback,
            whisperflow_restarting_callback,
            token_latency_info_callback,
            // perf stats
            NULL, // TBD
            // status
            NULL // TBD
        );
    }

signals:
    void updateText(const QString &text); // Signal to update text
    void finished(); // Signal to indicate work is finished
    // NB: even the args are passed by ref, Qt will make copies of them for the signal-slot mechanism. so thread safe
    void signal_confirm_tokens(const std::vector<std::tuple<double, double, std::string>>&); // Signal to confirm tokens
    void signal_unconfirmed_tokens(const std::vector<std::tuple<double, double, std::string>>&); // Signal to update unconfirmed tokens
    void signal_new_audio_chunck(double start, double end, const std::vector<float>&); // Signal to update audio chunk
    void signal_audio_buffer_info(double start, double end); // Signal to update audio buffer info
    void signal_whisperflow_restarting(bool need_restarting); // Signal to restart the process
    void signal_token_latency_info(const std::vector<std::tuple<double, double, std::string, double>>&); // Signal to update token latency
};

#endif // WORKER_H
