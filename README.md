# Mat9 Software

# First release HOW-TO

git clone git@github.com:mat9software/home.git
cd home
git submodule update --init --recursive
cd stocks/imgui
git checkout -b docking origin/docking
cd ../../docker
docker build . -t main:latest
cd ..
source docker/docker_start.txt
cd /srv/stocks
make serve -j10

#Outside of docker, you want to commit empscripten cache
docker ps
docker commit <container> main:latest
