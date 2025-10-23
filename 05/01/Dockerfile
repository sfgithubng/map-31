FROM ubuntu:25.04
RUN apt-get update -y
RUN apt-get install -y g++ vim
RUN /usr/bin/mkdir -p /opt/helloworld
WORKDIR /opt/helloworld/
COPY helloworld.cpp /opt/helloworld/helloworld.cpp
RUN /usr/bin/g++ helloworld.cpp -o helloworld
CMD "./helloworld"
