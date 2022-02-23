FROM gcc:latest

RUN mkdir /var/log/erss
RUN touch /var/log/erss/proxy.log
add . /var/log/erss
WORKDIR /var/log/erss
RUN chmod o+x run.sh

# ENTRYPOINT ["./run.sh"]
