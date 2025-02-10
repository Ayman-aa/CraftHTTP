FROM dahum/webserver:v1
RUN rm -rf default.yaml
COPY ./thisConf.yaml ./default.yaml

EXPOSE 9010

CMD ["./webserv", "-conf", "./thisConf.yaml"]