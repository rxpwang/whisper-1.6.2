### Measurement improvement
- per token latency [done]
- serialized pipeline w/o audio tag [done]

### Profiling
- pipeline efficiency
  - bubble time between CPU decoding and GPU decoding [done]
  - the cause of hallucination [done]

### Bugs
- segfault
  - buffer.insert might segfault for ngram size of 2 [done]

### Benchmarking
- M2 Pro
  - [x] run serialized pipeline with base model (w/ audio_tag and w/o audio_tag)
  - [x] search decoding length (1/4, 1/3/, 1/2, 2/3, 3/4)

- M3 Pro
  - [ ] search decoding length (1/4, 1/3/, 1/2, 2/3, 3/4)

- Dataset
  - [ ] 5 minute audio datasets on TED

### Paper Writing
- Introduction
  - [ ] Figures
    - [ ] Batch
    - [ ] Serialized pipeline
    - [ ] WhisperFlow

#### Benchmark on all data

### M2 / M4 evaluation
| system | whisper-base | whisper-medium|
| ------ | --------------| ---|
| ours (pipeline-v1) |  | w/ tag: 4722.0 (+-528.1) ms,| 
| serialized |  | w/ tag: 3014.0 (+-437.3) ms, |
| baseline   |  | w/o tag: 7434.0 (+-1058.8) ms, |


### M3 Pro evaluation
5P + 6E + 14 GPU, 18GB memory

#### Major results
#### Major result
| system | whisper-base | whisper-medium |
| ------ | --------------| ---|
| ours (pipeline-v1), 7 CPU threads, 3 GPU threads | w/tag: 74.0 (+-48.5) ms, w/o tag: 162.0 (+-28.7) ms | w/ tag: 1447.9 (+-39.6) ms, w/o tag: 3130.9 (+-54.2) ms | 
| serialized (5 threads)    |      | w/ tag: 1661.0 (+-175.9) ms, w/o tag: 3487.5 (+-410.6) ms
| baseline           |      |      |

#### Grid search on CPU / GPU function CPU thread counts
Using 10 cores in total
##### Base
| Ours | w/ audio tag | w/o audio tag |
| ------------ | ------------ | ------------- |
| CPU:9, GPU:1 | 141.4 (+-23.5) ms | 200.5 (+-22.8) ms |
| CPU:8, GPU:2 | 102.0 (+-54.6) ms | 187.0 (+-50.0) ms |
| CPU:7, GPU:3 | 74.0 (+-48.5) ms | 162.0 (+-28.7) ms |
| CPU:6, GPU:4 | 93.9 (+-50.0) ms | 134.6 (+-30.8) ms |
| CPU:5, GPU:5 | 88.3 (+-40.5) ms | 137.6 (+-21.0) ms |

##### Medium
| Ours | w/ audio tag | w/o audio tag |
| ------------ | ------------ | ------------- |
| CPU:9, GPU:1 | 1634.7 (+-190.5) ms | 3116.3 (+-25.3) ms |
| CPU:8, GPU:2 | 1457.2 (+-93.6) ms | 3170.3 (+-185.0) ms |
| CPU:7, GPU:3 | 1447.9 (+-39.3) ms | 3130.9 (+-54.2) ms |
| CPU:6, GPU:4 | 1477.9 (+-108.8) ms | 2975.2 (+-114.6) ms |
| CPU:5, GPU:5 | 1791.4 (+-223.6) ms | 3439.1 (+-227.5) ms |

### M2 Pro evaluation
8P + 4E + 19 GPU, 32GB memory
#### Major result
| system | whisper-base | whisper-medium |
| ------ | --------------| ---|
| ours (pipeline-v1) | w/ tag: 190.5 (+-47.5) ms, w/o tag: 217.4 (+-34.3) ms | w/ tag: 1170.4 (+-64.6) ms, w/o tag: 2389.4 (+-178.6) ms | 
| serialized         | w/ tag: 175.8 (+-32.4) ms, w/o tag: 389.2 (+-73.2) ms | w/ tag: 1362.6 (+-144.3) ms, w/o tag: 2766.0 (+-57.0) ms |
| baseline           |      | w/ tag: 3079.3 (+-260.7) ms, w/o tag: 3396.1 (+-30.9) ms |


