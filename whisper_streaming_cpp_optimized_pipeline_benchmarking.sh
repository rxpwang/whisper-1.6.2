#!/bin/bash

# Parameters from your command
model="ggml-medium.bin"
sample="bernie4min.wav"
dtw="medium"
threads="12"

# Array of different step lengths to loop through
step_lengths=(300 500 800 1000 2000)

# Loop over each step length
for step in "${step_lengths[@]}"; do
    # Create the log file name based on the parameters, including the current step length
    log_file="benchmarking_results/whisper_streaming_cpp_optimized_pipeline_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}.log"
    
    # Run the command and redirect the output to the log file
    ./whisper_streaming_cpp_optimized -m models/$model samples/$sample -kc -dtw $dtw -ac -1 -at audio_tag/medium_0.5s_avg.csv --step $step -t $threads > $log_file 2>&1
    
    echo "Output has been logged to $log_file"
done
