#!/bin/bash

# Parameters from your command
dtw="medium"
model="ggml-${dtw}.bin"
sample="bernie4min.wav"

# Get the number of P-core count on macOS
threads=10

# Array of different step lengths to loop through
step=100

# Number of repetitions
repetitions=10

# Loop over each step length
for i in $(seq 1 $repetitions); do
    # Create the log file name based on the parameters, including the current step length
    log_file="benchmarking_results/whisper_streaming_cpp_optimized_pipeline_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_run-${i}.log"
    log_file_no_audio_tag="benchmarking_results/whisper_streaming_cpp_optimized_pipeline_no_audio_tag_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_run-${i}.log"
    
    # Run the command and redirect the output to the log file
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/${dtw}_0.5s_avg.csv --step $step -t $threads > $log_file 2>&1
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw --step $step -t $threads > $log_file_no_audio_tag 2>&1
    
    echo "Output has been logged to $log_file"
    # Pause for 1 minute between repetitions, unless it's the last one
    if [ "$i" -lt "$repetitions" ]; then
        echo "Waiting for 1 minute before the next run..."
        sleep 60
    fi
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt
git rev-parse HEAD >> benchmarking_results/00-systeminfo_${model}_whisper_streaming_cpp_pipeline.txt