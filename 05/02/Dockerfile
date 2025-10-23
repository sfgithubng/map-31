FROM ubuntu:25.04
RUN apt-get update -y
RUN apt-get install -y g++ clang vim
RUN /usr/bin/mkdir -p /opt/testproject
WORKDIR /opt/testproject/
COPY 01.cpp /opt/testproject/01.cpp
COPY container_run.sh /opt/testproject/container_run.sh
RUN /usr/bin/g++ 01.cpp -o 01_gpp
RUN /usr/bin/clang++ 01.cpp -o 01_clang
RUN chmod +x container_run.sh
CMD "./container_run.sh"
