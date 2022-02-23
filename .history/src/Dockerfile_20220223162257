FROM gcc:latest
RUN mkdir /var/log/erss
RUN touch /var/log/erss/proxy.log
RUN make clean
RUN make
CMD ["echo "Starting server now""]
CMD ["./proxy"]