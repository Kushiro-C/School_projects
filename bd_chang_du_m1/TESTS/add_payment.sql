\echo '\nAjout d\'un nouveau contrat'

INSERT INTO contrat(date_debut_contrat, date_fin_contrat, remuneration)
VALUES ('2020-10-10', null, 1000);


\echo '\nAjout d\'un paiement en cours pour le nouveau contrat'

INSERT INTO paiement(id_contrat, type_paiement, date_prevu, date_effectue, montant)
VALUES ((SELECT COUNT(*) FROM contrat), 'frais', '2019-04-02', null, 200);


\echo '\nAjout d\'un paiement fini'

INSERT INTO paiement(id_contrat, type_paiement, date_prevu, date_effectue, montant)
VALUES ((SELECT COUNT(*) FROM contrat), 'frais', '2019-04-02', '2019-04-02', 200);


\echo '\nAjout d\'un paiement négatif (EXPECT NOTICE)'

INSERT INTO paiement(id_contrat, type_paiement, date_prevu, date_effectue, montant)
VALUES ((SELECT COUNT(*) FROM contrat), 'frais', '2019-04-02', null, -200);


\echo '\nAjout d\'un paiement pour contrat fini (EXPECT NOTICE) (id_contrat=1)'

INSERT INTO paiement(id_contrat, type_paiement, date_prevu, date_effectue, montant)
VALUES (
    (SELECT id_contrat FROM paiement WHERE date_effectue IS NOT NULL LIMIT 1)
    , 'frais', '2019-04-02', null, 200
);


\echo '\nAjout d\'un paiement qui dépasse le contrat (EXPECT NOTICE)'

INSERT INTO paiement(id_contrat, type_paiement, date_prevu, date_effectue, montant)
VALUES ((SELECT COUNT(*) FROM contrat), 'frais', '2019-04-02', null, 200000);
