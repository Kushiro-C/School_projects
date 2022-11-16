CPP=g++ --std=c++11
CFLAGS= -Wall

all : main

main : main.o Personnage.o Objet.o Equipement.o Cle.o Bouteille.o Amazone.o Guerrier.o Moine.o Sorciere.o Partie.o Conteneur.o
	$(CPP) $(CFLAGS) -o $@ $^

main.o : main.cpp
	$(CPP) $(CFLAGS) -c $<

Personnage.o : Personnage.cpp Personnage.hpp
	$(CPP) $(CFLAGS) -c $<

Objet.o : Objet.cpp Objet.hpp
	$(CPP) $(CFLAGS) -c $<

Equipement.o : Equipement.cpp Equipement.hpp
	$(CPP) $(CFLAGS) -c $<

Cle.o : Cle.cpp Cle.hpp
	$(CPP) $(CFLAGS) -c $<

Bouteille.o : Bouteille.cpp Bouteille.hpp
	$(CPP) $(CFLAGS) -c $<

Amazone.o : Amazone.cpp Amazone.hpp
	$(CPP) $(CFLAGS) -c $<

Guerrier.o : Guerrier.cpp Guerrier.hpp
	$(CPP) $(CFLAGS) -c $<

Moine.o : Moine.cpp Moine.hpp
	$(CPP) $(CFLAGS) -c $<

Sorciere.o : Sorciere.cpp Sorciere.hpp
	$(CPP) $(CFLAGS) -c $<

Partie.o : Partie.cpp Partie.hpp
	$(CPP) $(CFLAGS) -c $<

Conteneur.o : Conteneur.cpp Conteneur.hpp
	$(CPP) $(CFLAGS) -c $<

tester.o : tester.cpp
	$(CPP) $(CFLAGS) -c $<

tester : tester.o Personnage.o Objet.o Equipement.o Cle.o Bouteille.o Amazone.o Guerrier.o Moine.o Sorciere.o Partie.o Conteneur.o
	$(CPP) $(CFLAGS) -o $@ $^

clean :
	rm *.o

test : tester
	./tester
