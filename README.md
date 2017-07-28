# Explorer
Explorer is a key-value storage system with client and server.   
Explorer use B+ tree as storage engine. Client and server communicate using the TCP protocol.
## Compile
make

## Example  
First, run the server program :  
./exp-srv
Then, run the client program :   
./exp-cli   
Insert   
& gt set zx 1  
Search  
> get zx  
Update  
> update zx  
Delete  
> del zx  
State  
> stat  
Help  
> h  
Quit  
> q
