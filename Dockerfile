FROM kalilinux/kali-rolling
 
LABEL maintainer="batsec - @_batsec_"

#removed for testing
COPY . /root/shad0w

WORKDIR /root/shad0w
RUN ./install.sh

ENTRYPOINT ["python3.8", "./shad0w.py"]