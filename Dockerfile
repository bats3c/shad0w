FROM kalilinux/kali-rolling
 
LABEL maintainer="batsec - @_batsec_"

# https://docs.microsoft.com/en-us/dotnet/core/tools/telemetry#how-to-opt-out
ENV DOTNET_CLI_TELEMETRY_OPTOUT=1  

# Install dependencies as a base container layer before copying full shad0w source
COPY ./install.sh ./requirements.txt /root/shad0w/
WORKDIR /root/shad0w
RUN ./install.sh

COPY . /root/shad0w

ENTRYPOINT ["python3", "./shad0w.py"]
