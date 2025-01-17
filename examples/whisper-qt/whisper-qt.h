#include <vector>

int thread_main(int argc, char ** argv, 
    // IO
    void (*confirm_tokens)(std::vector<std::tuple<double, double, std::string>>&),
    void (*update_transcript_buffer)(std::vector<std::tuple<double, double, std::string>>&),
    void (*new_audio_chunk)(double start, double end, std::vector<float>&),
    void (*audio_buffer_info)(double start, double end),
    void (*whisperflow_restarting)(bool need_restarting),
    void (*token_latency_info)(std::vector<std::tuple<double, double, std::string, double>>&),
    // perf stats
    void (*update_avg_token_lat)(double),
    // status
    void (*vad_on)(bool)
    ); 

int thread_main_baseline(int argc, char ** argv, 
    // IO
    void (*confirm_tokens)(std::vector<std::tuple<double, double, std::string>>&),
    void (*update_transcript_buffer)(std::vector<std::tuple<double, double, std::string>>&),
    void (*new_audio_chunk)(double start, double end, std::vector<float>&),
    void (*audio_buffer_info)(double start, double end),
    void (*whisperflow_restarting)(bool need_restarting),
    void (*token_latency_info)(std::vector<std::tuple<double, double, std::string, double>>&),
    // perf stats
    void (*update_avg_token_lat)(double),
    // status
    void (*vad_on)(bool)
    ); 
