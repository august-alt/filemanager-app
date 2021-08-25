# Container image that runs your code
FROM alt:p9

RUN apt-get update \
    && apt-get install -y git \
    gear \
    hasher \
    hasher-priv \
    hasher-rich-chroot \
    hasher-rich-chroot-user-utils \
    rpm-utils \
    rpm-build \
    rpm-build-licenses \
    rpm-macros-cmake \
    rpm-macros-make \
    rpm-macros-generic-compat \
    apt-repo \
    apt-repo-tools \
    sudo \
    && export CURRENT_PWD=`pwd` \
    && useradd -ms /bin/bash builder2 \
    && groupadd sudo \
    && usermod -aG rpm builder2 \
    && usermod -aG sudo root \
    && usermod -aG sudo builder2 \
    && echo "root ALL=(ALL) NOPASSWD:ALL" | tee -a /etc/sudoers \
    && echo "builder2 ALL=(ALL) NOPASSWD:ALL" | tee -a /etc/sudoers \
    && hasher-useradd builder2 \
    && mkdir /app \
    && chown root:builder2 /app

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY script/buildalt.sh /build.sh

USER builder2
WORKDIR /home/builder2

# Code file to execute when the docker container starts up (`build.sh`)
ENTRYPOINT ["/build.sh"]