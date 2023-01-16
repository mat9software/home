git submodule update --init --recursive
cd stocks/imgui
git checkout -b docking origin/docking
cd ../../docker
docker build . -t main:latest
cd ..
source docker/docker_start.txt
cd /srv/stocks
make serve -j10

