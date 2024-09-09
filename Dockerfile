
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y tzdata

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y cmake ninja-build gcc-arm-none-eabi jq gh git python3 python3-pip python3-setuptools python3-wheel python3-venv python3-dev

RUN git config --global --add safe.directory /__w/Rafael-IoT-SDK-Internal/Rafael-IoT-SDK-Internal

CMD ["bash"]