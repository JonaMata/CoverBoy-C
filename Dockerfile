FROM balenalib/raspberry-pi-debian:build

# Turn on devices
ENV UDEV=1

RUN mkdir /app
RUN mkdir /build
WORKDIR /app

# Install python and other native module dependencies
RUN install_packages build-essential devscripts debhelper make gcc g++ cmake clang llvm

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

# Copy all the things
COPY ./ ./

CMD exit 0