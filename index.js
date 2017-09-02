const sub = require('./build/Release/mq_sub.node');

 sub.on('message', (message) => {
                                console.log('(from index.js) message received : ' + message);
                        });


sub.init('QM1', 'sport').then(
        () => {

                },
        (reason) => { console.error(reason); }
);

var http = require('http');

var server = http.createServer(function(req, res) {
  res.writeHead(200);
  res.end('Salut tout le monde !');
});
server.listen(8080);