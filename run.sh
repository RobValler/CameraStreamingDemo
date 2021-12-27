set -e
mkdir -p build || true
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j16
./Server/./CameraStreamingDemoServer &
./Client/./CameraStreamingDemoClient &
read -p "Press any key to continue... " -n1 -s
killall CameraStreamingDemoClient
killall CameraStreamingDemoServer
cd ..
