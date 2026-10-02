FROM alpine:3.20 
RUN apk add --no-cache g++ make 
WORKDIR /app 
COPY . . 
RUN make 
CMD ["./lab7"]