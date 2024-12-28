pushd .
cd ../../
make libwhisper.a -j4
make lib_whisper_stream.a -j4
popd 

# must clean ... how to enforce check lib dep?
make clean
make -j4