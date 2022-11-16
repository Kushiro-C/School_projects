DROP DATABASE IF EXISTS plong_chang_du;
DROP ROLE IF EXISTS plong_chang_du;
CREATE ROLE plong_chang_du LOGIN PASSWORD '1234';
SET ROLE postgres;
ALTER USER plong_chang_du WITH SUPERUSER;
SET ROLE plong_chang_du;
CREATE DATABASE plong_chang_du;
\c plong_chang_du;
SET ROLE plong_chang_du;

drop table if exists musics cascade;
drop table if exists classification cascade;
drop table if exists musics_fingerPr cascade;

create table musics(
music_id serial primary key,
name varchar(100) NOT NULL
);

create table musics_fingerPr(
music_id INTEGER NOT NULL,
fingerPr DOUBLE PRECISION NOT NULL,
foreign key(music_id) references musics(music_id)
);


create table classification(
music_id INTEGER NOT NULL,
genre varchar(25) NOT NULL,
foreign key(music_id) references musics(music_id)
);

CREATE INDEX idx_musics_fingerPr
ON musics_fingerPr USING HASH(fingerPr);