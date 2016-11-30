var http = require('http');
var Web3 = require('web3');
// create an instance of web3 using the HTTP provider.
// NOTE in mist web3 is already available, so check first if its available before instantiating
var web3 = new Web3(new Web3.providers.HttpProvider("http://localhost:8545"));


contracts = {};
ctrAddresses = {};
contracts["Sample"] = {
	interface: [{"constant":false,"inputs":[{"name":"v","type":"uint256"}],"name":"set","outputs":[],"type":"function"},{"constant":true,"inputs":[],"name":"get","outputs":[{"name":"","type":"uint256"}],"type":"function"},{"inputs":[{"name":"v","type":"uint256"}],"type":"constructor"}],
	address: "0xc702942ec216c6a701ffa772c0a9f30580619228"
};
contracts["Sample"].contractClass = web3.eth.contract(contracts["Sample"].interface);
contracts["Sample"].contract = contracts["Sample"].contractClass.at(contracts["Sample"].address);

if (!ctrAddresses["Sample"]){ ctrAddresses["Sample"] = [] }ctrAddresses["Sample"].push("0xc702942ec216c6a701ffa772c0a9f30580619228")

http.createServer(function (request, response) {

	var account = web3.eth.accounts;
	var respp = contracts['Sample'].contract.get();
	console.log(account);
	console.log(respp.c[0]);
	response.writeHead(200, {'Content-Type': 'text/plain'});
response.end('a' + respp.c[0]);
	//response.end(account[0]);

}).listen(8888);
