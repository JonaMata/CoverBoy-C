FROM balenalib/raspberry-pi-debian:build

# Turn on devices
ENV UDEV=1

RUN mkdir /app
RUN mkdir /build
WORKDIR /app

# Install python and other native module dependencies
RUN install_packages make gcc g++ cmake

# Copy all the things
COPY ./ ./

# Install and build
RUN cmake .
cmake -S . -B /build
RUN cmake --build /build --target CoverBoy_C -j 10

CMD exit 0