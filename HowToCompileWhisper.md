//install dependencies
sudo apt update
sudo apt install -y build-essential cmake git pkg-config gfortran \
    libasound2-dev portaudio19-dev python3-dev python3-pip

//download OpenBLAS
cd ~
git clone https://github.com/xianyi/OpenBLAS.git
cd OpenBLAS

//compile OpenBLAS
make -j4 //-j4 require 4 threads, depends on your board

//install OpenBLAS
sudo make PREFIX=/usr/local install
sudo ldconfig

//setup environment variable
export OPENBLAS_ROOT=/usr/local
export LD_LIBRARY_PATH=$OPENBLAS_ROOT/lib:$LD_LIBRARY_PATH
export C_INCLUDE_PATH=$OPENBLAS_ROOT/include:$C_INCLUDE_PATH
export PKG_CONFIG_PATH=$OPENBLAS_ROOT/lib/pkgconfig:$PKG_CONFIG_PATH

//verify installation
ls /usr/local/lib | grep openblas //should see "libopenblas.so"

python
import numpy as np
np.show_config() //check for "blas_info" and "lapack_opt_info"

exit()

//download whisper.cpp
cd ~
git clone https://github.com/ggml-org/whisper.cpp.git
cd whisper.cpp

//build whisper.cpp
mkdir build
cd build
//or
//cmake ..   -DGGML_BLAS=1   -DCMAKE_BUILD_TYPE=Release   -DCMAKE_PREFIX_PATH=$OPENBLAS_ROOT   -DBLAS_LIBRARIES=$OPENBLAS_ROOT/lib/libopenblas.so   -DBLAS_INCLUDE_DIRS=$OPENBLAS_ROOT/include
cmake .. -DGGML_BLAS=1
make -j4

//verify compile
./bin/whisper-cli -h

//test (optional)
cd ../models
wget -O ggml-tiny.en.bin https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-tiny.en.bin
cd ..
./build/bin/whisper-cli -m models/ggml-tiny.en.bin -f samples/jfk.wav -t 4 //-t 4 require 4 threads, depends on your board

//setup venv
cd ~
sudo apt install python3-full python3-venv
python3 -m venv ~/myenv
source ~/myenv/bin/activate
pip install numpy pandas flask openai pyserial

//install packages
python3 -m pip install --upgrade pip
pip install sounddevice numpy

//download pywhispercpp
cd ~
git clone --recursive https://github.com/abdeladim-s/pywhispercpp.git
cd pywhispercpp

//compile pywhispercpp
export GGML_BLAS=1

//install pywhispercpp
pip install . --no-cache-dir --force-reinstall -v

//verify install
python
from pywhispercpp.model import Model
model = Model('tiny.en', n_threads=1) //make sure "whisper_backend_init: using BLAS backend" exist

exit()

//DONE!!!












