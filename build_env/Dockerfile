FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN : \
  && apt-get update \
  && apt-get install -y \
    apt-utils \
    bison \
    bzip2 \
    ca-certificates \
    ccache \
    check \
    curl \
    flex \
    git \
    git-lfs \
    gperf \
    lcov \
    libbsd-dev \
    libffi-dev \
    libglib2.0-0 \
    libncurses-dev \
    libpixman-1-0 \
    libsdl2-2.0-0 \
    libslirp0 \
    libusb-1.0-0-dev \
    make \
    ninja-build \
    python3 \
    python3-venv \
    ruby \
    unzip \
    wget \
    xz-utils \
    zip \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/* \
  && update-alternatives --install /usr/bin/python python /usr/bin/python3 10 \
  && :

ADD vendor/espressif/esp-idf /opt/esp-idf

ENV IDF_PATH=/opt/esp-idf
ENV IDF_INSTALL_TARGETS=esp32s3

# Install all the required tools
RUN : \
  && update-ca-certificates --fresh \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install required --targets=${IDF_INSTALL_TARGETS} \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install qemu* --targets=${IDF_INSTALL_TARGETS} \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install cmake \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install-python-env \
  && :

# Add get_idf as alias
RUN echo 'alias get_idf=". /opt/esp-idf/export.sh"' >> /root/.bashrc

# The constraint file has been downloaded and the right Python package versions installed. No need to check and
# download this at every invocation of the container.
ENV IDF_PYTHON_CHECK_CONSTRAINTS=no

# Ccache is installed, enable it by default
ENV IDF_CCACHE_ENABLE=1

COPY ./build_env/entrypoint.sh /opt/esp-idf/entrypoint.sh
RUN chmod +x /opt/esp-idf/entrypoint.sh
ENTRYPOINT [ "/opt/esp-idf/entrypoint.sh" ]
CMD [ "/bin/bash" ]