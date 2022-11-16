\echo '\n Ajoute dans la table A_fait, pour la dernière personne une Formation FormationBidon'

INSERT INTO a_fait (nom_formation, id_personne)
VALUES ('FormationBidon', (SELECT COUNT(*) FROM personne));

SELECT * FROM a_fait;

SELECT * FROM formation;

