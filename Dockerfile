FROM ubuntu:25.10

RUN apt-get update \
    && apt-get install -y sudo git ssh vim cmake clang-format clang-tidy clang clangd libclang-dev lld lldb llvm-dev \
    && echo ubuntu ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu \
    && usermod --shell /bin/bash ubuntu

USER ubuntu
