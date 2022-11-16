drop table if exists Client cascade;
drop table if exists Commande cascade;
drop table if exists OrderItem cascade;
drop table if exists Matiere cascade;
drop table if exists Composition cascade;
drop table if exists Panier cascade;
drop table if exists Produits cascade;
drop table if exists Avis cascade;
drop table if exists Rembours_retour cascade;
drop table if exists SuiviProduit cascade;
drop table if exists Historique cascade;

CREATE TABLE Client (
    id_client serial NOT NULL,
    nom varchar(50) NOT NULL,
    prenom varchar(50) NOT NULL,
    telephone varchar(50) NULL,
    email varchar(50) NOT NULL,
    adresse varchar(50),
    anniversaire date NOT NULL,
    date_inscription date NOT NULL,
    PRIMARY KEY (id_client)
);
CREATE TABLE Commande (
    id_cmd serial NOT NULL,
    id_client integer NOT NULL,
    date_cmd timestamp DEFAULT current_timestamp,
    etat varchar(50) NOT NULL,
    adr_livraison text,
    payee BOOLEAN DEFAULT FALSE,
    PRIMARY KEY (id_cmd),
    FOREIGN KEY (id_client) REFERENCES Client (id_client)
);
CREATE TABLE Produits (
    id_produit serial NOT NULL,
    nom varchar(50) NOT NULL,
    prix decimal(12, 2) NULL,
    marque varchar(50) NULL,
    origine varchar(50) NOT NULL,
    taille varchar(50) NOT NULL,
    poids varchar(50) NOT NULL,
    stock integer,
    PRIMARY KEY (id_produit)
);
CREATE TABLE OrderItem (/*besoin de id_client ?*/
    id_order serial PRIMARY KEY,
    id_cmd integer NOT NULL,
    id_produit integer NOT NULL,
    /*prix decimal(12, 2) NOT NULL,*/  /*prix lors de l'achat*/
    FOREIGN KEY (id_cmd) REFERENCES Commande (id_cmd),
    FOREIGN KEY (id_produit) REFERENCES Produits (id_produit)
);
CREATE TABLE Rembours_retour (
    id_order integer PRIMARY KEY,
    montant decimal(12, 2) NOT NULL,
    statut varchar(50) NULL,
    date_retour TIMESTAMP,
    date_delai TIMESTAMP,
    raison text,
    FOREIGN KEY (id_order) REFERENCES OrderItem (id_order)
);
CREATE TABLE SuiviProduit (
    id_order integer PRIMARY KEY,
    statut varchar(50) NULL,
    date_expedition TIMESTAMP,
    date_livraison TIMESTAMP,
    FOREIGN KEY (id_order) REFERENCES OrderItem (id_order)
);
CREATE TABLE Matiere (
    id_matiere serial PRIMARY KEY,
    nom_matiere text
);
CREATE TABLE Composition (
    id_produit integer NOT NULL,
    id_matiere integer NOT NULL,
    FOREIGN KEY (id_produit) REFERENCES Produits (id_produit),
    FOREIGN KEY (id_matiere) REFERENCES Matiere (id_matiere)
);
CREATE TABLE Historique (
    date_vendu TIMESTAMP NOT NULL,
    prix decimal(12, 2) NOT NULL,
    id_produit integer NOT NULL,
    FOREIGN KEY (id_produit) REFERENCES Produits (id_produit)
);
CREATE TABLE Panier (
    id_panier serial PRIMARY KEY,
    id_produit integer NOT NULL,
    id_client integer NULL,
    quantite integer NOT NULL,
    FOREIGN KEY (id_client) REFERENCES Client (id_client),
    FOREIGN KEY (id_produit) REFERENCES Produits (id_produit)
);
CREATE TABLE Avis (
    id_client integer NOT NULL,
    id_order integer NOT NULL,
    id_produit integer NOT NULL,
    note integer,
    commentaire text,
    FOREIGN KEY (id_order) REFERENCES OrderItem (id_order),
    FOREIGN KEY (id_client) REFERENCES Client (id_client),
    FOREIGN KEY (id_produit) REFERENCES Produits (id_produit),
    PRIMARY KEY (id_client, id_produit)
);

/*
\COPY Client(nom, prenom, telephone, email, adresse, anniversaire, date_inscription) FROM 'csv/Client.csv' DELIMITER ',' CSV HEADER;
\COPY Produits(poids, nom, prix, marque, origine, stock, taille) FROM 'csv/Produits.csv' DELIMITER ',' CSV HEADER;
\COPY Panier(id_produit,id_client,quantite) FROM 'csv/Panier.csv' DELIMITER ',' CSV HEADER;
\COPY Matiere(nom_matiere) FROM 'csv/Matiere.csv' DELIMITER ',' CSV HEADER;
\COPY Composition(id_produit, id_matiere) FROM 'csv/Composition.csv' DELIMITER ',' CSV HEADER;
\COPY Commande(id_client,date_cmd,adr_livraison,payee,etat) FROM 'csv/Commande.csv' DELIMITER ',' CSV HEADER;
\COPY OrderItem(id_cmd,id_produit) FROM 'csv/OrderItem.csv' DELIMITER ',' CSV HEADER;
\COPY Historique(prix,id_produit,date_vendu) FROM 'csv/Historique.csv' DELIMITER ',' CSV HEADER;
\COPY SuiviProduit(id_order,statut,date_expedition,date_livraison) FROM 'csv/SuiviProduit.csv' DELIMITER ',' CSV HEADER;
\COPY Avis(id_client,id_order,commentaire,note,id_produit) FROM 'csv/Avis.csv' WITH DELIMITER ',' NULL AS 'null' CSV  HEADER;
\COPY Rembours_retour(id_order,montant,statut,date_retour,date_delai,raison) FROM 'csv/Rembours_retour.csv' DELIMITER ',' CSV HEADER;
*/

/*UPDATE Avis set commentaire = NULL WHERE commentaire = 'null';*/

/*\COPY (SELECT prix, id_produit,current_date FROM Produits) TO 'test.txt' WITH DELIMITER ',';*/