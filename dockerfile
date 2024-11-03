FROM alpine:latest

WORKDIR /app

COPY . .

RUN apk add --no-cache g++

RUN g++ -o main main.cpp

EXPOSE 8080

CMD ["./main"]