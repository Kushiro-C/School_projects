\echo '\n Ajoute dans la table Parle, pour la dernière personne une Langue LangueBidon'

INSERT INTO parle (id_personne, nom_langue)
VALUES ((SELECT COUNT(*) FROM personne), 'LangueBidon');

SELECT * FROM parle;

SELECT * FROM langue;

