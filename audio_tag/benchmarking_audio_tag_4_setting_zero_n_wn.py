import numpy as np
import pandas as pd
import subprocess

file_name = pd.read_csv("samples/librispeech_test_other_path_within_25s.txt", header=None)
file_names = file_name.values
#print(file_name[0][0])

for file_name in file_names:
    file = file_name[0]
    wav_file = file.replace(".flac", ".wav")
    #convert_command = f"ffmpeg -i {file} -ac 1 -ar 16000 {wav_file}"
    #subprocess.run(convert_command, shell=True, check=True)
    print(f"Converted {file} to {wav_file}")

    # Commands to run on the converted .wav file
    commands = [
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/zero_tag_2.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_zero_2.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/zero_tag_4.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_zero_4.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/wn_tag_2.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_wn_2.json",
                f"./main -m models/ggml-medium.bin {wav_file} -ac -1 -at audio_tag/wn_tag_4.csv -oj -ojf",
                f"mv {wav_file}.json {wav_file}_wn_4.json",

    ]

    for command in commands:
        # Run each command
        subprocess.run(command, shell=True, check=True)
        print(f"Command '{command}' executed successfully!")
