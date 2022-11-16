\echo '\nAjout d\'une nouvelle representation'

INSERT INTO Represente(id_artiste,id_agent,date_debut_represente,date_fin_represente)
VALUES ((SELECT COUNT(*) FROM Artiste),(SELECT COUNT(*) FROM Agent),'2019-04-02','2019-05-02');


\echo '\nAjout d\'une nouvelle representation avec un ex-agent'

INSERT INTO Represente(id_artiste,id_agent,date_debut_represente,date_fin_represente)
VALUES ((SELECT COUNT(*) FROM Artiste),(SELECT COUNT(*) FROM Agent),'2019-11-02','2019-12-02');

\echo '\nAjout d\'une nouvelle representation chevauchant une autre (EXPECT NOTICE)'

INSERT INTO Represente(id_artiste,id_agent,date_debut_represente,date_fin_represente)
VALUES ((SELECT COUNT(*) FROM Artiste),(SELECT COUNT(*) FROM Agent),'2019-04-02','2019-06-02');