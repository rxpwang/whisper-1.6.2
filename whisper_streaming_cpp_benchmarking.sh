#!/bin/bash

# Parameters from your command
model="ggml-medium.bin"
sample="bernie4min.wav"
dtw="medium"

# Get the number of P-core count on macOS
threads=$(($(sysctl -n hw.perflevel0.physicalcpu)-1))
# Fallback to a default value if the command fails
if [ -z "$threads" ]; then
    threads="12"
fi

# Array of different step lengths to loop through
step_lengths=(300 400 500)
step=1000
runs=5
#step_lengths=(800 1000)

# Loop over each step length
for i in $(seq 1 $runs); do
    echo "Run ${i}"
    # Create the log file name based on the parameters, including the current step length
    log_file="benchmarking_results/baseline_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_with_token_latency_record_run-${i}.log"
    # log_file_with_audio_tag="benchmarking_results/baseline_${model}_${sample}_dtw-${dtw}_step-${step}_t-${threads}_with_token_latency_record_audio_tag_run-${i}.log"
    # Run the command and redirect the output to the log file
    ./whisper_streaming_cpp -m models/$model samples/$sample -kc -dtw $dtw --step $step -t $threads > $log_file 2>&1
    echo "Output has been logged to $log_file"

    # ./whisper_streaming_cpp -m models/$model samples/$sample -kc -dtw $dtw --step $step -t $threads -at audio_tag/medium_0.5s_avg.csv > $log_file_with_audio_tag 2>&1
    # echo "Output has been logged to $log_file_with_audio_tag"
    
done

if [[ "$OSTYPE" == "darwin"* ]]; then
    system_profiler SPHardwareDataType > benchmarking_results/00-systeminfo.txt
fi
# Append the current git commit hash to the system info file
echo "Current Git Commit:" >> benchmarking_results/00-systeminfo.txt
git rev-parse HEAD >> benchmarking_results/00-systeminfo.txt