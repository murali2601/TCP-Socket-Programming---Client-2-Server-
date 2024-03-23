all : 
  gcc -o server server.c
  gcc -o client client.c
  > history.txt	

clean : 
  rm server 
  rm client
