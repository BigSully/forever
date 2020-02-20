


Build:<br/>
cc main.c -framework CoreFoundation -O -o forever

Usage:<br/>
CWD=./myproject/ ./forever /usr/bin/BINARY_NAME options<br/>
Example:<br/>
CWD=./myproject/ ./forever ~/.nvm/versions/node/v8.14.0/bin/node server.js -t<br/>

Note:<br/>
Make sure monitor the right process, the executable, don't exit. Cause we will monitor its pid.
 