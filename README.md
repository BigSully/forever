Monitor a program and resume it when it dies, either exiting abnormally or killed by OOM in linux.

**Usage:**<br/>
./forever /usr/bin/BINARY_NAME options<br/>
Example:<br/>
CWD=./myproject/ ./forever ~/.nvm/versions/node/v8.14.0/bin/node server.js -t<br/>

**Note:**<br/>
Make sure monitor the right process, the executable, don't exit. Cause we will monitor its pid.


Build universal version:<br/>
cc forever.c -O -o forever

**Deprecated:**<br/>
Build osx version:<br/>
cc forever-osx.c -framework CoreFoundation -O -o forever-osx



