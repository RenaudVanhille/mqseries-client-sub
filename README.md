Si le serveur est distant :
export MQSERVER='SYSTEM.DEF.SVRCONN/TCP/192.168.1.44'

Install (après MQ) :
echo '/opt/mqm/lib64' >> /etc/ld.so.conf.d/mqm.conf
ldconfig

Compile :
node-gyp configure && node-gyp build

Execute :
node index.js