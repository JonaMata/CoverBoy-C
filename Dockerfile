FROM balenalib/raspberry-pi-debian:build

# Turn on devices
ENV UDEV=1

RUN mkdir /app
WORKDIR /app

# Install python and other native module dependencies
RUN install_packages build-essential devscripts debhelper make gcc g++ cmake clang llvm


CMD exit 0