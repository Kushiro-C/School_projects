var connection = require('./config');
var express = require('express');
var server = express();
var session = require('express-session');
var bodyParser = require('body-parser');

server.use(session({
    secret: 'secret',
    resave: true,
    saveUninitialized: true
}));
server.use(express.urlencoded({ extended: true }));
server.use(express.static('images/background'));
server.use(express.static('images/profils'));
server.use(express.static('views/css'));
server.use(express.static('views/script'));
server.use(express.static('sql'));
server.set('view engine', 'ejs');

// Configuration BodyParser
server.use(bodyParser.json());
server.use(
    bodyParser.urlencoded({
        extended: true
    })
);

connection.connect();
var users;

/**
 * get all users
 **/
connection.query('SELECT * FROM person;', function(error, results, fields) {
    if (results.length > 0) {
        users = results;
    } else {
        res.send('Pas d\'utilisateurs disponibles.');
    }
});


server.get("/", function(req, res) {
    if (req.session.isConnected) {
        connection.query('SELECT DISTINCT * FROM publications WHERE UPPER(message) LIKE UPPER(\'%@everyone%\') OR UPPER(message) LIKE UPPER(\'%' + req.session.nickname + '%\') ORDER BY (date) DESC;', function(error, results, fields) {
            if (results.length > 0) {
                res.render('home.ejs', { publications: results, isConnected: req.session.isConnected, userData: req.session.userData, listUsers: users });
            } else {
                res.send('Pas de publications disponibles.');
            }
        });
    } else {
        console.log('SELECT * FROM publications WHERE UPPER(message) LIKE UPPER(\'%@everyone%\');');
        connection.query('SELECT * FROM publications WHERE UPPER(message) LIKE UPPER(\'%@everyone%\') ORDER BY (date) DESC;', function(error, results, fields) {
            if (results.length > 0) {
                res.render('home.ejs', { publications: results, isConnected: req.session.isConnected, userData: req.session.userData, listUsers: users });
            } else {
                res.send('Pas de publications disponibles.');
            }
        });
    }
});

/**
 * Logout session
 */
server.get('/logout', function(req, res) {
    req.session.destroy(function(err) {
        if (err)
            return console.log(err);

        console.log('Logging out');
        res.redirect('/');
    });
});

/**
 * authentification by post method
 */
server.post('/auth', function(req, res) {
    var email = req.body.email;
    var password = req.body.password;
    if (email && password) {
        connection.query(
            `SELECT * FROM person WHERE email = '${email}' AND pwd = MD5('${password}');`,
            function(error, results, fields) {

                if (results.length > 0) {
                    req.session.isConnected = true;
                    req.session.nickname = results[0].nickname;
                    req.session.userData = results;
                    console.log('SELECT DISTINCT * FROM publications WHERE UPPER(message) LIKE UPPER(\'%@everyone%\') OR UPPER(message) LIKE UPPER(\'%' + req.session.nickname + '%\') ;');
                    connection.query('SELECT DISTINCT * FROM publications WHERE UPPER(message) LIKE UPPER(\'%@everyone%\') OR UPPER(message) LIKE UPPER(\'%' + req.session.nickname + '%\') ORDER BY (date) DESC;', function(error, results2, fields) {
                        if (results2.length > 0) {
                            res.render('home.ejs', { publications: results2, isConnected: true, userData: results, listUsers: users });
                        } else {
                            res.send('Pas de publications disponibles.');
                        }
                    });
                } else {
                    console.log("Bad authentification");
                }

            });
    } else {
        res.send('error auth !');
        res.end();
    }
});

/**
 * update publications informations (likes, dislikes, etc..)
 * AJAX usage, get publications
 */
server.get('/update', function(req, res) {
    connection.query('SELECT * FROM publications ORDER BY (date) DESC;', function(error, results, fields) {
        if (error) throw error;
        if (req.session.isConnected) {
            connection.query(
                `SELECT * FROM subscriptions WHERE nickname = '${req.session.userData[0].nickname}';`,
                function(error2, results2, fields) {
                    if (error2) throw error2;
                    console.log(req.session.userData[0].nickname);
                    console.log('Updating New Posts');
                    res.send({
                        publications: results,
                        isConnected: req.session.isConnected,
                        listUsers: users,
                        subscriptions: results2
                    });
                }
            );
        } else {
            connection.query('SELECT * FROM subscriptions;', function(error2, results2, fields) {
                if (error2) throw error2;
                console.log('Updating New Posts');
                res.send({
                    publications: results,
                    isConnected: req.session.isConnected,
                    listUsers: users,
                    subscriptions: results2
                });
            });
        }
    });
});

server.post('/publish', function(req, res) {
    connection.query(
        `INSERT INTO publications(nickname, message) VALUES
        ('${req.body.name}', '${req.body.mess}');`,
        function(error) {
            if (error) throw error;
            console.log('Publishing new post');
        }
    );
});

/**
 * Updates counter nLike for a post
 */
server.post('/upLike', function(req, res) {
    connection.query(
        `UPDATE publications SET nLike = ${req.body.likes} WHERE pub_id = ${req.body.id}`,
        function(error) {
            if (error) throw error;
            console.log('Liked a post');

        }
    );
});

/**
 * Updates counter nDislike for a post
 */
server.post('/upDislike', function(req, res) {
    connection.query(
        `UPDATE publications SET nDislike = ${req.body.dislikes} WHERE pub_id = ${req.body.id}`,
        function(error) {
            if (error) throw error;
            console.log('Disliked a post');

        }
    );
});

server.listen(8080);