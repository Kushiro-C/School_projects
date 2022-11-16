var mysql = require('mysql');

var connection = mysql.createConnection({
	host: 'localhost', // Change if needed
	user: 'chang', // Change if needed
	password: '', // Change if needed
	database: 'Vincent_Patrick'
});

module.exports = connection;