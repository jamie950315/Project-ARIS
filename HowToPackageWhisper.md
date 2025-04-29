//check if files are under these directory 
ls /usr/local/lib/libopenblas*
ls /home/pi/myenv
ls /home/pi//whisper.cpp

//create a temporary folder
cd /home/pi
mkdir bundle_temp

//copy and paste libraries 
mkdir -p bundle_temp/usr/local/lib
cp /usr/local/lib/libopenblas* bundle_temp/usr/local/lib/

mkdir -p bundle_temp/home/pi/whisper.cpp
cp -r /home/pi/whisper.cpp bundle_temp/home/pi/whisper.cpp/

mkdir -p bundle_temp/home/pi
cp -r /home/pi/myenv bundle_temp/home/pi/

//compress to tar.gz
cd /home/pi
sudo tar -czpf whisper_bundle.tar.gz \
  --numeric-owner \
  -C bundle_temp . 

//delete raw folder (optional)
rm -rf bundle_temp