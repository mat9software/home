# Self contained imgui emscripten example.

git clone git@github.com:mat9software/home.git
cd home
git submodule update --init --recursive
cd 3rdparty/imgui
git checkout -b docking origin/docking
cd ../../docker
docker build . -t mat9software:latest
cd ..
ln -s barebone deploy
source docker/docker_start.source
cd barebone
make serve -j10

#Outside of docker, you want to commit empscripten cache
docker ps
docker commit <container> mat9software:latest
