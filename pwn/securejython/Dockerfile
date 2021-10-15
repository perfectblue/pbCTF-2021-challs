FROM ubuntu:20.10

RUN apt-get update
RUN apt-get install -y --no-install-recommends openjdk-16-jre-headless xinetd

RUN useradd ctf

RUN mkdir /app
WORKDIR /app
ADD challenge/jython.jar challenge/jython challenge/flag.txt ./
ADD challenge/xinetd /etc/xinetd.conf
EXPOSE 1337
ENTRYPOINT ["/usr/sbin/xinetd", "-dontfork"]
