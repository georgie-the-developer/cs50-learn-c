FROM gcc:latest

WORKDIR /app

RUN apt update && apt install -y gdb

COPY . .

CMD ["/bin/bash"]
