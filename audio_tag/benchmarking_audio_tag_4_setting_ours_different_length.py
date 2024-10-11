import numpy as np
import pandas as pd
import subprocess

file_name = pd.read_csv("samples/librispeech_test_other_path_within_25s.txt", header=None)
file_names = file_name.values
#print(file_name[0][0])

for file_name in file_names:
    file = file_name[0]
    wav_file = file.replace(".flac", ".wav")
    print(f"Converted {file} to {wav_file}")

    # Commands to run on the converted .wav file
    commands = [
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/medium_0.5s_avg.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_ours_0.5_avg.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/medium_1s_avg.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_ours_1_avg.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/medium_1.5s_avg.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_ours_1.5_avg.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/medium_2s_avg.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_ours_2_avg.json"

    ]

    for command in commands:
        # Run each command
        subprocess.run(command, shell=True, check=True)
        print(f"Command '{command}' executed successfully!")
