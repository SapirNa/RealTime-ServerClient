# RealTime-ServerClient
# by - Sapir Naugauker

this is demo of smart-home system base on server-clinet.

main_server : 
is the server that get all to messages from clients and do all the work.

client_light :
run on backgrond, when get message from main_server he can do:
  1. send light-on
  2. send light-off

client_move :
send to the main_server that there is a person move in the house.

client_log :
send to the main_server the log file that he save

client_write :
free text from the user to the main_server.
  1. the user can write free text and the main_server return the same message
  2. the user can write "light-on"/"light-off" and the main_server will call the client_light

client_shutdown :
send to the main_server to kill all the client and himself


to run this code:
    * compile each file individually
    * run the main_server - it will not stop until you press CTRL+C
    * run the client_light - it will not stop until you press CTRL+C
    * run ervery client you wants 
