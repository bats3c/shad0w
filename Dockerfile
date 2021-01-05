FROM kalilinux/kali-rolling
 
LABEL maintainer="batsec - @_batsec_"

COPY . /opt/shad0w

WORKDIR /opt/shad0w
RUN ./install.sh

ENTRYPOINT ["python3.8", "./shad0w.py"]
