// Autogenerated by Mix
contracts = {};
ctrAddresses = {};
contracts["Sample"] = {
	interface: [{"constant":false,"inputs":[{"name":"v","type":"uint256"}],"name":"set","outputs":[],"type":"function"},{"constant":true,"inputs":[],"name":"get","outputs":[{"name":"","type":"uint256"}],"type":"function"},{"inputs":[{"name":"v","type":"uint256"}],"type":"constructor"}],
	address: "0xc702942ec216c6a701ffa772c0a9f30580619228"
};
contracts["Sample"].contractClass = web3.eth.contract(contracts["Sample"].interface);
contracts["Sample"].contract = contracts["Sample"].contractClass.at(contracts["Sample"].address);
contracts["<Sample - 0>"] = {
	interface: [{"constant":false,"inputs":[{"name":"v","type":"uint256"}],"name":"set","outputs":[],"type":"function"},{"constant":true,"inputs":[],"name":"get","outputs":[{"name":"","type":"uint256"}],"type":"function"},{"inputs":[{"name":"v","type":"uint256"}],"type":"constructor"}],
	address: "0xc702942ec216c6a701ffa772c0a9f30580619228"
};
contracts["<Sample - 0>"].contractClass = web3.eth.contract(contracts["<Sample - 0>"].interface);
contracts["<Sample - 0>"].contract = contracts["<Sample - 0>"].contractClass.at(contracts["<Sample - 0>"].address);
if (!ctrAddresses["Sample"]){ ctrAddresses["Sample"] = [] }ctrAddresses["Sample"].push("0xc702942ec216c6a701ffa772c0a9f30580619228")