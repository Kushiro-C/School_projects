\echo '\nModification du dernier paiement en cours'

UPDATE paiement
SET type_paiement = 'commissions', date_prevu = '2021-01-01',
    date_effectue = null, montant = 50
WHERE id_paiement IN (
    SELECT id_paiement
    FROM paiement
    WHERE date_effectue IS NULL
    ORDER BY id_paiement DESC
    LIMIT 1
);


\echo '\nModification du dernier paiement en cours gros montant (EXPECT NOTICE)'

UPDATE paiement
SET type_paiement = 'commissions', date_prevu = '2021-01-01',
    date_effectue = null, montant = 10000
WHERE id_paiement IN (
    SELECT id_paiement
    FROM paiement
    WHERE date_effectue IS NULL
    ORDER BY id_paiement DESC
    LIMIT 1
);


\echo '\nModification du dernier paiement fini (EXPECT NOTICE)'

UPDATE paiement
SET type_paiement = 'commissions', date_prevu = '2021-01-01',
    date_effectue = null, montant = 50
WHERE id_paiement IN (
    SELECT id_paiement
    FROM paiement
    WHERE date_effectue IS NOT NULL
    ORDER BY id_paiement DESC
    LIMIT 1
);