#### Grid search on CPU / GPU function CPU thread counts
Using 10 cores, part of them is E cores

##### Medium

| Ours | w/ audio tag | w/o audio tag |
| ------------ | ------------ | ------------- |
| CPU:9, GPU:1 | 1457.4 (+-73.9) ms | 3362.3 (+-67.2) ms |
| CPU:8, GPU:2 | 1424.7 (+-79.0) ms | 2639.1 (+-83.9) ms |
| CPU:7, GPU:3 | 1290.2 (+-150.7) ms | 2575.7 (+-123.2) ms |
| CPU:6, GPU:4 | 1288.7 (+-146.7) ms  | 2502.8 (+-200.1) ms |
| CPU:5, GPU:5 | 1242.0 (+-129.6) ms  | 2595.4 (+-82.6) ms |

##### Base
| Ours | w/ audio tag | w/o audio tag |
| ------------ | ------------ | ------------- |
| CPU:9, GPU:1 | 310.7 (+-25.2) ms | 266.3 (+-28.5) ms |
| CPU:8, GPU:2 | 239.5 (+-41.1) ms | 193.9 (+-21.5) ms |
| CPU:7, GPU:3 | 208.0 (+-29.0) ms | 187.5 (+-20.5) ms |
| CPU:6, GPU:4 | 213.7 (+-66.1) ms | 167.2 (+-37.7) ms |
| CPU:5, GPU:5 | 164.4 (+-31.2) ms | 149.7 (+-23.6) ms |

#### Search on GPU decoding round
CPU: 5 threads, GPU: 5 threads

##### Base
| ratio of reference length | w/ audio tag | w/o audio tag |
| ---------- | ---------------- | ---------------- |
| 1/4        | 18.1 (+-14.7) ms | 53.2 (+-16.8) ms |
| 1/3        | 6 ms (+-26.4) ms (there are negatives) | 102.0 (+-24.1) ms |
| 1/2        | 188.8 (+-46.7) ms | 219.9 (+-112.5) ms |
| 2/3        | 231.6 (+-36.3) ms | 264.8 (+-16.6) ms |
| 3/4        | 228.6 (+-16.8) ms | 329.2 (+-45.5) ms |

##### Medium
| ratio of reference length | w/ audio tag | w/o audio tag |
| ---------- | ---------------- | ---------------- |
| 1/4        | 1434.9 (+-146.4) ms | 2369.1 (+-133.4) ms |
| 1/3        | 1170.4 (+-64.6) ms | 2389.4 (+-178.6) ms |
| 1/2        | 1270.3 (+-106.9) ms | 2560.2 (+-54.3) ms |
| 2/3        | 1425.1 (+-55.7) ms | 2790.0 (+-81.3) ms |
| 3/4        | 1604.3 (+-110.5) ms | 3725.4 (+-1444.3) ms |

### M2 Ultra evaluation
| system | whisper-medium | whisper-large|
| ------ | --------------| ---|
| ours (pipeline-v1) |  |   | 
| serialized |  |  |
| baseline   |  |  |

### M2 Max evaluation
#### Major result
| system | whisper-medium | whisper-large|
| ------ | --------------| ---|
| ours (pipeline-v1) | w/ tag: 807.2 (+-69.3) ms, w/o tag: 1658.3 (+-71.8) ms |   | 
| serialized | w/ tag: 897.7 (+-127.4) ms, w/o tag: 1826.5 (+-147.8) ms |  |
| baseline   | w/ tag: 3423.7 (+-988.6) ms, w/o tag: 2524.5 (+-334.7) ms |  |

#### Grid search on CPU / GPU function CPU thread counts
Using 10 cores, part of them is E cores

| Ours | w/ audio tag | w/o audio tag |
| ------------ | ------------ | ------------- |
| CPU:9, GPU:1 | - | - |
| CPU:8, GPU:2 | - | - |
| CPU:7, GPU:3 | 949.3 (+-97.6) ms | 1654.2 (+-9.6) ms |
| CPU:6, GPU:4 | 807.2 (+-69.3) ms  | 1658.3 (+-71.8) ms |
| CPU:5, GPU:5 | 920.2 (+-198.0) ms  | 1604.9 (+-38.2) ms |