#include <vector>

int thread_main(int argc, char ** argv, 
    // IO
    void (*confirm_tokens)(std::vector<std::tuple<double, double, std::string>>&),
    void (*update_transcript_buffer)(std::vector<std::tuple<double, double, std::string>>&),
    void (*new_audio_chunk)(double start, double end, std::vector<float>&),
    // perf stats
    void (*update_avg_token_lat)(double),
    // status
    void (*vad_on)(bool)
    ); 

