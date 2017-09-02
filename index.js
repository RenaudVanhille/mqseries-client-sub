const pub = require('./build/Release/mq_sub.node');

function SendPub(i) {
        return new Promise(
                (resolve, reject) => {
                        pub.send('message ' + i).then(
                                () => { setTimeout(() => { return SendPub(++i); }, 1000) },
                                (reason) => { reject(reason); }
                        );
                }
        );
}

pub.init('QM1', 'sport').then(
        () => {
			pub.on('message', (message) => {
				console.log('message received : ' + message);
			}
		},
        (reason) => { console.error(reason); }
);