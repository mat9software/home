FROM ubuntu:22.04

#How to use :
#
#(optional) sudo su
#docker build . -t mat9software:latest

WORKDIR /docker_install

# jammy is $(lsb_release -cs)
RUN echo "deb https://apache.bintray.com/couchdb-deb jammy main" | tee -a /etc/apt/sources.list


RUN apt update

RUN apt install -y ca-certificates

RUN apt update

RUN apt install -y vim wget git build-essential python2.7
RUN apt install -y libsdl2-dev libsdl2-image-dev

RUN git clone https://github.com/emscripten-core/emsdk.git
RUN git config --global user.name "Mathieu Drapeau"
RUN git config --global user.email m.drapeau9@gmail.com 

RUN ln -s /usr/bin/python2.7 /usr/bin/python2 
RUN ln -s /usr/bin/python2 /usr/bin/python

WORKDIR /docker_install/emsdk

RUN ./emsdk install latest
RUN ./emsdk activate latest
RUN echo 'source "/docker_install/emsdk/emsdk_env.sh"' >> $HOME/.bashrc

RUN apt install -y cmake tmux

RUN apt install -y clang-format locate universal-ctags

RUN git config --global --add safe.directory /srv

RUN echo 'export EDITOR=vim'
RUN echo "alias sf='find -iname \"\$@\"'"


RUN apt install -y gdb

RUN apt install -y nginx
COPY site_stocks /etc/nginx/sites-available/default

COPY vimrc /root/.vimrc
COPY vimrc /root/.gdbinit


#RUN git config --global user.name ""
#RUN git config --global user.email ""

ENTRYPOINT service nginx restart && bash

