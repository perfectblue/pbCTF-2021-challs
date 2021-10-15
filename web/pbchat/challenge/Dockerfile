FROM ubuntu:focal as builder

RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y curl build-essential rpm
RUN curl -fsSL https://deb.nodesource.com/setup_16.x | bash -
RUN DEBIAN_FRONTEND=noninteractive apt install -y nodejs

RUN mkdir /app/
COPY package.json package-lock.json /app/
WORKDIR /app
RUN npm install

COPY ./resources /app/resources
COPY ./assets /app/assets
COPY ./src /app/src
COPY ./index.html ./main.js  ./preload.js ./preload-chat.js ./webpack.common.js ./webpack.prod.js /app/

RUN npm run build && npm run make:linux

FROM ubuntu:focal

RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y curl tightvncserver icewm \
                                  libgtk-3-0 libnotify4 libnss3 libxss1 libxtst6 xdg-utils \
                                 libatspi2.0-0 libuuid1 libappindicator3-1 libsecret-1-0
COPY --from=builder /app/dist/pbchat_1.0.0_amd64.deb /tmp/pbchat_1.0.0_amd64.deb
# COPY ./pbchat_1.0.0_amd64.deb /tmp/pbchat_1.0.0_amd64.deb
RUN DEBIAN_FRONTEND=noninteractive apt install -y /tmp/pbchat_1.0.0_amd64.deb

COPY ./flag.txt /flag.txt
RUN chmod 400 /flag.txt

COPY ./read_flag /read_flag
RUN chmod 4555 /read_flag

RUN useradd -ms /bin/bash ctf
USER ctf

ENV USER=ctf
ENV HOME=/home/ctf
COPY .pbchatrc $HOME/.pbchatrc

RUN mkdir $HOME/.vnc/
RUN /bin/bash -c 'vncpasswd -f <<<"hunter2" >"$HOME/.vnc/passwd" && chmod 600 $HOME/.vnc/passwd'
RUN echo 'icewm&' > "$HOME/.vnc/xstartup" && chmod +x $HOME/.vnc/xstartup

CMD /bin/sh -c 'vncserver :10 && DISPLAY=:10 pbchat'
