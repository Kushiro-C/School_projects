\echo req15 tous les produits étant composé de toutes les matieres avec sous req correlées
SELECT DISTINCT c1.id_produit
FROM Composition c1
WHERE NOT EXISTS (
        SELECT m.id_matiere
        FROM Matiere m
        WHERE NOT EXISTS (
                SELECT c2.id_produit
                FROM Composition c2
                WHERE m.id_matiere = c2.id_matiere
                    AND c1.id_produit = c2.id_produit
            )
    );