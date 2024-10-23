#!/bin/bash

# Parameters from your command
model="ggml-medium.bin"
sample="bernie4min.wav"
dtw="medium"
# Get the number of P-core count on macOS
threads=$(sysctl -n hw.perflevel0.physicalcpu)
# Fallback to a default value if the command fails
if [ -z "$threads" ]; then
    threads="12"
fi


# Array of different step lengths to loop through
#step_lengths=(300 500 800 1000 2000)
step_lengths=(300 400 500)

# Loop over each step length
for step in "${step_lengths[@]}"; do
    # Create the log file name based on the parameters, including the current step length
    log_file="benchmarking_results/whisper_streaming_cpp_optimized_pipeline_serialized_all_gpu_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_token_latency_recorded_hallucination_fixed.log"
    log_file_no_audio_tag="benchmarking_results/whisper_streaming_cpp_optimized_pipeline_serialized_all_gpu_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_no_tag_token_latency_recorded_hallucination_fixed.log"
    
    echo "will Output to $log_file ... running..."

    # Run the command and redirect the output to the log file
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/medium_0.5s_avg.csv --step $step -t $threads > $log_file 2>&1
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -t $threads > $log_file_no_audio_tag 2>&1
    
    echo "Output has been logged to $log_file"
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo.txt
fi