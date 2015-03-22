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
1.	Nous avons développé une petite fonction qui nous permet d'obtenir directement le coeur le moins chargé à la création d'un nouveau
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
    
    int balance_ctx() {
        unsigned corei, corej, ctx_balanced_count = 0;
    
        for(corei = 0; corei < CORE_NCORE; corei++) {
            for(corej = corei + 1; corej < CORE_NCORE; corej++) {
                int diff = ctx_load[corei] - ctx_load[corej];
            	printf("Load average difference between the core : %d - %d with diff %d\n", corei, corej, diff);
    
                if(diff > 1) {
                    if(swap_ctx(corei, corej) == RETURN_FAILURE) {
                        fprintf(stderr, "Error when balancing the load on the core : %d - %d with diff %d\n", corei, corej, diff);
                        continue;
                    }
                    ctx_balanced_count++;
                }
            }
        }
    
        return ctx_balanced_count;
    }
    
```

2.	Nous avons développé un terminal avec une commande top qui permet de lister sur chaque coeur les processus actifs avec leur état

```C
    
    static void top(struct _cmd *c) 
```


Exemples d'exécution
--------------------

###Prodcons
```
Initialize the core...
Start the core #3
Start the core #2
Current ctx put on core #1
coreid : 1   core add : 0x8e71760
Current ctx put on core #2
coreid : 2   core add : 0x8e75798
Start the scheduler ...
Start the core #1
	Load average difference between the core : 0 - 1 with diff -1
	Load average difference between the core : 0 - 2 with diff -1
	Load average difference between the core : 0 - 3 with diff 0
	Load average difference between the core : 1 - 2 with diff 0
	Load average difference between the core : 1 - 3 with diff 1
	Load average difference between the core : 2 - 3 with diff 1
	Load average difference between the core : 0 - 1 with diff -1
	Load average difference between the core : 0 - 2 with diff -1
	Load average difference between the core : 0 - 3 with diff 0
	Load average difference between the core : 1 - 2 with diff 0
	Load average difference between the core : 1 - 3 with diff 1
	Load average difference between the core : 2 - 3 with diff 1
start : 1
Start the current context on the core #1
	Load average difference between the core : 0 - 1 with diff -1
	Load average difference between the core : 0 - 2 with diff -1
	Load average difference between the core : 0 - 3 with diff 0
	Load average difference between the core : 1 - 2 with diff 0
	Load average difference between the core : 1 - 3 with diff 1
	Load average difference between the core : 2 - 3 with diff 1
start : 2
Start the current context on the core #2
ping : 1
pong : 2
```

Cette éxécution de prodcons affiche plusieurs informations :

1.	Tous les coeurs sont démarrés, le contexte ping est sur le coeur 1 et pong sur le 2. Il n'y a rien sur le coeur 3 et 4.
2.	Le balanceur de charge est également mis en place : "load average..."
3.	Une fois tous les coeurs balancés et les contextes mis en place, les commandes ping/pong sont exécutées respectivement sur le coeur 1 et 2.
4.	Ping : 1, correspond à l'appel de la méthode ping, sur le coeur 1. Pong : 2, correspond à l'appel de la méthode pong, sur le coeur 2



###Shell

####Multicore
```
	start : 3
	Start the current context on the core #3
	new
	new
	Current ctx put on core #1
	coreid : 1   core add : 0x8842760
	> new
	new
	Current ctx put on core #2
	coreid : 2   core add : 0x8846798
	> new
	new
	Current ctx put on core #3
	coreid : 3   core add : 0xb5b0c2d8
	> new
	new
	Current ctx put on core #1
	coreid : 1   core add : 0x8842760
	> job finished
```

1.	La commande "new" permet de lancer un nouveau contexte qui éxecute la methode "compute" (une simple boucle qui dure un certains temps).
2.	Comme on peut observer, chaque nouveau contexte de la commande compute est placée sur chaque coeur(1, puis 2, ensuite 3, puis de nouveau 1)
   Nous considérons que seul le coeur 0 est utilisé pour la boucle "loop", permettant d'écrire des commandes à exécuter.
3.	A la fin de cet exemple, on peut voir que le premier contexte se termine, en affichant "job finished".


####Mise en background

```
	&compute
	Current ctx put on core #3
	coreid : 3   core add : 0xb5b0c2d8
	> Load average difference between the core : 0 - 1 with diff -1
	Load average difference between the core : 0 - 2 with diff -1
	Load average difference between the core : 0 - 3 with diff -1
	Load average difference between the core : 1 - 2 with diff 0
	Load average difference between the core : 1 - 3 with diff 0
	Load average difference between the core : 2 - 3 with diff 0
	start : 3
	Start the current context on the core #3
	compute
	job finished
```

1.	'&' permet de placer une commande en background, et de l'exécuter.
2.	On peut clairement voir que notre algorithme de répartition des charges est exécutés également (load average...)
3.	Il en résulte que les autres coeurs sont tous également occupés, de ce fait, compute continue d'être exécuté sur le coeur 3 et se termine.


