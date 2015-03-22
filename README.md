# ASE++ - Multithread
=====================
* Développement d'une gestion de contextes en multi-thread
* Antoine Durigneux & Quentin Warnant
* 22/03/15
* La version en ligne (syntaxe, couleurs) de ce README est disponible à l'adresse https://github.com/qwarnant/ASA-Multithread

HOW TO
------------
```bash
    ./bin/prodcons 
    ./bin/shell
```
* L'exécutable prodcons correspond à une implémentation de l'algorithme consommateur-producteur en multi-thread.
* L'exécutable shell correspond à un terminal interactif avec prompt qui permet de lancer des processus et voir l'état de ceux-ci.

Introduction
------------
Cette librairie logicielle est une implémentation de la gestion multi-thread de processus avec des contextes en langage C.

Architecture
------------
* bin
* etc
* include 
* lib
* obj
* src

Etat du projet
--------------
Nous avons implémenté tout ce qui était demandé sauf le spin-lock qui aurait du permettre d'avoir une attente active momentanée
pour certains processus où il est plus couteux de changer de contexte que d'attendre quelques millisecondes en plus. 

Notre gestion de changement de contexte multi-thread fonctionne avec des processus qui ne demandent pas d'allouer une grande pile
comme ping, pong, pang dans prodcons. Dès qu'on tente de faire des contextes plus complexes, nous obtenons des erreurs de segmentation
comme dans le terminal où une mystérieuse erreur provient de la librairie scanf lorsque nous lançons trop de contexte depuis le terminal.
    
Code samples
------------
1. Nous avons développé une petite fonction qui nous permet d'obtenir directement le coeur le moins chargé à la création d'un nouveau
contexte, on a donc un système qui est toujours correctement balancé.

```C

    int get_available_core(){
    
    	unsigned i = 1, min = 1000, coreid=1;
    	for(i = 1; i < CORE_NCORE; i++){
    		unsigned load = ctx_load[i];
    		if(min > load){
    			min = load;
    			coreid = i;
    		}
    	}
    
    	return coreid;
    }
```

2. Nous avons développé un terminal avec une commande top qui permet de lister sur chaque coeur les processus actifs avec leur état

```C
    
    static void top(struct _cmd *c) 
```


Exemples d'éxécution
--------------------

