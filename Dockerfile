FROM balenalib/raspberry-pi-debian:build

# Turn on devices
ENV UDEV=1

RUN mkdir /app
RUN mkdir /build
WORKDIR /app

# Install python and other native module dependencies
RUN install_packages make gcc g++ cmake clang llvm

# Copy all the things
COPY ./ ./

CMD exit 0