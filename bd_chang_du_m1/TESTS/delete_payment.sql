\echo '\nSuppression du dernier paiement fini (EXPECT NOTICE)'

DELETE FROM paiement
WHERE id_paiement IN (
    SELECT id_paiement
    FROM paiement
    WHERE date_effectue IS NOT NULL
    ORDER BY id_paiement DESC
    LIMIT 1
);


\echo '\nSuppression du dernier paiement en cours'

DELETE FROM paiement
WHERE id_paiement IN (
    SELECT id_paiement
    FROM paiement
    WHERE date_effectue IS NULL
    ORDER BY id_paiement DESC
    LIMIT 1
);
