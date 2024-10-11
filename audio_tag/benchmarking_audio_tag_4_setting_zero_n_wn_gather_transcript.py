import numpy as np
import pandas as pd
import subprocess
import json

file_name = pd.read_csv("samples/librispeech_test_other_path_within_25s.txt", header=None)
file_names = file_name.values
#print(file_name[0][0])

def extract_transcription_text(json_file):
    # Load the JSON data
    with open(json_file, 'r', encoding='utf-8', errors="replace") as file:

        data = json.load(file)

    # Extract transcription text
    transcription_text = ""
    for item in data.get("transcription", []):
        transcription_text += item.get("text", "") + " "

    # Return the combined transcription text
    return transcription_text.strip()

file_suffixs = ['_zero_2.json', '_zero_4.json', '_wn_2.json', '_wn_4.json']
default_transcript = []
no_padding_transcript = []
ours_transcript = []
zero_padding_transcript = []
for file_name in file_names:
    file = file_name[0]
    wav_file = file.replace(".flac", ".wav")
    # the file to read: wav_file_1-4.json
    for i in range(len(file_suffixs)):
        tmp_transcript = extract_transcription_text(wav_file + file_suffixs[i])
        if i == 0:
            default_transcript.append(tmp_transcript)
        elif i == 1:
            no_padding_transcript.append(tmp_transcript)
        elif i == 2:
            ours_transcript.append(tmp_transcript)
        else:
            zero_padding_transcript.append(tmp_transcript)

df = pd.DataFrame(default_transcript)
df.to_csv("samples/zero_2_transcript.txt", index=False, header=False)
df = pd.DataFrame(no_padding_transcript)
df.to_csv("samples/zero_4_transcript.txt", index=False, header=False)
df = pd.DataFrame(ours_transcript)
df.to_csv("samples/wn_2_transcript.txt", index=False, header=False)
df = pd.DataFrame(zero_padding_transcript)
df.to_csv("samples/wn_4_transcript.txt", index=False, header=False)
