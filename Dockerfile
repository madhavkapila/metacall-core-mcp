# Dockerfile
FROM metacall/core:latest

RUN apt-get update && apt-get install -y gcc make && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY vendor/ vendor/
COPY src/ src/
COPY Makefile .

RUN make

COPY entrypoint.sh /entrypoint.sh
COPY test/ /app/test/
RUN chmod +x /entrypoint.sh

WORKDIR /app/scripts

# The entrypoint sets up environment and runs the binary
ENTRYPOINT ["/entrypoint.sh"]
