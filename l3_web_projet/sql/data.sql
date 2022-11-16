DROP DATABASE IF EXISTS `Vincent_Patrick`;

CREATE DATABASE IF NOT EXISTS `Vincent_Patrick` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `Vincent_Patrick`;

drop table if exists person cascade;
drop table if exists subscriptions cascade;
drop table if exists publications cascade;

/*email text NOT NULL,*/
create table person(
pid serial,
lastName text NOT NULL, 
firstName text NOT NULL,
pwd text NOT NULL,
email VARCHAR(100),
nickname VARCHAR(100) NOT NULL,
PRIMARY KEY(nickname)
);


create table subscriptions(
nickname VARCHAR(100),
followed_nickname VARCHAR(100),
FOREIGN KEY (nickname) REFERENCES person(nickname),
FOREIGN KEY (followed_nickname) REFERENCES person(nickname)
);


create table publications(
pub_id serial,
nickname VARCHAR(100),
message TEXT,
nLike integer DEFAULT 0,
nDislike integer DEFAULT 0,
date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
update_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
FOREIGN KEY (nickname) REFERENCES person(nickname)
);
