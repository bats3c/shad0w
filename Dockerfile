FROM kalilinux/kali-rolling
 
LABEL maintainer="batsec - @_batsec_"

COPY . /root/shad0w

WORKDIR /root/shad0w
RUN ./install.sh

ENTRYPOINT ["python3.8", "./shad0w.py"]