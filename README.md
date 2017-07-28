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

Insert a key-value:  
&gt; set&nbsp;&nbsp;zx&nbsp;&nbsp;1  
   
Search a key:   
&gt; get&nbsp;&nbsp;zx  
  
Update a key:  
&gt; update&nbsp;&nbsp;zx  
   
Delete a key:  
&gt; del&nbsp;&nbsp;zx  
  
State:  
&gt; stat  
  
Look for help:  
&gt; h  
  
Quit:  
&gt; q
