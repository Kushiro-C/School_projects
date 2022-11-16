## Checks :

- Pour les dates, DateDébut < DateFin

- Il y a au minimum 1 Paiement par Contrat

- Les montants ne peuvent etre negatifs

- Les nombres ne sont pas négatifs

---

## Contraintes Extérieurs et Précisions:

- Distinction dans la table Critique (sert comme récompense) peut être NULL, ce qui donne juste une simple critique

- La somme des montants de la table Paiement ne doit pas dépasser la rémunération de la table Contrat

- Les périodes de contrats ne se chevauchent pas

- Pas d'artiste non lié à une personne (un artiste est une personne)

- Une personne parle obligatoirement au moins une langue

- Un musicien joue obligatoirement un instrument

- Un contrat a durée indéterminée n'a pas de date de fin

- Un paiement prévu et non effectué n'a pas de date_effectué

- Une Critique avec une Distinction (non null) est consideré comme une récompense (ex : meilleur film 2017)

- Pas de même nom de récompenses (Distinction dans la table Critique) pour une même Réalisation (Meilleur... de l'année...)

- Une personne n'a pas à se connaître elle même

- Réponse dans l'Audition est null au debut

---

## Uniques :

- Une seule demande par audition/demande

---

## Triggers :

- check_payment() : vérifie si le nombre de paiement est correct et si le montant total correspond au montant final attendu du contrat

- calcul_percent() : calcul un paiment selon un pourcentage

- is_free_to_sign() : verifie si des dates ne se chevauchent pas avant de lier un artiste à un agent

- pay(id_contrat) : réalise le paiement

- plays_instruments(id_artiste, nom) : les artistes qui jouent cet instrument

- speaks(nom) : les artistes parlant une langue nom
